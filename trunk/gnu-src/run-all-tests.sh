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
	val=`cat ${tmp_count}`

	if [ ${val} -gt 0 ]
	then
	    (( val-- ))
	    echo ${val} > ${tmp_count}
	    return  0
	else
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
	val=`cat ${tmp_count}`
	(( val++ ))
	echo ${val} > ${tmp_count}
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

    set timeout 2
    expect {
        "${command}> " {}
        timeout    {exit 1}
    }

    send "open ${ip}\n"

    set timeout 5
    expect {
        -re "${conn_prompt}" {}
        timeout   {exit 2}
    }

    send "root\n"

    set timeout 2
    expect {
        -re "${op_prompt}" {}
        timeout   {exit 3}
    }

    send "exit\n"

    set timeout 5
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

# @param[in] $1  The IP address

# @return  0 on success, non-zero (10 x telnet failure + ftp failure)
#          otherwise.
function check_telnet_ftp {
    check_telnet $1
    res_telnet=$?
    check_ftp $1
    res_ftp=$?

    return $(( $res_telnet * 10 + $res_ftp ))
}


# Run one block of tests. The arguments in order are:

# @param[in] $1  the test directory
# @param[in] $2  the source directory
# @param[in] $3  the name of the tool
# @param[in] $4  the base name of the test file & test file name.
# @param[in] $5  an index (increments on each retry)

# @return  0 if the IP address is still alive at the end of the test, 1 if it
#          is not.
function run_test_block {
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
    # failure.
    ip=`sed < ${tool}/${log_file} -n -e 's/OR32 target hostname is //p'`
    if check_telnet_ftp ${ip}
    then
	echo "Running ${test_file} on ${ip} completed successfully"
	res=0
    else
	echo "Running ${test_file} on ${ip} died"
	mv ${tool}/${log_file} ${tool}/${log_file}-failed-$$-${index}
	mv ${tool}/${sum_file} ${tool}/${sum_file}-failed-$$-${index}
	${root_dir}/get-ip.sh --delete ${ip}
	res=1
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

    # Create the local config file for DejaGnu
    create_local_config ${test_dir}

    for t in $*
    do
        # Wait until an IP address is free
	until dec_count
	do
	    sleep 1
	done

	# Run the test in the background
	(
	    index=1
	    until run_test_block ${test_dir} ${src_dir} ${tool} ${t} ${index}
	    do
		index=$(( ${index} + 1 ))
	    done

	    inc_count
        ) &
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
exec8_list=`cd ${td}; echo execute/pr[^0-3]* execute/[mnMNq-zQ-Z]*`

exec1_list=`echo ${exec1_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec2_list=`echo ${exec2_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec3_list=`echo ${exec3_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec4_list=`echo ${exec4_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec5_list=`echo ${exec5_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec6_list=`echo ${exec6_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec7_list=`echo ${exec7_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`
exec8_list=`echo ${exec8_list} | sed -e 's/ /* /g' -e 's/$/*/' -e 's/ /%/g'`

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
conf1_list=`cd ${lib_src_dir}; echo [ab]* de* p*/*`
conf2_list=`cd ${lib_src_dir}; echo e*/[a-m]*`
conf3_list=`cd ${lib_src_dir}; echo e*/n*`
conf4_list=`cd ${lib_src_dir}; echo e*/pb*`
conf5_list=`cd ${lib_src_dir}; echo e*/po* e*/pr*`
conf6_list=`cd ${lib_src_dir}; echo e*/[^a-p]*`
conf7_list=`cd ${lib_src_dir}; echo [013-9]*/* 20_*/*`
conf8_list=`cd ${lib_src_dir}; echo 21_*/*`
conf9_list=`cd ${lib_src_dir}; echo 22_*/*`
conf10_list=`cd ${lib_src_dir}; echo 23_*/[a-m]*`
conf11_list=`cd ${lib_src_dir}; echo 23_*/[^a-m]*`
conf12_list=`cd ${lib_src_dir}; echo 24_*/* 25_*/*`
conf13_list=`cd ${lib_src_dir}; echo 26_*/*`
conf14_list=`cd ${lib_src_dir}; echo 27_*/basic_f*`
conf15_list=`cd ${lib_src_dir}; echo 27_*/basic_s*`
conf16_list=`cd ${lib_src_dir}; echo 27_*/basic_i*`
conf17_list=`cd ${lib_src_dir}; echo 27_*/basic_of*`
conf18_list=`cd ${lib_src_dir}; echo 27_*/basic_os*`
conf19_list=`cd ${lib_src_dir}; echo 27_*/[^b]* 2[8-9]_*/*`
conf20_list=`cd ${lib_src_dir}; echo t*/[0-5]*`
conf21_list=`cd ${lib_src_dir}; echo t*/[^0-5]*`

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
               conformance-21#conformance.exp=${conf21_list}"

run_tool_tests ${gcc_test_dir} ${gcc_src_dir} "gcc" ${gcc_test_list}
run_tool_tests ${gcc_test_dir} ${gcc_src_dir} "g++" ${gpp_test_list}
run_tool_tests ${lib_test_dir} ${lib_src_dir} "libstdc++" ${lib_test_list}

# Wait for all the tests to finish
while [ `get_count` != "${ip_count}" ]
do
    sleep 1
done
