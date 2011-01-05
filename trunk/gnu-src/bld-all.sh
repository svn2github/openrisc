#!/bin/bash

# Copyright (C) 2009, 2010 Embecosm Limited
# Copyright (C) 2010 ORSoC AB

# Contributor Joern Rennecke <joern.rennecke@embecosm.com>
# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>
# Contributor Julius Baxter <julius.baxter@orsoc.se>

# This file is a script to build key elements of the OpenRISC tool chain

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
# This script builds two GNU C/C++ tool chains:

# 1. With target or32-elf (fully or32-unknown-elf-newlib) for use on bare
#    metal targets

# 2. With target or32-linux (fully or32-unknown-linux-uclibc) for use with
#    OpenRISC linux.

# In both cases the binutils, gcc and C/C++ libraries are built using a
# unified source directory created by linking the top level directories from
# binutils and gcc. For the first tool chain, newlib and libgloss are also
# linked into the unified source directory.

# GDB 7.2 does not have fully compatible binutils directories, so is built
# separately.

# For the or32-linux tool chain, the system headers are needed to bootstrap
# the building of libgcc. This is achieved by a 2 stage process

# 1. Configure, build and install gcc for C only for or32-linux using the host
#    system's headers, installing into a temporary prefix.

# 2. Clean the gcc build directory and reconfigure, build and install gcc for
#    C and C++ using the headers from the temporary prefix.

# Once this is complete, the Linux headers are installed and uClibc can be
# built.

# The following arguments control how the script runs:

# --force
#     Ensure the unified source directory and build directories are
#     recreated. Only build directories of targets being built are removed.

# --languages <str>
#     Specify the languages to be built (default c,c++).

# --prefix <dir>
#     Specify the install directory (default /opt/or32-new)

# --prefix-tmp <dir>
#     Specify the temporary install directory (default /tmp/or32-tmp-${USER}).
#     Incorporating the user name avoid name clashes with other users

# --unisrc-dir <dir>
#     Specify the unified source directory (default unisrc)

# --build-dir-elf <dir>
#     Specify the build directory for the newlib (or32-elf) tool chain
#     (default bd-elf).

# --build-dir-elf-gdb <dir>
#     Specify the build directory for the newlib (or32-elf) GDB (default
#     bd-elf-gdb).

# --build-dir-linux <dir>
#     Specify the build directory for the uClibc (or32-linux) tool chain
#     (default bd-linux)

# --build-dir-linux-gdb <dir>
#     Specify the build directory for the uClibc (or32-linux) GDB (default
#     bd-linux-gdb)

# --or1ksim-dir <dir>
#     Specify the Or1ksim installation directory. Used by GDB, which links in
#     the Or1ksim simulator (default /opt/or1ksim-new)

# --binutils-dir
#     Source directory for binutils (default binutils-2.20.1)

# --gcc-dir
#     Source directory for gcc (default gcc-4.5.1')

# --newlib-dir
#     Source directory for newlib (default newlib-1.18.0)

# --uclibc-dir
#     Source directory for uClibc (default uclibc-0.9.31)

# --gdb-dir
#     Source directory for gdb (default  gdb-7.2)

# --linux-dir
#     Source directory for Linux (default linux-2.6.35)

# --no-or32-elf
#     Don't configure, build and install the newlib (or32-elf) tool chain.

# --no-or32-linux
#     Don't configure, build and install the uClibc (or32-linux) tool chain.

# --nolink
#     Don't build the unified source directory (default is to build it)

# --noconfig
#     Don't run configure. Note that this will break the two stage build of
#     gcc for Linux, so it should only be used if at least one of the uClibc
#     stages is being omitted (default is to configure)

# --nobuild
#     Don't build any tools. Useful just to reinstall stuff (default is to
#     build).

# --check
#     Run DejaGnu tests using the or32-elf tool chain (default do not run).

# --noinstall
#     Don't run install. Beware that this will omit the installation of Linux
#     headers, required for uClibc building and the installation of headers
#     from stage1 of gcc for Linux, required for stage2 of gcc for Linux. So
#     only use if at least one of the uClibc stages is being omitted (default
#     is to install).

# --no-newlib
#     Don't build newlib (default is to build newlib)

# --no-uclibc
#     Don't build uClibc (default is to build uClibc)

# --no-uclibc-stage1
#     Don't do the stage1 build of gcc for Linux (default is to build stage1).

# --no-uclibc-stage2
#     Don't do the stage2 build of gcc for Linux (default is to build stage2).

# --make-load <number>
#     Set make load for make to <number>

# --help
#     List these options and exit

# In general global variables track each of these options. In addition the
# following global variables are used:

# target - the current target (or32-elf or or32-linux).


# ------------------------------------------------------------------------------
# Set the default values of all parameters
function set_defaults {
    # Public params
    force_flag="false"
    languages="c,c++"
    prefix="/opt/or32-new"
    prefix_tmp="/tmp/or32-tmp-${USER}"
    unisrc_dir="unisrc"
    bd_elf="bd-elf"
    bd_elf_gdb="bd-elf-gdb"
    bd_linux="bd-linux"
    bd_linux_gdb="bd-linux-gdb"
    or1ksim_dir="/opt/or1ksim-new"
    binutils_dir="binutils-2.20.1"
    gcc_dir="gcc-4.5.1"
    newlib_dir="newlib-1.18.0"
    uclibc_dir="uclibc-0.9.31"
    gdb_dir="gdb-7.2"
    linux_dir="linux-2.6.36"
    or32_elf_flag="true"
    or32_linux_flag="true"
    link_flag="true"
    config_flag="true"
    build_flag="true"
    check_flag="false"
    install_flag="true"
    newlib_flag="true"
    uclibc_flag="true"
    uclibc_stage1_flag="true"
    uclibc_stage2_flag="true"
    
    # Consequential params
    newlib_config="--with-newlib"
    newlib_build="all-target-newlib all-target-libgloss"
    newlib_check="check-target-newlib check-target-libgloss"
    newlib_install="install-target-newlib install-target-libgloss"

    # Determine how many processes to use in parallel building. If
    # /proc/cpuinfo is avaliable, limit load to launch extra jobs to number of
    # processors + 1. If /proc/cpuinfo is not available, we use a constant of
    # 2.
    make_n_jobs=`(echo processor;cat /proc/cpuinfo 2>/dev/null || \
	echo processor) | grep -c processor`
    make_load="-j $make_n_jobs -l $make_n_jobs"
    unset make_n_jobs
 
}	# set_defaults ()
    

# ------------------------------------------------------------------------------
# Parse the arguments
function parse_args {
    until
    opt=$1
    case ${opt}
	in
	--force)
	    force_flag="true";
	    ;;

	--languages)
	    languages=$2;
	    shift;
	    ;;

	--prefix)
	    prefix=$2;
	    shift;
	    ;;

	--prefix-tmp)
	    prefix_tmp=$2;
	    shift;
	    ;;

	--unisrc_dir)
	    unisrc_dir=$2;
	    shift;
	    ;;
	
	--bd-elf)
	    bd_elf=$2;
	    shift;
	    ;;
	
	--bd-elf-gdb)
	    bd_elf_gdb=$2;
	    shift;
	    ;;
	
	--bd-linux)
	    bd_linux=$2;
	    shift;
	    ;;
	
	--bd-linux-gdb)
	    bd_linux_gdb=$2;
	    shift;
	    ;;

	--or1ksim-dir)
	    or1ksim_dir=$2;
	    shift;
	    ;;
	
	--binutils-dir)
	    binutils_dir=$2;
	    shift;
	    ;;
	
	--gcc-dir)
	    gcc_dir=$2;
	    shift;
	    ;;
	
	--newlib-dir)
	    newlib_dir=$2;
	    shift;
	    ;;
	
	--uclibc-dir)
	    uclibc_dir=$2;
	    shift;
	    ;;
	
	--gdb-dir)
	    gdb_dir=$2;
	    shift;
	    ;;
	
	--linux-dir)
	    linux_dir=$2;
	    shift;
	    ;;
	
	--no-or32-elf)
	    or32_elf_flag="false"
	    ;;

	--no-or32-linux)
	    or32_linux_flag="false"
	    ;;

	--nolink)
	    link_flag="false";
	    ;;
	
	--noconfig)
	    config_flag="false";
	    ;;
	
	--nobuild)
	    build_flag="false";
	    ;;
	
	--check)
	    check_flag="true";
	    ;;
	
	--noinstall)
	    install_flag="false";
	    ;;
	
	--no-newlib)
	    newlib_flag="false";

	    newlib_config="";
	    newlib_build="";
	    newlib_check="";
	    newlib_install="";
	    ;;

	--no-uclibc)
	    uclibc_flag="false";
	    ;;

	--no-uclibc-stage1)
	    uclibc_stage1_flag="false";
	    ;;

	--no-uclibc-stage2)
	    uclibc_stage2_flag="false";
	    ;;

	--make-load)
	    make_load="-j $2 -l $2";
	    shift;
	    ;;

	--help)
	    cat <<EOF;
--force
    Ensure the unified source directory and build directories are
    recreated. Only build directories of targets being built are removed.

--languages <str>
    Specify the languages to be built (default c,c++).

--prefix <dir>
    Specify the install directory (default /opt/or32-new)

--prefix-tmp <dir>
    Specify the temporary install directory (default /tmp/or32-tmp-${USER}).
    Incorporating the user name avoid name clashes with other users

--unisrc-dir <dir>
    Specify the unified source directory (default unisrc)

--build-dir-elf <dir>
    Specify the build directory for the newlib (or32-elf) tool chain
    (default bd-elf).

--build-dir-elf-gdb <dir>
    Specify the build directory for the newlib (or32-elf) GDB (default
    bd-elf-gdb).

--build-dir-linux <dir>
    Specify the build directory for the uClibc (or32-linux) tool chain
    (default bd-linux)

--build-dir-linux-gdb <dir>
    Specify the build directory for the uClibc (or32-linux) GDB (default
    bd-linux-gdb)

--or1ksim-dir <dir>
    Specify the Or1ksim installation directory. Used by GDB, which links in
    the Or1ksim simulator (default /opt/or1ksim-new)

--binutils-dir
    Source directory for binutils (default binutils-2.20.1)

--gcc-dir
    Source directory for gcc (default gcc-4.5.1')

--newlib-dir
    Source directory for newlib (default newlib-1.18.0)

--uclibc-dir
    Source directory for uClibc (default uclibc-0.9.31)

--gdb-dir
    Source directory for gdb (default  gdb-7.2)

--linux-dir
    Source directory for Linux (default linux-2.6.35)

--no-or32-elf
    Don't configure, build and install the newlib (or32-elf) tool chain.

--no-or32-linux
    Don't configure, build and install the uClibc (or32-linux) tool chain.

--nolink
    Don't build the unified source directory (default is to build it)

--noconfig
    Don't run configure. Note that this will break the two stage build of
    gcc for Linux, so it should only be used if at least one of the uClibc
    stages is being omitted (default is to configure)

--nobuild
    Don't build any tools. Useful just to reinstall stuff (default is to
    build).

--check
    Run DejaGnu tests using the or32-elf tool chain (default do not run).

--noinstall
    Don't run install. Beware that this will omit the installation of Linux
    headers, required for uClibc building and the installation of headers
    from stage1 of gcc for Linux, required for stage2 of gcc for Linux. So
    only use if at least one of the uClibc stages is being omitted (default
    is to install).

--no-newlib
    Don't build newlib (default is to build newlib)

--no-uclibc
    Don't build uClibc (default is to build uClibc)

--no-uclibc-stage1
    Don't do the stage1 build of gcc for Linux (default is to build stage1).

--no-uclibc-stage2
    Don't do the stage2 build of gcc for Linux (default is to build stage2).

--make-load <num>
    Set make load passed with -j and -l options when calling make. Default
    is set to number of processors as detected via /proc/cpuinfo

--help
    List these options and exit
EOF
	    exit 0
	    ;;

	--*)
	    echo "unrecognized option \"$1\""
	    exit 1
	    ;;
	
	*)
	    opt=""
	    ;;
    esac;
    [ -z "${opt}" ]
    do 
	shift
    done

}	# parse_args


# ------------------------------------------------------------------------------
# Sanity check argument values
function sanity_check {
    # Check unified source directory is meaningful if we are linking and that
    # we can create it.
    if [ "true" == "${link_flag}" ]
    then
	# Is it meaninful
	case ${unisrc_dir}
	    in
	    "")
		unisrc_prefix='.'
		;;

	    */*)
		echo "/ in unified source directory not implemented"
		exit 1
		;;
	
	    *..*)
		echo ".. in unified source directory not implemented"
		exit 1
		;;
	    *)
		unisrc_prefix='..'
		;;
	esac

	# Check it's a directory, and if it doesn't exist that we can create
	# it.
	if [ -n "${unisrc_dir}" ]
	then
	    if [ ! -d ${unisrc_dir} ]
	    then
		if [ -e ${unisrc_dir} ]
		then
		    echo "${unisrc_dir} is not a directory";
		    exit 1
		fi
	    
		mkdir ${unisrc_dir}
	    fi
	fi
    fi
}	# sanity_check


# ------------------------------------------------------------------------------
# Conditionally build the unified source directory. We know by now it's a
# viable and extant directory.
function link_unified {
    if [ "true" == "${link_flag}" ]
    then
	mkdir -p ${unisrc_dir}
	cd ${unisrc_dir}
	ignore_list=". .. CVS .svn"
	component_dirs="${binutils_dir} ${gcc_dir} ${newlib_dir}"
    
	for srcdir in ${component_dirs}
	do
	    echo "Component: $srcdir"
	    case srcdir
		in
		/* | [A-Za-z]:[\\/]*)
		    ;;
	    
		*)
		    srcdir="${unisrc_prefix}/${srcdir}"
		    ;;
	    esac
	
	    files=`ls -a ${srcdir}`
	
	    for f in ${files}
	    do
		found=
	    
		for i in ${ignore_list}
		do
		    if [ "$f" = "$i" ]
		    then
			found=yes
		    fi
		done
	    
		if [ -z "${found}" ]
		then
		    echo "$f		..linked"
		    ln -s ${srcdir}/$f .
		fi
	    done

	    ignore_list="${ignore_list} ${files}"
	done

	if [ $? != 0 ]
	then
	    echo "failed to create ${unisrc_dir}"
	    exit 1
	fi

	unset component_dirs
	unset ignore_list
	cd -
    fi
}	# link_unified


# ------------------------------------------------------------------------------
# Conditionally configure a GNU source directory (could be the unified
# directory, or GDB)

# @param[in] $1       "true" if we should execute this function
# @param[in] $2       The prefix to use for installation.
# @param[in] $3       The build directory to configure in.
# @param[in] $4       The source directory containing configure (relative to $1)
# @param[in] $5       The languages to configure for.
# @param[in] $6, ...  Additional configure args.
function gnu_config {
    cond=$1
    shift

    if [ "true" == ${cond} ]
    then
	this_prefix=$1
	shift
	top_builddir=$1
	shift
	top_srcdir=$1
	shift
	langs=$1
	shift

	echo "bld-all.sh: gnu_config ${top_builddir} ${top_srcdir} ${langs} $*"

	verstr="OpenRISC 32-bit toolchain for ${target} (built `date +%Y%m%d`)"

	mkdir -p ${top_builddir} &&                                   \
	    cd ${top_builddir} &&                                     \
	    ${top_srcdir}/configure --target=${target}                \
  	        --with-pkgversion="${verstr}" --disable-shared        \
	        --with-bugurl=http://www.opencores.org/               \
	        --with-or1ksim=${or1ksim_dir}                         \
	        --enable-fast-install=N/A --disable-libssp            \
	        --enable-languages=${langs} --prefix=${this_prefix} $*

	if [ $? != 0 ]
	then
	    echo "configure failed."
	    exit 1
	fi

	cd -

	unset verstr
	unset langs
	unset top_srcdir
	unset top_builddir
	unset this_prefix
    fi

    unset cond

}	# gnu_config


# ------------------------------------------------------------------------------
# Conditionally run make and check the result.

# @param[in] $1       "true" if we should execute this function
# @param[in] $2      The build directory to make in
# @param[in] $3, ... The targets to make
function gnu_make {
    cond=$1
    shift

    if [ "true" == ${cond} ]
    then
	echo "bld-all.sh: gnu_make $*"

	cd $1
	shift

	make $make_load $*

	if [ $? != 0 ]
	then
	    echo "make ($*) failed."
	    exit 1
	fi

	cd -
    fi

    unset cond

}	# gnu_make


# ------------------------------------------------------------------------------
# Conditionally configure and install the Linux headers

# @param[in] $1       The prefix to use for installation.
function install_linux_headers {
    this_prefix=$1

    cd $linux_dir

    if [ "true" == "${config_flag}" ]
    then
	echo "bld-all.sh: Configuring Linux headers"

	make ARCH=openrisc defconfig

	if [ $? != 0 ];
	then
	    echo "Linux configure failed"
	    exit 1
	fi
    fi

    # This is a bit iffy. We do rely on the headers being installed for uClibc
    # to build, so not installing the first time would be a bit dodgy.
    if [ "true" == "${install_flag}" ]
    then
	echo "bld-all.sh: Installing Linux headers"

	make INSTALL_HDR_PATH=${this_prefix}/or32-linux headers_install

	if [ $? != 0 ];
	then
	    echo "Linux header installation failed"
	    exit 1
	fi

	unset this_prefix
    fi

    cd -

}	# install_linux_headers


# ------------------------------------------------------------------------------
# Conditionally configure uClibc. Clean before configuring.

# @param[in] $1       The prefix to use for installation.
function uclibc_config {

    if [ "true" == "${config_flag}" ]
    then
	this_prefix=$1
	echo "bld-all.sh: Configuring uClibc"

	cd ${uclibc_dir}

	kheaders="KERNEL_HEADERS=\\\"${this_prefix}\\/or32-linux\\/include\\\""
	devprefix="DEVEL_PREFIX=\\\"${this_prefix}\\/or32-linux\\\""
	ccprefix="CROSS_COMPILER_PREFIX=\\\"or32-linux-\\\""

	sed -i extra/Configs/defconfigs/or32     \
	    -e "s|KERNEL_HEADERS.*|${kheaders}|g"  \
	    -e "s|DEVEL_PREFIX.*|${devprefix}|g" \
	    -e "s|CROSS_COMPILER_PREFIX.*|${ccprefix}|g"

	if [ $? != 0 ];
	then
	    echo "uClibc sed failed"
	    exit 1
	fi

	make ARCH=or32 clean

	if [ $? != 0 ];
	then
	    echo "uClibc clean failed"
	    exit 1
	fi

	make ARCH=or32 defconfig

	if [ $? != 0 ];
	then
	    echo "uClibc configure failed"
	    exit 1
	fi

	unset this_prefix
	unset ccprefix
	unset devprefix
	unset kheaders

	cd -
    fi
}	# uclibc_config


# ------------------------------------------------------------------------------
# Conditionally build and install uClibc
function uclibc_build_install {
    cd ${uclibc_dir}

    if [ "true" == "${build_flag}" ]
    then
	echo "bld-all.sh: Building uClibc"

	make ARCH=or32 all

	if [ $? != 0 ];
	then
	    echo "uClibc build failed"
	    exit 1
	fi
    fi

    if [ "true" == "${install_flag}" ]
    then
	echo "bld-all.sh: Installing uClibc"

	make ARCH=or32 install

	if [ $? != 0 ];
	then
	    echo "uClibc install failed"
	    exit 1
	fi
    fi

    cd -

}	# uclibc_build_install


# ------------------------------------------------------------------------------
# Main program
set_defaults
parse_args $*
sanity_check

# --force always blows away the link directory. It only blows away build
# directories we are actually building (see below).
if [ "true" == "${force_flag}" ]
then
    echo -n "bld-all.sh: removing ${unisrc_dir}"
    rm -rf ${unisrc_dir}
fi

link_unified

# Build the newlib (or32-elf) tool chain.
if [ "true" == "${or32_elf_flag}" ]
then
    target="or32-elf"
    echo "bld-all.sh: or32-elf toolchain"

    # --force only applies to build directories we are using!
    if [ "true" == "${force_flag}" ]
    then
	echo -n "bld-all.sh: removing ${bd_elf} ${bd_elf_gdb} "
	rm -rf ${bd_elf} ${bd_elf_gdb}
    fi

    # Configure all
    gnu_config ${config_flag} ${prefix} ${bd_elf} ../${unisrc_dir} \
	"${languages}" "${newlib_config}"
    gnu_config ${config_flag} ${prefix} ${bd_elf_gdb} ../${gdb_dir} \
	"${languages}"

    # Build all
    gnu_make ${build_flag} ${bd_elf} all-build all-binutils all-gas all-ld
    gnu_make ${build_flag} ${bd_elf} all-gcc
    gnu_make ${build_flag} ${bd_elf} all-target-libgcc all-target-libstdc++-v3 \
	${newlib_build}
    gnu_make ${build_flag} ${bd_elf_gdb} all-build all-sim all-gdb

    # Check all
    gnu_make ${check_flag} ${bd_elf} check-binutils check-gas check-ld \
	check-gcc check-target-libgcc check-target-libstdc++-v3 ${newlib_check}
    gnu_make ${check_flag} ${bd_elf_gdb} check-sim check-gdb

    # Install all
    gnu_make ${install_flag} ${bd_elf} install-binutils install-gas install-ld \
	install-gcc install-target-libgcc install-target-libstdc++-v3 \
	${newlib_install}
    gnu_make ${install_flag} ${bd_elf_gdb} install-sim install-gdb
fi

# Build the uClibc (or32-linux) tool chain
if [ "true" == "${or32_linux_flag}" ]
then
    target="or32-linux"
    echo "bld-all.sh: or32-linux toolchain"

    # --force only applies to build directories we are using!
    if [ "true" == "${force_flag}" ]
    then
	echo -n "bld-all.sh: removing ${bd_linux} ${bd_linux_gdb} "
	rm -rf ${bd_linux} ${bd_linux_gdb}
    fi

    # Stage 1 binutils/GCC build uses no headers and only C
    # language. This is just to create the libc headers, which we put in the
    # temporary prefix directory.
    if [ "true" == "${uclibc_stage1_flag}" ]
    then
	echo "bld-all.sh: uClibc GCC stage 1"

	# Make a clean temporary install directory.
	rm -rf ${prefix_tmp}
	mkdir ${prefix_tmp}

        # For header building we use just single threads, or we don't find
        # pthread.h.
	thread_config="--enable-threads=single --disable-tls"

	# To create the headers we only use C
	gnu_config ${config_flag} ${prefix_tmp} ${bd_linux} ../${unisrc_dir} \
	    "c" "--without-headers ${thread_config}"
	gnu_make ${build_flag} ${bd_linux} all-build all-binutils all-gas all-ld
	gnu_make ${build_flag} ${bd_linux} all-gcc
	gnu_make ${build_flag} ${bd_linux} all-target-libgcc
	gnu_make ${install_flag} ${bd_linux} install-binutils install-gas \
	    install-ld
	gnu_make ${install_flag} ${bd_linux} install-gcc install-target-libgcc

	# Create the headers using our temporary tool chain
	save_path="${PATH}"
	export PATH="${prefix_tmp}/bin:${PATH}"
	install_linux_headers ${prefix_tmp}
	uclibc_config ${prefix_tmp}
	uclibc_build_install
	PATH="${save_path}"
	unset save_path
    fi

    # If we are doing both stage 1 and stage 2, then we need to completely
    # clean gcc and target libraries. Just blow the entire directory away.
    if [ "truetrue" == "${uclibc_stage1_flag}${uclibc_stage2_flag}" ]
    then
	echo "bld-all.sh: uClibc GCC inter-stage cleaning"
	rm -rf ${bd_linux}/gcc ${bd_linux}/or32-linux
    fi

    # Stage 2 GCC uses the headers installed from stage 1.
    if [ "true" == "${uclibc_stage2_flag}" ]
    then
	echo "bld-all.sh: uClibc GCC stage 2"

        # uClibc now supports POSIX threads, but not TLS
	thread_config="--enable-threads=posix --disable-tls"

	gnu_config ${config_flag} ${prefix} ${bd_linux} ../${unisrc_dir} \
	    "${languages}" \
	    "--with-headers=${prefix_tmp}/or32-linux/include ${thread_config}"
	gnu_make ${build_flag} ${bd_linux} all-build all-binutils all-gas all-ld
	gnu_make ${build_flag} ${bd_linux} all-gcc
	gnu_make ${build_flag} ${bd_linux} all-target-libgcc
	gnu_make ${install_flag} ${bd_linux} install-binutils install-gas \
	    install-ld
	gnu_make ${install_flag} ${bd_linux} install-gcc install-target-libgcc

	# We need to build uClibc before building the C++ libraries, which in
	# turn needs the Linux headers
	export PATH=${prefix}/bin:${PATH}
	install_linux_headers ${prefix}
	uclibc_config ${prefix}
	uclibc_build_install

	# Finish building the C++ library
	gnu_make ${build_flag} ${bd_linux} all-target-libstdc++-v3
	gnu_make ${install_flag} ${bd_linux} install-target-libstdc++-v3
    fi

    # Configure, build and install GDB (note we need to reconfigure in case
    # only stage1 has been run previously).
    gnu_config ${config_flag} ${prefix} ${bd_linux_gdb} ../${gdb_dir} \
	"${languages}"
    gnu_make ${build_flag} ${bd_linux_gdb} all-build all-sim all-gdb
    gnu_make ${install_flag} ${bd_linux_gdb} install-sim install-gdb
fi
