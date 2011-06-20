#!/bin/bash

# Copyright (C) 2010 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to run each group of GNU tests manually

# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.          

# ------------------------------------------------------------------------------

# For each block of tests we run the tests on a target machine. There are no
# arguments. The IP address of the target machines are found in the file
# `dirname ${DEJAGNU}`/ip-avail.txt


# Set global variables
function set_globals {
    root_dir=`dirname ${DEJAGNU}`
    ip_file=${root_dir}/ip-avail.txt
    boards_dir=${root_dir}/boards
    board_file=${boards_dir}/or32-linux-sim.exp
    tmp_count=/tmp/rat-sema-$$
    lockfile=/tmp/rat-lockfile-$$
    ip_count=`wc -l ${ip_file} | cut -d " " -f 1`
 }


# Set the count file

# @param[in] $1  The count to set.
function set_count {

    # Lock all the file manipulation.
    (
	flock -e 201
	echo $1 > ${tmp_count}
    ) 201> ${lockfile}
}


# Get the count file

# @return  The current count
function get_count {

    # Lock all the file manipulation.
    (
	cat ${tmp_count}
    ) 201> ${lockfile}
}


# Decrement the count file if it is not already zero

# @return 0 if we suceed in decrementing, 1 otherwise
function dec_count {

    # Lock all the file manipulation. Result will be result of function
    (
	flock -e 201
	echo "**** Decrement started ($$)"
	val=`cat ${tmp_count}`

	if [ ${val} -gt 0 ]
	then
	    (( val-- ))
	    echo ${val} > ${tmp_count}
	    echo "**** Decrement ended ($$)"
	    return  0
	else
	    echo "**** Decrement ended ($$)"
	    return  1
	fi
    ) 201> ${lockfile}
}


# Increment the count file

# @return 0 if we do decrement, 1 if we don't
function inc_count {

    # Lock all the file manipulation.
    (
	flock -e 201
	echo "**** Increment started ($$)"
	val=`cat ${tmp_count}`
	(( val++ ))
	echo ${val} > ${tmp_count}
	echo "**** Increment ended ($$)"
    ) 201> ${lockfile}
}


# See if telnet or ftp is working

# We call the program with a sequence

# command (telnet or ftp)
# wait for prompt ("telnet> "or "ftp> ")
# send open <ip address>
# wait for prompt ("login: " or "Name (<ip address>:<user>): ")
# send root
# wait for prompt ("# " or "ftp> ")
# send exit
# wait for closing message ("Connection closed by foreign host." or
#                           "221 Operation successful")

# @param[in] $1  The command to use (ftp or telnet)
# @param[in] $2  The IP address
# @param[in] $3  The connection prompt (regular expression)
# @param[in] $4  The operational prompt (regular expression)
# @param[in] $5  The closing message (regular expression)

# @return  0 on success, non-zero otherwise.
function check_remote_command {

    command=$1
    ip=$2
    conn_prompt=$3
    op_prompt=$4
    close_mess=$5

    expect -f - <<EOF > /dev/null 2>&1
    spawn "${command}"

    set timeout 30
    expect {
        "${command}> " {}
        timeout    {exit 1}
    }

    send "open ${ip}\n"

    expect {
        -re "${conn_prompt}" {}
        timeout   {exit 2}
    }

    send "root\n"

    expect {
        -re "${op_prompt}" {}
        timeout   {exit 3}
    }

    send "exit\n"

    expect {
        -re "${close_mess}" {exit 0}
        timeout {exit 4}
    }
EOF

    return $?
}


# See if telnet is working.

# We call check_remote_command with appropriate arguments

# @param[in] $1  The IP address

# @return  0 on success, non-zero otherwise.
function check_telnet {
    check_remote_command telnet $1 "^.* login: " "^.*# " \
        "Connection closed by foreign host."

    return $?
}


# See if FTP is working.

# We call check_remote_command with appropriate arguments

# @param[in] $1  The IP address

# @return  0 on success, non-zero otherwise.

function check_ftp {
    check_remote_command ftp $1 "Name \\\($1:.*\\\): " "ftp> " \
        "221 Operation successful"

    return $?
}


# See if telnet and FTP are working.

# We combine calls to check_telnet and check_ftp. Note that we do check both,
# even though we really only need to check one for failure. However in the
# future the additional information may be useful.

# This seems to be quite pessimistic. So we try each test several times.

# @param[in] $1  The IP address

# @return  0 on success, non-zero (10 x telnet failure + ftp failure)
#          otherwise.
function check_telnet_ftp {
    # Try telnet 10 times to see if we can get it to work. Once working is
    # enough
    count=10
    while [ ${count} -gt 0 ]
    do
	(( count-- ))
	check_telnet $1
	res_telnet=$?
	if [ ${res_telnet} -eq 0 ]
	then
	    break
	fi
    done

    # Try FTP 10 times to see if we can get it to work. Once working is enough
    count=10
    while [ ${count} -gt 0 ]
    do
	(( count-- ))
	check_ftp $1
	res_ftp=$?

	if [ ${res_ftp} -eq 0 ]
	then
	    break
	fi
    done

    return $(( $res_telnet * 10 + $res_ftp ))
}


# Run one block of tests.

# Do not start any computation until an IP address is free.

# @param[in] $1  the test directory
# @param[in] $2  the source directory
# @param[in] $3  the name of the tool
# @param[in] $4  the base name of the test file & test file name.
# @param[in] $5  an index (increments on each retry)

# @return  0 if the IP address is still alive at the end of the test, 1 if it
#          is not.
function run_test_block {

    # Wait until an IP address is free (between 10 and 19 seconds)
    until dec_count
    do
	sleep $(( 10 + ${RANDOM} % 10 ))
    done

    test_dir=$1
    src_dir=$2
    tool=$3
    test_base=`echo $4 | sed -e 's/#.*$//'`
    test_file=`echo $4 | sed -e 's/^.*#//' -e 's/%/ /g'`
    index=$5

    echo "test_dir=${test_dir}"
    echo "src_dir=${src_dir}"
    echo "tool=${tool}"
    echo "test_base=${test_base}"
    echo "test_file=${test_file}"

    log_file="${test_base}.log"
    sum_file="${test_base}.sum"
    res_dir="${tool}/${test_base}"

    # Run the tests, with the result in a directory of its own.
    echo "Running ${test_file}"
    mkdir -p ${test_dir}
    cd ${test_dir}
    mkdir -p ${res_dir}

    runtest --target=or32-linux          \
	    --srcdir=${src_dir}          \
	    --tool=${tool}               \
            --outdir=${res_dir}          \
	    --objdir=${test_dir}/${tool} \
            "${test_file}"

    # Use sed to get rid of irritating ctrl-M characters and move the files to
    # the main results directory.
    sed -e 's/\r//g' < ${res_dir}/${tool}.log > ${tool}/${log_file}
    sed -e 's/\r//g' < ${res_dir}/${tool}.sum > ${tool}/${sum_file}
    rm -rf ${res_dir}

    # If the telnet or FTP to the IP address is dead, we assume the test
    # failed, the target machine having died (at last partially), so we return
    # failure having blown away that IP address.

    # We also need to fail if we see any TCL or expect error messages (with
    # the string "ERROR: ". These are indicative of transient failures.
    ip=`sed < ${tool}/${log_file} -n -e 's/OR32 target hostname is //p'`
    if check_telnet_ftp ${ip}
    then
	if grep "ERROR: " ${tool}/${log_file} > /dev/null 2>&1 ||
	   grep "status not a number" ${tool}/${log_file} > /dev/null 2>&1 ||
	   grep "Download.*failed." ${tool}/${log_file} > /dev/null 2>&1
	then
	    echo "Running ${test_file} on ${ip} hit TCL/expect failure"
	    mv ${tool}/${log_file} ${tool}/${log_file}-failed-$$-${index}
	    mv ${tool}/${sum_file} ${tool}/${sum_file}-failed-$$-${index}
	    res=1
	else
	    echo "Running ${test_file} on ${ip} completed successfully"
	    res=0
	fi

	# Whether we succeeded or failed, we have returned an IP address for
	# reuse, so increment the count.
	inc_count
    else
	echo "Running ${test_file} on ${ip} died with code $?"
	mv ${tool}/${log_file} ${tool}/${log_file}-failed-$$-${index}
	mv ${tool}/${sum_file} ${tool}/${sum_file}-failed-$$-${index}
	${root_dir}/get-ip.sh --delete ${ip}
	echo "**** IP ${ip} deleted"
	res=1

	# We have not returned an IP address for reuse, so do not increment
	# the count.
    fi

    cd - > /dev/null 2>&1
    return  ${res}

}


# Create a site.exp file in the test directory. This is automatically done
# with automake, but we must do it by hand. The arguments are:
#  the test directory
function create_local_config {
    test_dir=$1
    shift
    cf=${test_dir}/site.exp

    # This omits setting TEST_GCC_EXEC_PREFIX (to "/opt/or32-new/lib/gcc/")
    # and LDFLAGS (appends "
    # -L/home/jeremy/svntrunk/GNU/or32/bd-linux/gcc/../ld"). These values are
    # only set for gcc/g++ in the original automake anyway.
    echo "set rootme \"${test_dir}/..\""              >  ${cf}
    echo "set host_triplet i686-pc-linux-gnu"         >> ${cf}
    echo "set build_triplet i686-pc-linux-gnu"        >> ${cf}
    echo "set target_triplet or32-unknown-linux-gnu"  >> ${cf}
    echo "set target_alias or32-linux"                >> ${cf}
    echo "set libiconv \"\""                          >> ${cf}
    echo "set CFLAGS \"\""                            >> ${cf}
    echo "set CXXFLAGS \"\""                          >> ${cf}
    echo "set HOSTCC \"gcc\""                         >> ${cf}
    echo "set HOSTCFLAGS \"-g -O2\""                  >> ${cf}
    echo "set TESTING_IN_BUILD_TREE 1"                >> ${cf}
    echo "set HAVE_LIBSTDCXX_V3 1"                    >> ${cf}
    echo "set ENABLE_PLUGIN 1"                        >> ${cf}
    echo "set PLUGINCC \"gcc\""                       >> ${cf}
    echo "set PLUGINCFLAGS \"-g \""                   >> ${cf}
    echo "set GMPINC \"\""                            >> ${cf}
    echo "append LDFLAGS \" -L${test_dir}/../../ld\"" >> ${cf}
    echo "set tmpdir \"${test_dir}\""                 >> ${cf}
}


# Run all the tests for a tool. The arguments in order are:
#  the test directory
#  the source directory
#  the name of the tool
#  All the test base names
function run_tool_tests {
    test_dir=$1
    shift
    src_dir=$1
    shift
    tool=$1
    shift

    # If we have no IP addresses, give up here.
    if ${root_dir}/get-ip.sh > /dev/null 2>&1
    then
	true
    else
	echo "No IP addresses - exiting"
	exit 1
    fi

    # Create the local config file for DejaGnu
    create_local_config ${test_dir}

    for t in $*
    do
	# Run the test in the background. Nothing will happen until it can
	# actually get an IP address.
	(
	    index=1

	    until run_test_block ${test_dir} ${src_dir} ${tool} ${t} ${index}
	    do
	        # Give up if there are no more IP addresses.
		if ${root_dir}/get-ip.sh > /dev/null 2>&1
		then
		    continue
		else
		    break
		fi
		index=$(( ${index} + 1 ))
	    done
        ) &

	# If we have exhausted all the IP addresses, give up here.
	if ${root_dir}/get-ip.sh > /dev/null 2>&1
	then
	    true
	else
	    echo "Out of IP addresses - exiting"
	    exit 1
	fi
    done
}
    

# Initalize all the global variables.
set_globals $*
set_count ${ip_count}

# Where to find gcc/g++ tests
gcc_test_dir=${root_dir}/bd-linux/gcc/testsuite
gcc_src_dir=${root_dir}/unisrc/gcc/testsuite

# The list of GCC tests to run.  These are the tests actually used in make
# check-gcc. The test is a test base name followed by the .exp file name. Most
# of them are target specific and do nothing.
td=${gcc_src_dir}/gcc.c-torture
exec1_list=`cd ${td}; echo execute/200[0-1]*`
exec2_list=`cd ${td}; echo execute/200[2-3]* execute/20[1-9]*`
exec3_list=`cd ${td}; echo execute/200[4-9]*`
exec4_list=`cd ${td}; echo execute/[013-89][0-4]*`
exec5_list=`cd ${td}; echo execute/[013-89][5-9]*`
exec6_list=`cd ${td}; echo execute/[abc-lABC-L]*`
exec7_list=`cd ${td}; echo execute/p[^r]* execute/pr[0-3]*`
exec8_list=`cd ${td}; echo execute/pr[^0-3]*`
exec9_list=`cd ${td}; echo execute/[mnMN]*`
exec10_list=`cd ${td}; echo execute/[q-sQ-S]*`
exec11_list=`cd ${td}; echo execute/[tuw-zTUW-Z]*`
exec12_list=`cd ${td}; echo execute/[vV]*`

exec1_list=`echo ${exec1_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec2_list=`echo ${exec2_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec3_list=`echo ${exec3_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec4_list=`echo ${exec4_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec5_list=`echo ${exec5_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec6_list=`echo ${exec6_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec7_list=`echo ${exec7_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec8_list=`echo ${exec8_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec9_list=`echo ${exec9_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec10_list=`echo ${exec10_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec11_list=`echo ${exec11_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec12_list=`echo ${exec12_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`

gcc_test_list="aapcs#aapcs.exp \
               abi-avx#abi-avx.exp \
               abi-x86_64#abi-x86_64.exp \
               acker1#acker1.exp \
               alpha#alpha.exp \
               arm#arm.exp \
               arm-isr#arm-isr.exp \
               autopar#autopar.exp \
               avr#avr.exp \
               avr-torture#avr-torture.exp \
               bfin#bfin.exp \
               bprob#bprob.exp \
               builtins#builtins.exp \
               callabi#callabi.exp \
               charset#charset.exp \
               compat#compat.exp \
               compile#compile.exp \
               cpp#cpp.exp \
               cris#cris.exp \
               cris-torture#cris-torture.exp \
               debug#debug.exp \
               dectest#dectest.exp \
               dfp#dfp.exp \
               dg#dg.exp \
               dg-torture#dg-torture.exp \
               dhry#dhry.exp \
               dwarf2#dwarf2.exp \
               ea#ea.exp \
               execute-1#execute.exp=${exec1_list} \
               execute-2#execute.exp=${exec2_list} \
               execute-3#execute.exp=${exec3_list} \
               execute-4#execute.exp=${exec4_list} \
               execute-5#execute.exp=${exec5_list} \
               execute-6#execute.exp=${exec6_list} \
               execute-7#execute.exp=${exec7_list} \
               execute-8#execute.exp=${exec8_list} \
               execute-9#execute.exp=${exec9_list} \
               execute-10#execute.exp=${exec10_list} \
               execute-11#execute.exp=${exec11_list} \
               execute-12#execute.exp=${exec12_list} \
               fixed-point#fixed-point.exp \
               format#format.exp \
               frv#frv.exp \
               gcov#gcov.exp \
               gomp#gomp.exp \
               graphite#graphite.exp \
               guality#guality.exp \
               help#help.exp \
               i386-costmodel-vect#i386-costmodel-vect.exp \
               i386#i386.exp \
               i386-prefetch#i386-prefetch.exp \
               ia64#ia64.exp \
               ieee#ieee.exp \
               ipa#ipa.exp \
               linkage#linkage.exp \
               lto#lto.exp \
               m68k#m68k.exp \
               math-torture#math-torture.exp \
               matrix1#matrix1.exp \
               matrix#matrix.exp \
               mg-2#mg-2.exp \
               mg#mg.exp \
               mips16-inter#mips16-inter.exp \
               mips-abi#mips-abi.exp \
               mips#mips.exp \
               mips-nonpic#mips-nonpic.exp \
               neon#neon.exp \
               noncompile#noncompile.exp \
               options#options.exp \
               pch#pch.exp \
               plugin#plugin.exp \
               powerpc#powerpc.exp \
               ppc-costmodel-vect#ppc-costmodel-vect.exp \
               rx#rx.exp \
               s390#s390.exp \
               sh#sh.exp \
               sieve#sieve.exp \
               sort2#sort2.exp \
               sparc#sparc.exp \
               special#special.exp \
               spu-costmodel-vect#spu-costmodel-vect.exp \
               spu#spu.exp \
               stackalign#stackalign.exp \
               struct-layout-1#struct-layout-1.exp \
               struct-reorg#struct-reorg.exp \
               test-framework#test-framework.exp \
               tls#tls.exp \
               trad#trad.exp \
               tree-prof#tree-prof.exp \
               tree-ssa#tree-ssa.exp \
               unsorted#unsorted.exp \
               vect#vect.exp \
               vmx#vmx.exp \
               vxworks#vxworks.exp \
               weak#weak.exp \
               x86_64-costmodel-vect#x86_64-costmodel-vect.exp \
               xstormy16#xstormy16.exp"

# The list of G++ tests to run. These are the tests actually used in make
# check-gcc. The test is a test base name followed by the .exp file name.
gpp_test_list="bprob#bprob.exp \
               charset#charset.exp \
               compat#compat.exp \
               debug#debug.exp \
               dfp#dfp.exp \
               dg#dg.exp \
               dg-torture#dg-torture.exp \
               dwarf2#dwarf2.exp \
               ecos#ecos.exp \
               gcov#gcov.exp \
               gomp#gomp.exp \
               graphite#graphite.exp \
               lto#lto.exp \
               old-deja#old-deja.exp \
               pch#pch.exp \
               plugin#plugin.exp \
               stackalign#stackalign.exp \
               struct-layout-1#struct-layout-1.exp \
               tls#tls.exp \
               tree-prof#tree-prof.exp \
               unix#unix.exp"

# Where to find libstdc++-v3 tests
lib_test_dir=${root_dir}/bd-linux/or32-linux/libstdc++-v3/testsuite
lib_src_dir=${root_dir}/unisrc/libstdc++-v3/testsuite

# The list of libstdc++-v3 tests to run. These are the tests actually used in
# make check-libstdc++-v3. The test is a test base name followed by the .exp
# file name.
conf1_list=`cd ${lib_src_dir}; echo a*/d*/a*`
conf2_list=`cd ${lib_src_dir}; echo a*/d*/[^a]*`
conf3_list=`cd ${lib_src_dir}; echo a*/[^d]*`
conf4_list=`cd ${lib_src_dir}; echo backward/hash_map*`
conf5_list=`cd ${lib_src_dir}; echo backward/hash_set*`
conf6_list=`cd ${lib_src_dir}; echo backward/[^h]*`
conf7_list=`cd ${lib_src_dir}; echo de*`
conf8_list=`cd ${lib_src_dir}; echo e*/[a-c]*`
conf9_list=`cd ${lib_src_dir}; echo e*/[d-f]*`
conf10_list=`cd ${lib_src_dir}; echo e*/[g-i]*`
conf11_list=`cd ${lib_src_dir}; echo e*/[j-m]*`
conf12_list=`cd ${lib_src_dir}; echo e*/n*`
conf13_list=`cd ${lib_src_dir}; echo e*/pb*/e*/[a-e]*`
conf14_list=`cd ${lib_src_dir}; echo e*/pb*/e*/[f-h]*`
conf15_list=`cd ${lib_src_dir}; echo e*/pb*/e*/[p-s]*`
conf16_list=`cd ${lib_src_dir}; echo e*/pb*/e*/[^a-s]*`
conf17_list=`cd ${lib_src_dir}; echo e*/pb*/[^e]*/[a-h]*`
conf18_list=`cd ${lib_src_dir}; echo e*/pb*/[^e]*/[i-l]*`
conf19_list=`cd ${lib_src_dir}; echo e*/pb*/[^e]*/[m-p]*`
conf20_list=`cd ${lib_src_dir}; echo e*/pb*/[^e]*/tree*`
conf21_list=`cd ${lib_src_dir}; echo e*/pb*/[^e]*/trie*`
conf22_list=`cd ${lib_src_dir}; echo e*/po*`
conf23_list=`cd ${lib_src_dir}; echo e*/pr*`
conf24_list=`cd ${lib_src_dir}; echo e*/[rs]*`
conf25_list=`cd ${lib_src_dir}; echo e*/[^a-s]*`
conf26_list=`cd ${lib_src_dir}; echo 17*/*`
conf27_list=`cd ${lib_src_dir}; echo 18*/*`
conf28_list=`cd ${lib_src_dir}; echo 19*/*`
conf29_list=`cd ${lib_src_dir}; echo [3-9]*/*`
conf30_list=`cd ${lib_src_dir}; echo 20_*/[a-d]*`
conf31_list=`cd ${lib_src_dir}; echo 20_*/[e-h]*`
conf32_list=`cd ${lib_src_dir}; echo 20_*/[i]*`
conf33_list=`cd ${lib_src_dir}; echo 20_*/[^a-i]*`
conf34_list=`cd ${lib_src_dir}; echo 21_*/b*/[a-c]*`
conf35_list=`cd ${lib_src_dir}; echo 21_*/b*/[d-i]*`
conf36_list=`cd ${lib_src_dir}; echo 21_*/b*/n*`
conf37_list=`cd ${lib_src_dir}; echo 21_*/b*/[o-p]*`
conf38_list=`cd ${lib_src_dir}; echo 21_*/b*/r*`
conf39_list=`cd ${lib_src_dir}; echo 21_*/b*/[^a-r]*`
conf40_list=`cd ${lib_src_dir}; echo 21_*/[^b]*`
conf41_list=`cd ${lib_src_dir}; echo 22_*/[a-c]*`
conf42_list=`cd ${lib_src_dir}; echo 22_*/[d-l]*`
conf43_list=`cd ${lib_src_dir}; echo 22_*/m*`
conf44_list=`cd ${lib_src_dir}; echo 22_*/nump*`
conf45_list=`cd ${lib_src_dir}; echo 22_*/num[^p]*`
conf46_list=`cd ${lib_src_dir}; echo 22_*/[^a-n]*`
conf47_list=`cd ${lib_src_dir}; echo 23_*/[a-b]*`
conf48_list=`cd ${lib_src_dir}; echo 23_*/[c-f]*`
conf49_list=`cd ${lib_src_dir}; echo 23_*/[g-l]*`
conf50_list=`cd ${lib_src_dir}; echo 23_*/ma*`
conf51_list=`cd ${lib_src_dir}; echo 23_*/m[^a]*`
conf52_list=`cd ${lib_src_dir}; echo 23_*/[n-t]*`
conf53_list=`cd ${lib_src_dir}; echo 23_*/unordered_map*`
conf54_list=`cd ${lib_src_dir}; echo 23_*/unordered_multimap*`
conf55_list=`cd ${lib_src_dir}; echo 23_*/unordered_multiset*`
conf56_list=`cd ${lib_src_dir}; echo 23_*/unordered_set*`
conf57_list=`cd ${lib_src_dir}; echo 23_*/[^a-u]*`
conf58_list=`cd ${lib_src_dir}; echo 24_*/*`
conf59_list=`cd ${lib_src_dir}; echo 25_*/[a-f]*`
conf60_list=`cd ${lib_src_dir}; echo 25_*/[g-m]*`
conf61_list=`cd ${lib_src_dir}; echo 25_*/[n-p]*`
conf62_list=`cd ${lib_src_dir}; echo 25_*/r*`
conf63_list=`cd ${lib_src_dir}; echo 25_*/[^a-r]*`
conf64_list=`cd ${lib_src_dir}; echo 26_*/[a-i]*`
conf65_list=`cd ${lib_src_dir}; echo 26_*/[j-p]*`
conf66_list=`cd ${lib_src_dir}; echo 26_*/r*`
conf67_list=`cd ${lib_src_dir}; echo 26_*/[^a-r]*`
conf68_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/[a-i]*`
conf69_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/[j-r]*`
conf70_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/s[a-d]*`
conf71_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/see*`
conf72_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/se[^e]*`
conf73_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/s[f-o]*`
conf74_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/sp*`
conf75_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/s[^a-p]*`
conf76_list=`cd ${lib_src_dir}; echo 27_*/basic_filebuf/[^a-s]*`
conf77_list=`cd ${lib_src_dir}; echo 27_*/basic_f[^i]*`
conf78_list=`cd ${lib_src_dir}; echo 27_*/basic_streambuf*`
conf79_list=`cd ${lib_src_dir}; echo 27_*/basic_stringbuf/[a-i]*`
conf80_list=`cd ${lib_src_dir}; echo 27_*/basic_stringbuf/[j-r]*`
conf81_list=`cd ${lib_src_dir}; echo 27_*/basic_stringbuf/[^a-r]*`
conf82_list=`cd ${lib_src_dir}; echo 27_*/basic_stringstream*`
conf83_list=`cd ${lib_src_dir}; echo 27_*/basic_if*`
conf84_list=`cd ${lib_src_dir}; echo 27_*/basic_io*`
conf85_list=`cd ${lib_src_dir}; echo 27_*/basic_istream*/[a-e]*`
conf86_list=`cd ${lib_src_dir}; echo 27_*/basic_istream*/[f-p]*`
conf87_list=`cd ${lib_src_dir}; echo 27_*/basic_istream*/[^a-p]*`
conf88_list=`cd ${lib_src_dir}; echo 27_*/basic_istring*`
conf89_list=`cd ${lib_src_dir}; echo 27_*/basic_of*`
conf90_list=`cd ${lib_src_dir}; echo 27_*/basic_ostream/[a-h]*`
conf91_list=`cd ${lib_src_dir}; echo 27_*/basic_ostream/inserters_a*`
conf92_list=`cd ${lib_src_dir}; echo 27_*/basic_ostream/inserters_[b-c]*`
conf93_list=`cd ${lib_src_dir}; echo 27_*/basic_ostream/inserters_[^a-c]*`
conf94_list=`cd ${lib_src_dir}; echo 27_*/basic_ostream/[^a-i]*`
conf95_list=`cd ${lib_src_dir}; echo 27_*/basic_ostring*`
conf96_list=`cd ${lib_src_dir}; echo 27_*/[acd-m]*`
conf97_list=`cd ${lib_src_dir}; echo 27_*/[^a-m]*`
conf98_list=`cd ${lib_src_dir}; echo 28_*/*`
conf99_list=`cd ${lib_src_dir}; echo 29_*/*`
conf100_list=`cd ${lib_src_dir}; echo p*/*`
conf101_list=`cd ${lib_src_dir}; echo t*/[0-3]*`
conf102_list=`cd ${lib_src_dir}; echo t*/4*/[a-h]*`
conf103_list=`cd ${lib_src_dir}; echo t*/4*/[i]*`
conf104_list=`cd ${lib_src_dir}; echo t*/4*/[^a-i]*`
conf105_list=`cd ${lib_src_dir}; echo t*/5*/[a-h]*`
conf106_list=`cd ${lib_src_dir}; echo t*/5*/[i-r]*`
conf107_list=`cd ${lib_src_dir}; echo t*/5*/[^a-r]*/0*`
conf108_list=`cd ${lib_src_dir}; echo t*/5*/[^a-r]*/1*`
conf109_list=`cd ${lib_src_dir}; echo t*/5*/[^a-r]*/2*`
conf110_list=`cd ${lib_src_dir}; echo t*/5*/[^a-r]*/[^0-2]*`
conf111_list=`cd ${lib_src_dir}; echo t*/6*`
conf112_list=`cd ${lib_src_dir}; echo t*/7*`
conf113_list=`cd ${lib_src_dir}; echo t*/[^0-7]*`

conf1_list=`echo ${conf1_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf2_list=`echo ${conf2_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf3_list=`echo ${conf3_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf4_list=`echo ${conf4_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf5_list=`echo ${conf5_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf6_list=`echo ${conf6_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf7_list=`echo ${conf7_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf8_list=`echo ${conf8_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf9_list=`echo ${conf9_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf10_list=`echo ${conf10_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf11_list=`echo ${conf11_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf12_list=`echo ${conf12_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf13_list=`echo ${conf13_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf14_list=`echo ${conf14_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf15_list=`echo ${conf15_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf16_list=`echo ${conf16_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf17_list=`echo ${conf17_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf18_list=`echo ${conf18_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf19_list=`echo ${conf19_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf20_list=`echo ${conf20_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf21_list=`echo ${conf21_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf22_list=`echo ${conf22_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf23_list=`echo ${conf23_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf24_list=`echo ${conf24_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf25_list=`echo ${conf25_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf26_list=`echo ${conf26_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf27_list=`echo ${conf27_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf28_list=`echo ${conf28_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf29_list=`echo ${conf29_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf30_list=`echo ${conf30_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf31_list=`echo ${conf31_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf32_list=`echo ${conf32_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf33_list=`echo ${conf33_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf34_list=`echo ${conf34_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf35_list=`echo ${conf35_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf36_list=`echo ${conf36_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf37_list=`echo ${conf37_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf38_list=`echo ${conf38_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf39_list=`echo ${conf39_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf40_list=`echo ${conf40_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf41_list=`echo ${conf41_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf42_list=`echo ${conf42_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf43_list=`echo ${conf43_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf44_list=`echo ${conf44_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf45_list=`echo ${conf45_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf46_list=`echo ${conf46_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf47_list=`echo ${conf47_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf48_list=`echo ${conf48_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf49_list=`echo ${conf49_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf50_list=`echo ${conf50_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf51_list=`echo ${conf51_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf52_list=`echo ${conf52_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf53_list=`echo ${conf53_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf54_list=`echo ${conf54_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf55_list=`echo ${conf55_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf56_list=`echo ${conf56_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf57_list=`echo ${conf57_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf58_list=`echo ${conf58_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf59_list=`echo ${conf59_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf60_list=`echo ${conf60_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf61_list=`echo ${conf61_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf62_list=`echo ${conf62_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf63_list=`echo ${conf63_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf64_list=`echo ${conf64_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf65_list=`echo ${conf65_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf66_list=`echo ${conf66_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf67_list=`echo ${conf67_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf68_list=`echo ${conf68_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf69_list=`echo ${conf69_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf70_list=`echo ${conf70_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf71_list=`echo ${conf71_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf72_list=`echo ${conf72_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf73_list=`echo ${conf73_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf74_list=`echo ${conf74_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf75_list=`echo ${conf75_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf76_list=`echo ${conf76_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf77_list=`echo ${conf77_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf78_list=`echo ${conf78_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf79_list=`echo ${conf79_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf80_list=`echo ${conf80_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf81_list=`echo ${conf81_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf82_list=`echo ${conf82_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf83_list=`echo ${conf83_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf84_list=`echo ${conf84_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf85_list=`echo ${conf85_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf86_list=`echo ${conf86_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf87_list=`echo ${conf87_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf87_list=`echo ${conf87_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf88_list=`echo ${conf88_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf89_list=`echo ${conf89_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf90_list=`echo ${conf90_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf91_list=`echo ${conf91_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf92_list=`echo ${conf92_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf93_list=`echo ${conf93_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf94_list=`echo ${conf94_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf95_list=`echo ${conf95_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf96_list=`echo ${conf96_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf97_list=`echo ${conf97_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf98_list=`echo ${conf98_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf99_list=`echo ${conf99_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf100_list=`echo ${conf100_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf101_list=`echo ${conf101_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf102_list=`echo ${conf102_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf103_list=`echo ${conf103_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf104_list=`echo ${conf104_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf105_list=`echo ${conf105_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf106_list=`echo ${conf106_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf107_list=`echo ${conf107_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf108_list=`echo ${conf108_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf109_list=`echo ${conf109_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf110_list=`echo ${conf110_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf111_list=`echo ${conf111_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf112_list=`echo ${conf112_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
conf113_list=`echo ${conf113_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`

lib_test_list="abi#abi.exp \
               conformance-1#conformance.exp=${conf1_list} \
               conformance-2#conformance.exp=${conf2_list} \
               conformance-3#conformance.exp=${conf3_list} \
               conformance-4#conformance.exp=${conf4_list} \
               conformance-5#conformance.exp=${conf5_list} \
               conformance-6#conformance.exp=${conf6_list} \
               conformance-7#conformance.exp=${conf7_list} \
               conformance-8#conformance.exp=${conf8_list} \
               conformance-9#conformance.exp=${conf9_list} \
               conformance-10#conformance.exp=${conf10_list} \
               conformance-11#conformance.exp=${conf11_list} \
               conformance-12#conformance.exp=${conf12_list} \
               conformance-13#conformance.exp=${conf13_list} \
               conformance-14#conformance.exp=${conf14_list} \
               conformance-15#conformance.exp=${conf15_list} \
               conformance-16#conformance.exp=${conf16_list} \
               conformance-17#conformance.exp=${conf17_list} \
               conformance-18#conformance.exp=${conf18_list} \
               conformance-19#conformance.exp=${conf19_list} \
               conformance-20#conformance.exp=${conf20_list} \
               conformance-21#conformance.exp=${conf21_list} \
               conformance-22#conformance.exp=${conf22_list} \
               conformance-23#conformance.exp=${conf23_list} \
               conformance-24#conformance.exp=${conf24_list} \
               conformance-25#conformance.exp=${conf25_list} \
               conformance-26#conformance.exp=${conf26_list} \
               conformance-27#conformance.exp=${conf27_list} \
               conformance-28#conformance.exp=${conf28_list} \
               conformance-29#conformance.exp=${conf29_list} \
               conformance-30#conformance.exp=${conf30_list} \
               conformance-31#conformance.exp=${conf31_list} \
               conformance-32#conformance.exp=${conf32_list} \
               conformance-33#conformance.exp=${conf33_list} \
               conformance-34#conformance.exp=${conf34_list} \
               conformance-35#conformance.exp=${conf35_list} \
               conformance-36#conformance.exp=${conf36_list} \
               conformance-37#conformance.exp=${conf37_list} \
               conformance-38#conformance.exp=${conf38_list} \
               conformance-39#conformance.exp=${conf39_list} \
               conformance-40#conformance.exp=${conf40_list} \
               conformance-41#conformance.exp=${conf41_list} \
               conformance-42#conformance.exp=${conf42_list} \
               conformance-43#conformance.exp=${conf43_list} \
               conformance-44#conformance.exp=${conf44_list} \
               conformance-45#conformance.exp=${conf45_list} \
               conformance-46#conformance.exp=${conf46_list} \
               conformance-47#conformance.exp=${conf47_list} \
               conformance-48#conformance.exp=${conf48_list} \
               conformance-49#conformance.exp=${conf49_list} \
               conformance-50#conformance.exp=${conf50_list} \
               conformance-51#conformance.exp=${conf51_list} \
               conformance-52#conformance.exp=${conf52_list} \
               conformance-53#conformance.exp=${conf53_list} \
               conformance-54#conformance.exp=${conf54_list} \
               conformance-55#conformance.exp=${conf55_list} \
               conformance-56#conformance.exp=${conf56_list} \
               conformance-57#conformance.exp=${conf57_list} \
               conformance-58#conformance.exp=${conf58_list} \
               conformance-59#conformance.exp=${conf59_list} \
               conformance-60#conformance.exp=${conf60_list} \
               conformance-61#conformance.exp=${conf61_list} \
               conformance-62#conformance.exp=${conf62_list} \
               conformance-63#conformance.exp=${conf63_list} \
               conformance-64#conformance.exp=${conf64_list} \
               conformance-65#conformance.exp=${conf65_list} \
               conformance-66#conformance.exp=${conf66_list} \
               conformance-67#conformance.exp=${conf67_list} \
               conformance-68#conformance.exp=${conf68_list} \
               conformance-69#conformance.exp=${conf69_list} \
               conformance-70#conformance.exp=${conf70_list} \
               conformance-71#conformance.exp=${conf71_list} \
               conformance-72#conformance.exp=${conf72_list} \
               conformance-73#conformance.exp=${conf73_list} \
               conformance-74#conformance.exp=${conf74_list} \
               conformance-75#conformance.exp=${conf75_list} \
               conformance-76#conformance.exp=${conf76_list} \
               conformance-77#conformance.exp=${conf77_list} \
               conformance-78#conformance.exp=${conf78_list} \
               conformance-79#conformance.exp=${conf79_list} \
               conformance-80#conformance.exp=${conf80_list} \
               conformance-81#conformance.exp=${conf81_list} \
               conformance-82#conformance.exp=${conf82_list} \
               conformance-83#conformance.exp=${conf83_list} \
               conformance-84#conformance.exp=${conf84_list} \
               conformance-85#conformance.exp=${conf85_list} \
               conformance-86#conformance.exp=${conf86_list} \
               conformance-87#conformance.exp=${conf87_list} \
               conformance-88#conformance.exp=${conf88_list} \
               conformance-89#conformance.exp=${conf89_list} \
               conformance-90#conformance.exp=${conf90_list} \
               conformance-91#conformance.exp=${conf91_list} \
               conformance-92#conformance.exp=${conf92_list} \
               conformance-93#conformance.exp=${conf93_list} \
               conformance-94#conformance.exp=${conf94_list} \
               conformance-95#conformance.exp=${conf95_list} \
               conformance-96#conformance.exp=${conf96_list} \
               conformance-97#conformance.exp=${conf97_list} \
               conformance-98#conformance.exp=${conf98_list} \
               conformance-99#conformance.exp=${conf99_list} \
               conformance-100#conformance.exp=${conf100_list} \
               conformance-101#conformance.exp=${conf101_list} \
               conformance-102#conformance.exp=${conf102_list} \
               conformance-103#conformance.exp=${conf103_list} \
               conformance-104#conformance.exp=${conf104_list} \
               conformance-105#conformance.exp=${conf105_list} \
               conformance-106#conformance.exp=${conf106_list} \
               conformance-107#conformance.exp=${conf107_list} \
               conformance-108#conformance.exp=${conf108_list} \
               conformance-109#conformance.exp=${conf109_list} \
               conformance-110#conformance.exp=${conf110_list} \
               conformance-111#conformance.exp=${conf111_list} \
               conformance-112#conformance.exp=${conf112_list} \
               conformance-113#conformance.exp=${conf113_list}"

# Use this list of tests just to check the currently known failures and
# timeouts.
# lib_test_list="conformance-24#conformance.exp=${conf24_list} \
#                conformance-29#conformance.exp=${conf29_list} \
#                conformance-33#conformance.exp=${conf33_list} \
#                conformance-49#conformance.exp=${conf49_list} \
#                conformance-50#conformance.exp=${conf50_list} \
#                conformance-89#conformance.exp=${conf89_list} \
#                conformance-95#conformance.exp=${conf95_list} \
#                conformance-101#conformance.exp=${conf101_list}"


run_tool_tests ${gcc_test_dir} ${gcc_src_dir} "gcc" ${gcc_test_list}
run_tool_tests ${gcc_test_dir} ${gcc_src_dir} "g++" ${gpp_test_list}
run_tool_tests ${lib_test_dir} ${lib_src_dir} "libstdc++" ${lib_test_list}

# Wait for all the tests to finish. We recompute the ip_count, since if boards
# have failed, it will be lower than when we started.
while [ `get_count` -lt "${ip_count}" ]
do
    sleep 1
    ip_count=`wc -l ${ip_file} | cut -d " " -f 1`
done
