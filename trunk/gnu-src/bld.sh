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

# The current version of GDB (7.1) is incompatible with the current version of
# binutils, so is built on its own.
component_dirs='binutils-2.20.1 newlib-1.18.0 gcc-4.2.2'
gdb_dir='gdb-7.1'
unified_src=srcw
build_dir=bld-or32
gdb_build_dir=bld-gdb
install_dir=/opt/or32-elf-new
or1ksim_dir=/opt/or1ksim-new

# Sanity check!
case ${unified_src} in
    "")
	prefix='.'
	;;

    */*)
	echo "/ in <dest> not implemented"
	exit 1
	;;

    *..*)
	echo ".. in <dest> not implemented"
	exit 1
	;;
    *)
	prefix='..'
	;;
esac

# Assume we have hardware multiply and divide. OK for Or1ksim, not the default
# for the Verilog.
export CFLAGS_FOR_TARGET='-mhard-mul -mhard-div -O2 -g'

# If /proc/cpuinfo is avaliable, limit load to launch extra jobs to
# number of processors + 1. If /proc/cpuinfo is not available, we use a
# constant of 2.
make_load="-j -l `(echo processor;cat /proc/cpuinfo 2>/dev/null || \
                   echo processor) | grep -c processor`"

# Parse options

# --force:        Ensure the unified source directory and build directory are
#                 recreated.
# --prefix:       Specify the install directory
# --scdir:        Specify the unified source directory
# --builddir:     Specify the build directory
# --gdb-builddir: Specify the build directory
# --or1ksim:      Specify the or1ksim installation directory
# --no-newlib:    Don't build newlib
# --nolink:       Don't build the unified source directory
# --noconfig:     Don't run configure
# --check:        Run DejaGnu tests
# --noinstall:    Don't run install
# --help:         List these options and exit
doforce="false";
nolink="false";
noconfig="false";
docheck="false";
noinstall="false";
newlibconfigure="--with-newlib";
newlibmake="all-target-newlib all-target-libgloss";
newlibcheck="check-target-newlib check-target-libgloss";
newlibinstall="install-target-newlib install-target-libgloss";

until
opt=$1
case ${opt}
    in
    --force)
	doforce="true";
	;;

    --prefix)
	install_dir=$2;
	shift;
	;;

    --srcdir)
	unified_src=$2;
	shift;
	;;

    --builddir)
	build_dir=$2;
	shift;
	;;

    --gdb-builddir)
	gdb_build_dir=$2;
	shift;
	;;

    --or1ksim)
	or1ksim_dir=$2;
	shift;
	;;

    --no-newlib)
	newlibconfigure=;
	newlibmake=;
	newlibcheck=;
	newlibinstall=;
	;;
	
    --nolink)
	nolink="true";
	;;

    --noconfig)
	noconfig="true";
	;;

    --check)
	docheck="true";
	;;

    --noinstall)
	noinstall="true";
	;;

    --help)
	echo "Usage: sh bld.sh [options]"
	echo ""
	echo "Options:"
	echo "    --force               Ensure the unified source directory and"
	echo "                          build directory are recreated."
	echo "    --prefix <dir>:       Specify the install directory"
	echo "    --scdir <dir:         Specify the unified source directory"
	echo "    --builddir <dir>:     Specify the build directory"
	echo "    --gdb-builddir <dir>: Specify the build directory"
	echo "    --or1ksim <dir>:      Specify the Or1ksim installation"
	echo "                          directory"
	echo "    --no-newlib           Don't build newlib"
	echo "    --nolink              Don't build the unified source"
        echo "                          directory"
	echo "    --noconfig            Don't run configure"
        echo "    --check:              Run DejaGnu tests"
	echo "    --noinstall           Don't run install"
	echo "    --help                List these options and exit"
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

# If --force was specified, delete the unified source and build directories
if [ "x$doforce" == "xtrue" ]
then
    rm -rf ${unified_src} ${build_dir} ${gdb_build_dir}
fi

# ------------------------------------------------------------------------------
# Create a unified source directory. Currently we have to leave out GDB.
if [ "x$nolink" == "xfalse" ]
then
    if [ -n "${unified_src}" ]
    then
	if [ ! -d ${unified_src} ]
	then
	    if [ -e ${unified_src} ]
	    then
		echo "${unified_src} is not a directory";
		exit 1
	    fi
	    
	    mkdir ${unified_src}
	fi
    fi
    
    cd ${unified_src}
    ignore_list=". .. CVS .svn"
    
    for srcdir in ${component_dirs}
    do
	echo "Component: $srcdir"
	case srcdir
	    in
	    /* | [A-Za-z]:[\\/]*)
		;;
	    
	    *)
		srcdir="${prefix}/${srcdir}"
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
	echo "failed to create ${unified_src}"
	exit 1
    fi

    cd ..
fi


# ------------------------------------------------------------------------------
# Configure everything
if [ "x$noconfig" == "xfalse" ]
then
    mkdir -p ${build_dir} && cd ${build_dir} && \
	  ../${unified_src}/configure --target=or32-elf \
	  --with-pkgversion="OpenRISC 32-bit toolchain (built `date +%Y%m%d`)" \
	  --with-bugurl=http://www.opencores.org/ \
	  --with-or1ksim=${or1ksim_dir} \
	  ${newlibconfigure} \
	  --enable-fast-install=N/A --disable-libssp \
	  --enable-languages=c --prefix=${install_dir}

    if [ $? != 0 ]
    then
	echo "configure failed."
	exit 1
    fi

    cd ..

    # GDB has to be separately configured at present.
    mkdir -p ${gdb_build_dir} && cd ${gdb_build_dir} && \
	  ../${gdb_dir}/configure --target=or32-elf \
	  --with-pkgversion="OpenRISC 32-bit toolchain (built `date +%Y%m%d`)" \
	  --with-bugurl=http://www.opencores.org/ \
	  --with-or1ksim=${or1ksim_dir} \
	  ${newlibconfigure} \
	  --enable-fast-install=N/A --disable-libssp \
	  --enable-languages=c --prefix=${install_dir}

    if [ $? != 0 ]
    then
	echo "GDB configure failed."
	exit 1
    fi

    cd ..

fi

# ------------------------------------------------------------------------------
# Make everything. GCC can handle parallel make, the others can't
cd ${build_dir}

make all-build all-binutils all-gas all-ld
if [ $? != 0 ]
then
    echo "make (binutils) failed."
    exit 1
fi

# Actually GCC seems to have minor problems at 4.2.2 with parallel make. We
# leave the structure here, since we'll reinstate it on a future release.

# make $make_load all-gcc
make all-gcc
if [ $? != 0 ]
then
    echo "make (GCC) failed."
    exit 1
fi

# Only make newlib if required.
if [ "x${newlibmake}" != "x" ]
then
    make ${newlibmake} all-gdb
    if [ $? != 0 ]
    then
	echo "make (Newlib) failed."
	exit 1
    fi
fi

# GDB has to be built separately at present.
cd ..
cd ${gdb_build_dir}

make all-build all-gdb
if [ $? != 0 ]
then
    echo "make (GDB) failed."
    exit 1
fi

cd ..

# ------------------------------------------------------------------------------
# Optionally check everything. We do each target in turn and don't blow out
# here if the RC is not zero. Most of the test suites fail somewhere.
if [ "x${docheck}" == "xtrue" ]
then
    export DEJAGNU=`pwd`/site.exp

    cd {build_dir}

    for tool_check in check-binutils check-gas check-ld check-gcc \
	              ${newlibcheck}
    do
	make ${tool_check}

	if [ $? != 0 ];
	then
	    echo "make ${tool_check} failed."
	fi
    done

    # GDB has to be checked separately at present.
    cd ..
    cd ${gdb_build_dir}

    make check-gdb

    if [ $? != 0 ];
    then
	echo "make check-gdb failed."
    fi

    cd ..
fi

# ------------------------------------------------------------------------------
# Install everything
if [ "x${noinstall}" == "xfalse" ]
then
    cd ${build_dir}

    make install-binutils install-gas install-ld install-gcc \
	 ${newlibinstall}

    if [ $? != 0 ];
    then
	echo "make install failed."
	exit 1
    fi

    # GDB has to be installed separately at present.
    cd ..
    cd ${gdb_build_dir}

    make install-gdb

    if [ $? != 0 ];
    then
	echo "make install (GDB) failed."
	exit 1
    fi

    cd ..
fi

# ------------------------------------------------------------------------------
# If we have built newlib, move it. This means the target specific include
# directory and the crt0.o and libraries from the target specific lib
# directory.
if [ "x${newlibconfigure}" != "x" ]
then
    mkdir -p ${install_dir}/or32-elf/newlib
    rm -rf ${install_dir}/or32-elf/newlib-include
    mv ${install_dir}/or32-elf/include ${install_dir}/or32-elf/newlib-include
    mv ${install_dir}/or32-elf/lib/*.a ${install_dir}/or32-elf/newlib
    mv ${install_dir}/or32-elf/lib/crt0.o ${install_dir}/or32-elf/newlib
fi

# uClibc could be safely built and installed now
