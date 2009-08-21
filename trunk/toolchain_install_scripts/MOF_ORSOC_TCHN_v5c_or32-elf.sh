#!/bin/bash

# Copyright (C) 2008,2009 www.meansoffreedom.org, www.orsoc.se
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

##Date: 26/05/2009
##Title: MOF_ORSOC_TCN_v5c_or32-elf.sh
##Purpose: Complete Toolchain Builder from MOF & ORSoC.
##initial rgd, mse
##updated jb

#Changelog
# 090306 - jb
#        - added exit code checking after each make and patch
#        - changed download method to instead generate a script and run it in 
#          kermit (requiring single ftp logon per host)
# 090309 - jb
#        - changed download of GNU tools so they occurr from the GNU.org ftp 
#          (except binutils-2.18.50, it's not there)
#        - changed download of Linux kernel so it's now from ftp.kernel.org
#        - changed patch for binutils (fixes gcc-4.3.2 compilation problem 
#          caused by non-string literals in printf)
#        - changed or1ksim version from 0.3.0rc2 to 0.3.0
#        - changed Linux kernel version from 2.6.19 to 2.6.24
#        - changed Linux kernel patch version for this new kernel version
#          generated new patch for Linux kernel, based on one from MOF 
#          (www.meansoffreedom.net) which contains a more complete config file.
#          removing need for user input during "make oldconfig"
#        - changed organisation so that all the packages are not copied into
#          the build directory before they are extracted, instead it's done
#          from where they are.
#        - changed gdb-6.8 patch version due to stricter gcc-4.3.2 checking
#          return types.
#        - included better or1ksim config file in Linux sources patch, now
#          named "or1ksim_linux.cfg"
# 100309 - jb
#        - changed back to wget for file retrieval
#        - changed introduction readme
#        - changed install destination checking and setup
#        - added md5sum checking of any existing downloaded files, removing
#          need for redownloading things every time
#        - added debug build mode - run script with -d at command line
#          note: not everything in this mode has been tested, may not work
#          in certain combinations!
# 120309 - jb
#        - New OS X version - busybox doesn't work (removed from script), 
#          nor does ors1ksim (compiles but segfaults somewhere during test).
# 140309 - Added ability to change target name. Included some sed commands to 
#          change or32-uclinux to whatever $TARGET is set to in some scripts 
#          (mainly for uClibc install, the are set at configure time)
# 160309 - (v5a) Replaced busybox sources in download list (removed for OS X
#          version and forgot to replace it when running on Linux again.)
# 270309 - Changed the declaration of the check_exit_code function to be without
#          the preceeding "function" as this isn't valid in sh, and the new way
#          appears to be valid in bash.
#        - Changed the link in the error output to the new OpenCores forum.
# 210409 - Changed linux kernel and uClibc patch versions to latest
#        - Changed output to all be piped to appropriate files, makes things neater
# 260409 - Added Cygwin exports section
#        - Hopefully now runs on Cygwin with new patch for uClibc
# 300409 - New Binutils patch - fixes objdump instruction decode issue
# 110509 - (v5c) Put in check for required build tools before building
# 260509 - Changed gcc's package download to gcc-core as we only use the
#          c-language compiler and libraries right now.
# 030609 - Fixed odd problem with uClibc build on latest Cygwin where it tried
#          to run the INSTALL script file instead of the systems install binary
#          by adding a suffix to the text file.
# 290609 - Changed linux patch to 2.3 - is a patch derived from revision 42 of
#          OpenRISC repository's kernel.
# TODO: OS X build things - need an "elf.h" from some Linux machine's 
#       /usr/local/include dir and put in Mac's /usr/local/include dir - 
#       this solves issue with Linux compilation
# TODO: OS X build things - add HOST_LOADLIBES="lintl" to make line of uClibc
# TODO: OS X build things - must remove the -Werror line in $GDB_VER/gdb/Makefile, 
#       sed command: 
#       sed 's/WERROR\_CFLAGS\ \=\ \-Werror/WERROR\_CFLAGS\ \=/' $GDB_VER/gdb/Makefile
#	Can also just change the gdb-6.8/gdb/Makefile.in and remove the Werror= thing
# TODO: OS X build things - or1ksim, in or1ksim-0.3.0/peripheral/atadevice.c, it 
#       includes byteswap.h - actually it's abstract.h that defines #LE_16() bswap_16(), 
#       so in abstract.h we put in defines for bswap_16 as 
#       #define bswap_16(value) ((((value) & 0xff) << 8) | ((value) >> 8))
# TODO: OS X Build things - or1ksim strndup.c, in or1k-0.3.0/port/strndup.c strndup function, 
#       types not exist etc. Need to include sys/types.h and string.h to this file to fix 
#       a few problems.
# TODO: OS X build things - or1ksim - or1ksim-0.3.0/sim-cmd.c passes one of our functions 
#       to a readline function, rl_event_hook, but this is not defined in the OS X (BSD) 
#       readline package, so the GNU readline package must be used.
# TODO: OS X build things - or1ksim segfaults when run for some reason.

## A function we'll call throughout the setup process to check if the previous 
## command finished without error
check_exit_code()
{
    if [ $? -ne 0 ]
    then
	echo
	echo "############################ERROR#############################"
	echo
	echo "Error during toolchain installation."
	echo
	echo "Please report this to the script maintainers."
	echo
	echo "A useful report would contain information such as the tool"
	echo "being built/\"make\"d when the error occurred and the relevant"
	echo "lines of console output or log file relating to the error. It"
	echo "also helps to include information about your the host OS and"
	echo "version of GCC."
	echo "A good place to look for solutions, and report any bugs, is at"
	echo "the OpenRISC project's bug tracker on OpenCores.org:"
	echo
	echo "           http://opencores.org/openrisc,bugtracker"
	echo
	exit $?
    fi
}

REQUIRED_TOOL_LIST="
make
patch
gcc
g++
bzip2
wget
makeinfo
"
check_essential_build_tools()
{

echo "#########################Checking for required build tools######################"
echo
	for TOOL in $REQUIRED_TOOL_LIST
	do
	    echo -n "Checking for $TOOL    "
	    WHICHTOOL=`which $TOOL`
	    echo "$WHICHTOOL"
	    if [ -z $WHICHTOOL ]
	    then
		echo
		echo "\t$TOOL not found"
		echo
		echo "Please install $TOOL and re-run this script"
		echo
		exit 2
	    fi
	done

# Now check if libncurses-dev has been installed - check for the header
# Get host system's GCC prefix dir (usually /usr, but just to be sure)
# Get the verbose output of GCC, redirect STDERR to STDOUT, pipe to grep replacing spaces with newlines
# grep for the prefix dir line, cut away after the equals sign and we should have the right dir
	echo "Checking for libncurses-dev (headers)"
	HOST_GCC_PREFIX=`gcc -v 2>&1 | sed 's/\ /\n/g' | grep prefix | cut -d "=" -f 2 | head -1`
	NCURSES_HEADER_COUNT=`find $HOST_GCC_PREFIX/include -name "ncurses.h" | grep ncurses -c`
	if [ $NCURSES_HEADER_COUNT -eq 0 ]; then
	    echo
	    echo "\tlibncurses-dev not found"
	    echo ; echo "Please install libncurses-dev and re-run this script"; echo
	    exit 2
	fi
# Todo: somehow inform the user that if we're checking for makeinfo they need to install texinfo package
	echo
}

## SCRIPT VERSION ##
SCRIPT_VERSION="v5c"

## Beginning globals ##

## ORSoC FTP download settings ##
ORSOC_FTP_HOST="195.67.9.12"
ORSOC_FTP_USER="ocuser"
ORSOC_FTP_PASSWD="oc"
ORSOC_FTP_DIR="toolchain"

## GNU FTP download settings ##
GNU_FTP_HOST="ftp.gnu.org"
GNU_FTP_USER="anonymous"
#GNU_FTP_BINUTILS_DIR="gnu/binutils"
GNU_FTP_GCC_DIR="gnu/gcc/gcc-4.2.2"
GNU_FTP_GDB_DIR="gnu/gdb"

## Linux kernel download settings ##
KERNEL_FTP_HOST="ftp.kernel.org"
KERNEL_FTP_USER="anonymous"
KERNEL_FTP_PASSWD="anonymous"
KERNEL_FTP_DIR="pub/linux/kernel/v2.6"

## Local directory variables ##
START_DIR=`pwd`
DN="n"
DIR=`pwd`

## Versions of the toolchain components ##
BINUTILS_VER=binutils-2.18.50
GCC_VER_NUM=4.2.2
GCC_VER=gcc-$GCC_VER_NUM
GDB_VER=gdb-6.8
UCLIB_VER=uClibc-0.9.29
LINUX_VER=linux-2.6.24
BUSY_VER=busybox-1.7.5
SIM_VER=or1ksim-0.3.0

## Patches ##
BINUTILS_PATCH="$BINUTILS_VER.or32_fixed_patch-v2.1.bz2"
GCC_PATCH=$GCC_VER.or32patch.bz2
LINUX_PATCH="linux_2.6.24_or32_unified_v2.3.bz2"
UCLIBC_PATCH="uClibc-0.9.29-or32-patch-1.1.bz2"
GDB_PATCH="or32-gdb-6.8-patch-2.4.bz2"

## MD5sum file ##
MD5SUM_FILE=md5sums

## Configs ##
#CONFIG1="rgd_uc_29dotconfig" ## Now changed to rgd_uc_29v5dotconfig as it uses paths to the 2.6.24 kernel ##
CONFIG1="rgd_uc_29v5dotconfig"
CONFIG2="rgd_bb_1.75dotconfig"

## Ramdisk name ##
RAMDISK_FILE=initrd-fb-03.ext2.last.work.1.7.5
LINUX_RAMDISK_FILE=arch/or32/support/initrd-fb-03.ext2 ## The location and name of the ramdisk that Linux wants when it compiles

## Toolchain prefix ##
TARGET=or32-elf


## Tarballs ##
ZBALL1=$BINUTILS_VER.tar.bz2
TBALL1=$BINUTILS_VER.tar
ZBALL2=gcc-core-$GCC_VER_NUM.tar.bz2
TBALL2=$GCC_VER.tar
ZBALL3=$UCLIB_VER.tar.bz2
TBALL3=$UCLIB_VER.tar
ZBALL4=$LINUX_VER.tar.bz2
TBALL4=$LINUX_VER.tar
ZBALL5=$BUSY_VER.tar.bz2
TBALL5=$BUSY_VER.tar
ZBALL6=$SIM_VER.tar.bz2
TBALL6=$SIM_VER.tar
ZBALL7=$RAMDISK_FILE.bz2
ZBALL8=$GDB_VER.tar.bz2
TBALL8=$GDB_VER.tar

ORSOC_FTP_FILE_LIST="$ZBALL1
	$ZBALL3
	$ZBALL5
	$ZBALL6 
	$ZBALL7 
	$BINUTILS_PATCH
	$GCC_PATCH
	$LINUX_PATCH
	$UCLIBC_PATCH
	$GDB_PATCH
	$CONFIG1 
	$CONFIG2"
	
GNU_FTP_FILE_LIST="$ZBALL2
$ZBALL8"

KERNEL_FTP_FILE_LIST=$ZBALL4



## Check if we've been given the debug mode flag ##
## We won't clear away all the directories if run with the -d or -D options ##
DBG_BUILD=0
while getopts d OPT; do
    case "$OPT" in
	d)
	DBG_BUILD=1
	;;
    esac
done

##Welcome message
echo
echo "The OpenRISC toolchain install script - Provided by ORSoC & MeansOfFreedom.net"
echo "Version $SCRIPT_VERSION"
echo
echo "#####################################README#####################################"
echo 
echo "This script will install tools required for development on the OpenRISC platform"
echo "This version ($SCRIPT_VERSION) includes: "
echo "        $GCC_VER, $BINUTILS_VER, $UCLIB_VER"
echo "        $LINUX_VER, $GDB_VER, $BUSY_VER and $SIM_VER"
echo
echo "Please follow the prompts to select the install location."
echo
echo "Note: Two new directories, or32-build and or32-download, will be created in the"
echo "current directory for storing downloaded files and building the tools."
echo "Also: This script attempts to mount a ramdisk image in a local directory to copy"
echo "in the freshly built busybox. This requires root user privileges - if they are"
echo "not deteceted the existing busybox binaries will remain in the ramdisk image."
echo
echo "#####################################README#####################################"
echo

echo "Install the toolchain under current directory?"
echo "Current directory: [$DIR]"
echo "Tools will be installed in [$DIR/$TARGET]"
echo [Y/n]:
read YN

## $YN will be zero length string if user just pressed enter ##
if [ -z $YN ]
    then
    VAL="y"
else
    VAL=$(echo $YN | tr [:upper:] [:lower:])
fi

if [ $VAL = "n" ]
    then
    # Optional install path
    echo "Enter the path the toolchain will be installed in:"
    read DIR
    
    # Check if the path exists and ask the user to verify it
    until [ $DN = "y" ]
      do
      
        #Check the path exists
	if [ -d $DIR ]
	    then
	    # The path entered is OK
	    echo "Chosen directory is [$DIR]"
            echo "Tools will be installed in [$DIR/$TARGET]"
	    echo "[y/n]:"
	    read DN

	    if [ -z $DN ]
		then
		# Presume yes here
		DN="y"
	    fi
	    
	    VAL=$(echo $DN | tr [:upper:] [:lower:])
	else
	    echo
	    #echo "Path does not exist. Please create it and ensure user has correct permissions."
	    echo "Path does not exist. Create $DIR now?"
	    echo "[y/N]:"
	    read MKPATH
	    if [ -z $MKPATH ]
		then
		## $MKPATH was zero, so user probably just pressed enter without entering anything, which we'll interpret as "n" ##
		MKPATHYN="n"
	    else
		MKPATHYN=$(echo $MKPATH | tr [:upper:] [:lower:])
	    fi
	    
	    if [ $MKPATHYN = "y" ]
		then
		`mkdir $DIR`
		
		# Check we made it successfully
		if [ $? -ne 0 ]
		    then
		    DN="n"
		else
		    DN="x" # Set this so we do go back through and ask the user to verify
		fi
		
	    else
		DN="n"
	    fi
	fi
	
	if [ $DN = "n" ]
	    then
	    echo "Enter the path the toolchain should be installed in:"
	    read DIR
	fi
	
    done
    cd $DIR
#    echo "loop done"
#    echo "Working in " `pwd`
fi

## Now check that the required tools to build the toolchain are installed
check_essential_build_tools

BUILD_TOP=$START_DIR/or32-build
export BUILD_TOP

## Set donload directory to be under the current directory ##
DOWNLOAD_DIR=$START_DIR/or32-download

## Setup the target tool installation directory ##
INSTALL_DIR=$DIR

mkdir $INSTALL_DIR

####################################################################################################
## Now check which sources have already been downloaded, if any ##
DO_DOWNLOADS="y" ## Default is to NOT do downloads

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should do downloads ##
    echo "Do downloads?"
    echo "[y/N]:"
    read DO_DOWNLOADS
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $DO_DOWNLOADS ]
	then
	DO_DOWNLOADS="n"
    else
	DO_DOWNLOADS=$(echo $DO_DOWNLOADS | tr [:upper:] [:lower:])
    fi

fi

if [ $DO_DOWNLOADS = "y" ]
then
## Check if download directory exists, if not create it ##
    if [ ! -d $DOWNLOAD_DIR ]; then
	mkdir $DOWNLOAD_DIR
    fi
    
    cd $DOWNLOAD_DIR
    
## We'll need the md5sum file ##
## This is a file stored on the ORSoC FTP server, containing checksums of all the files we need ##
    rm -f $MD5SUM_FILE
    wget ftp://$ORSOC_FTP_USER:$ORSOC_FTP_PASSWD@$ORSOC_FTP_HOST/$ORSOC_FTP_DIR/$MD5SUM_FILE
## Make sure we got that OK ##
    check_exit_code
    
## We have lists of what to get from each FTP let's create a loop for each ##
    
## ORSoC FTP server downloads ##
    for CURRENT_FILE in $ORSOC_FTP_FILE_LIST
      do
  # Check if we've already got the file
      if [ -e $CURRENT_FILE ]
	  then
	  echo "File $CURRENT_FILE exists locally."
	  echo "Verifying MD5 checksum:"
      ## Verify it's right by checking the MD5 checksum ##
      ## First we get the line for $CURRENT_FILE out of the $MD5SUM_FILE, then feed it to "md5sum -c"
      ## which checks it and sets the return code, $? to 0 if OK, 1 if problem ##
	  cat $MD5SUM_FILE | grep $CURRENT_FILE | md5sum -c
	  if [ $? -ne 0 ]
	      then
	  #MD5sum of $CURRENT_FILE differs from expected value - we'll download it again
	      echo "MD5 checksum of $CURRENT_FILE differs from expected value. Re-downloading."
	      rm -f $CURRENT_FILE
	      wget ftp://$ORSOC_FTP_USER:$ORSOC_FTP_PASSWD@$ORSOC_FTP_HOST/$ORSOC_FTP_DIR/$CURRENT_FILE
	      check_exit_code
	  fi
      else
      # File doesn't exist locally - we'll download it
	  echo "Downloading $CURRENT_FILE"
	  wget ftp://$ORSOC_FTP_USER:$ORSOC_FTP_PASSWD@$ORSOC_FTP_HOST/$ORSOC_FTP_DIR/$CURRENT_FILE
	  check_exit_code
      fi
      echo
    done
    
## GNU FTP server downloads ##
    for CURRENT_FILE in $GNU_FTP_FILE_LIST
      do
  ## Check if we've already got the file ##
      if [ -e $CURRENT_FILE ]
	  then
	  echo "File $CURRENT_FILE exists locally."
	  echo "Verifying MD5 checksum:"
      ## Verify it's right by checking the MD5 checksum ##
      ## First we get the line for $CURRENT_FILE out of the $MD5SUM_FILE, then feed it to "md5sum -c"
      ## which checks it and sets the return code, $? to 0 if OK, 1 if problem ##
	  cat $MD5SUM_FILE | grep $CURRENT_FILE | md5sum -c
	  if [ $? -ne 0 ]
	      then
	  ## MD5sum of present one is incorrect - we'll download it again ##
	      echo "MD5 checksum of $CURRENT_FILE differs from expected value. Re-downloading."
	      rm -f $CURRENT_FILE
	  ## Depending on the file, we need a specific path ##
	  ## gcc's path on ftp ##
	      if [ $CURRENT_FILE = $ZBALL2 ]; then
		  GNU_FTP_DIR=$GNU_FTP_GCC_DIR
	      fi
	  ## gdb's path on ftp ##
	      if [ $CURRENT_FILE = $ZBALL8 ]; then
		  GNU_FTP_DIR=$GNU_FTP_GDB_DIR
	      fi
	      wget ftp://$GNU_FTP_USER:$GNU_FTP_USER@$GNU_FTP_HOST/$GNU_FTP_DIR/$CURRENT_FILE
	      check_exit_code
	  fi
      else
      # File doesn't exist - we'll download it
	  echo "Downloading $CURRENT_FILE"
      ## Depending on the file, we need a specific path ##
      ## gcc's path on ftp ##
	  if [ $CURRENT_FILE = $ZBALL2 ]; then
	      GNU_FTP_DIR=$GNU_FTP_GCC_DIR
	  fi
      ## gdb's path on ftp ##
	  if [ $CURRENT_FILE = $ZBALL8 ]; then
	      GNU_FTP_DIR=$GNU_FTP_GDB_DIR
	  fi
	  wget ftp://$GNU_FTP_USER:$GNU_FTP_USER@$GNU_FTP_HOST/$GNU_FTP_DIR/$CURRENT_FILE
	  check_exit_code
      fi
      echo
    done
    
## Kernel.org FTP server downloads ##
    CURRENT_FILE=$KERNEL_FTP_FILE_LIST
## Usually just a single file from the kernel.org ftp site ##
    if [ -e $CURRENT_FILE ]
    then
	echo "File $CURRENT_FILE exists locally."
	echo "Verifying MD5 checksum:"
    ## Verify it's right by checking the MD5 checksum ##
    ## First we get the line for $CURRENT_FILE out of the $MD5SUM_FILE, then feed it to "md5sum -c"
    ## which checks it and sets the return code, $? to 0 if OK, 1 if problem ##
	cat $MD5SUM_FILE | grep $CURRENT_FILE | md5sum -c
	if [ $? -ne 0 ]
	    then
	#MD5sum of present one is incorrect - we'll download it again
	    echo "MD5 checksum of $CURRENT_FILE differs from expected value. Re-downloading."
	    rm -f $CURRENT_FILE
	    wget ftp://$KERNEL_FTP_USER:$KERNEL_FTP_USER@$KERNEL_FTP_HOST/$KERNEL_FTP_DIR/$CURRENT_FILE
	    check_exit_code
	fi
    else
    ## Download it ##
	echo "Downloading $CURRENT_FILE"
	wget ftp://$KERNEL_FTP_USER:$KERNEL_FTP_PASSWD@$KERNEL_FTP_HOST/$KERNEL_FTP_DIR/$CURRENT_FILE
    check_exit_code    
    fi
    
echo
fi # if [ $DO_DOWNLOADS = "y" ]
    
####################################################################################################

## Cygwin exports ##
ON_CYGWIN=`set | grep -i mach|grep -i -c cygwin`
if [ $ON_CYGWIN -ge 1 ]
    then
    echo
    echo "Cygwin host detected"
    echo "Platform specific exports:"
    echo  "export HOST_LOADLIBES=\"-lcurses -lintl\" "
    echo
    export ON_CYGWIN=1
    export HOST_LOADLIBES="-lcurses -lintl"
fi

####################################################################################################

cd $START_DIR

## if not debug building, let's remove all the old stuff ##

if [ $DBG_BUILD -eq 0 ]
    then
## Always start with a clean build dir ##
    rm -fr $BUILD_TOP
    mkdir $BUILD_TOP
    chmod 777 $BUILD_TOP
    
    mkdir $BUILD_TOP/b-gcc
    mkdir $BUILD_TOP/b-b
fi

## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should build binutils ##
    echo "Re-build $BINUTILS_VER ?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n"
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Delete and recreate the binutils directory ##
	rm -rf $BUILD_TOP/b-b
	rm -rf $BUILD_TOP/$BINUTILS_VER
	mkdir $BUILD_TOP/b-b

    fi
fi

##########################Building Binutils#######################
if [ $BUILD_THIS = "y" ]
    then

    echo "############################## Building binutils ###############################"
    echo
    echo "Decompressing source"
    cd $BUILD_TOP
    
## Extract sources ##
    tar xjf $DOWNLOAD_DIR/$ZBALL1
    
    echo 
    echo "Patching binutils with $BINUTILS_PATCH"
    
    cd $BINUTILS_VER
    
    bzip2 -dc $DOWNLOAD_DIR/$BINUTILS_PATCH | patch -p1 > $BINUTILS_VER-patch.log 2>&1
    
## Make sure that patched ok ##
    check_exit_code
    
    cd ..
    cd $BUILD_TOP/b-b
    
    echo
    echo "Configuring $BINUTILS_VER: --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --disable-checking"
    echo "Logging output to $BINUTILS_VER-configure.log"
    $BUILD_TOP/$BINUTILS_VER/configure --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --disable-checking > $BINUTILS_VER-configure.log 2>&1
    
    echo
    echo "Making and installing $BINUTILS_VER"
    echo "Logging output to $BINUTILS_VER-make.log "
    make all install > $BINUTILS_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code

    cd $BUILD_TOP
    
fi
#######################Finish Building Binutils#####################

## Export the path to the new or32 binaries directory, used by the rest of this script ##
echo
echo "Adding $INSTALL_DIR/$TARGET/bin to the PATH variable"
export PATH=$INSTALL_DIR/$TARGET/bin:$PATH

## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should do the initial builds of GCC and Linux ##
    echo "Re-do initial $GCC_VER and $LINUX_VER builds (note if you choose to do this then it's EXTREMELY likely that you will have to do the rest of the script, particularly the re-build of gcc that occurs later)?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Delete and recreate the gcc directory ##
	rm -rf $BUILD_TOP/b-gcc
	rm -rf $BUILD_TOP/$GCC_VER
	rm -rf $BUILD_TOP/$LINUX_VER
	
	mkdir $BUILD_TOP/b-gcc	

	## Undo everything done in the next part
	rm -rf $INSTALL_DIR/$TARGET/include
	unlink $INSTALL_DIR/$TARGET/sys-include
	
    fi
fi

##########################Building GCC and Linux############################
if [ $BUILD_THIS = "y" ]
    then

    echo
    echo "####################### Building or32 gcc and Linux ############################"
    echo
    echo "Decompressing source"
    
    cd $BUILD_TOP
    
## Extract sources ##
    tar xjf $DOWNLOAD_DIR/$ZBALL2 ## gcc sources ##
    
    tar xjf $DOWNLOAD_DIR/$ZBALL4 ## Linux kernel sources ##
    
    echo
    echo "Patching $LINUX_VER with patch file $LINUX_PATCH"
    
    cd $LINUX_VER
    
## Apply patch to linux kernel sources ##
    bzip2 -dc $DOWNLOAD_DIR/$LINUX_PATCH | patch -p1 > $LINUX_VER-patch.log 2>&1
    
## Make sure that patched ok ##
    check_exit_code
## Copy our preconfigured configuration file into place ##    
    cp rgd_dot_config .config

    echo
    echo "Making Linux oldconfig"
    
    make oldconfig > $LINUX_VER-make-oldconfig.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
    
    mkdir $INSTALL_DIR/$TARGET/include
    mkdir $INSTALL_DIR/$TARGET/include/asm
    mkdir $INSTALL_DIR/$TARGET/include/linux
    
    cp -f -dR $LINUX_VER/include/linux/* $INSTALL_DIR/$TARGET/include/linux
    cp -f -dR $LINUX_VER/include/asm-or32/* $INSTALL_DIR/$TARGET/include/asm
    
    cd $INSTALL_DIR/$TARGET
    ln -s include sys-include
    cd $BUILD_TOP
    
    cd $GCC_VER
    
## Patch gcc ##
    echo
    echo "Patching $GCC_VER with $GCC_PATCH"

    bzip2 -dc $DOWNLOAD_DIR/$GCC_PATCH | patch -p1 > $GCC_VER-patch.log 2>&1
    
## Make sure that patched ok ##
    check_exit_code
    
    cd $BUILD_TOP
    cd b-gcc

    # configure GCC
    echo
    echo "Configuring $GCC_VER: --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --with-local-prefix=$INSTALL_DIR/$TARGET/$TARGET --with-gnu-as --with-gnu-ld --disable-libssp --enable-languages=c"
    echo "Logging output to $GCC_VER-configure.log"
    $BUILD_TOP/$GCC_VER/configure --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --with-local-prefix=$INSTALL_DIR/$TARGET/$TARGET --with-gnu-as --with-gnu-ld --disable-libssp --enable-languages=c > $GCC_VER-configure.log 2>&1

    echo
    echo "Making and installing $GCC_VER"
    echo "Logging output to $GCC_VER-make.log"
    make all install > $GCC_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
    
    echo
    echo "Making vmlinux image"
    echo "Logging output to vmlinux-make.log"

    cd $LINUX_VER
    make vmlinux ARCH=or32 CROSS_COMPILE=$INSTALL_DIR/$TARGET/bin/$TARGET- > vmlinux-make.log 2>&1

## Make sure that built ok ##
    check_exit_code
fi
##########################Finish Building GCC and Linux############################


## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild uClibc ##
    echo "Re-build $UCLIB_VER ?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Delete the uClibc directory ##
	rm -rf $BUILD_TOP/$UCLIB_VER	
    fi
fi

##########################Building uClibc#########################
if [ $BUILD_THIS = "y" ]
    then
    
    echo
    echo "############################## Building uClibc #################################"
    echo
    echo "Decompressing source"
    
    cd $BUILD_TOP
    tar xjf $DOWNLOAD_DIR/$ZBALL3 ## uClibc sources ##
    
    echo
    echo "Patching $UCLIB_VER with $UCLIBC_PATCH"
    
    cd $UCLIB_VER
    
## Patch uClibc sources ##
    bzip2 -dc $DOWNLOAD_DIR/$UCLIBC_PATCH  | patch -p1 > $UCLIB_VER-patch.log 2>&1
    
## Make sure that patched ok ##
    check_exit_code
    
    export CC=$TARGET-gcc
    export BUILD_TOP
    #cp $DOWNLOAD_DIR/$CONFIG1  $BUILD_TOP/$UCLIB_VER/.config
    #cp $DOWNLOAD_DIR/$CONFIG1  $BUILD_TOP/$UCLIB_VER/.config.old

    ## $TARGET name implementation into config script ##
    ## First get the second part of the target name, after the "-", the "elf" in "or32-elf", for example ##
    TARGET_SECOND_PART=`echo $TARGET | cut -f 2 -d "-"`
    ## First change any references to the uclinux target ##
    ## And delete the line saying DEVEL_PREFIX so we can put our $INSTALL_DIR path in ##
    sed "s/or32\-uclinux/\or32\-$TARGET_SECOND_PART/" $DOWNLOAD_DIR/$CONFIG1 | sed "/DEVEL\_PREFIX\=/d"  > $BUILD_TOP/$UCLIB_VER/.config  ## Output to the .config ##
    ## Now output the new DEVEL_PATH= value ##
    echo "DEVEL_PREFIX=\"$INSTALL_DIR/$TARGET\"" >> $BUILD_TOP/$UCLIB_VER/.config
    ## copy it to .config.old ##
    cp $BUILD_TOP/$UCLIB_VER/.config $BUILD_TOP/$UCLIB_VER/.config.old
    ## Do the configuration ##

## Change the cross compiler name in the Config.or32 file    
## First get the second part of the target name, after the "-" ##
    TARGET_SECOND_PART=`echo $TARGET | cut -f 2 -d "-"`
## Removes the uclinux line and changes to whatever our current target is ##
    sed "s/or32\-uclinux/\or32\-$TARGET_SECOND_PART/" $BUILD_TOP/$UCLIB_VER/extra/Configs/Config.or32  > $BUILD_TOP/$UCLIB_VER/extra/Configs/Config.or32.newtarget
    mv $BUILD_TOP/$UCLIB_VER/extra/Configs/Config.or32.newtarget $BUILD_TOP/$UCLIB_VER/extra/Configs/Config.or32    

if [ $ON_CYGWIN -ge 1 ]
    then
## If on Cygwin, fix strange error where 'install' command by the makefile ##
## results in it trying to execute the INSTALL text file in the root of the ##
## package ##
    mv INSTALL INSTALL.txt
fi

    echo
    echo "Making uClibc oldconfig"
    
    HOST_LOADLIBES="-lcurses -lintl" HOSTCC=`which gcc` make oldconfig > $UCLIB_VER-make-oldconfig.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    ln -s extra/Configs/Config.or32 Config

    echo
    echo "Making and install $UCLIB_VER"
    echo "Logging output to $UCLIB_VER-make.log"
    make all install > $UCLIB_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    unset CC
fi
##########################Finish Building uClibc#########################    
    

## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild gcc ##
    echo "Perform $GCC_VER re-build? - YES (y) if you rebuilt GCC and Linux previously, otherwise definitely NO (n)."
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Undo some things we did before after doing this compile ##
	unlink $INSTALL_DIR/$TARGET/$TARGET/sys-include
	rm -rf $INSTALL_DIR/$TARGET/$TARGET/lib/*
    fi
fi


##########################re-Building GCC########################
if [ $BUILD_THIS = "y" ]
    then

    echo
    echo "############################## Re-building or-32 gcc ###########################"
    echo
    echo "Re-building $GCC_VER"
    
    cd $BUILD_TOP
    
    cd b-gcc

    echo
    echo "Re-configuring $GCC_VER: --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --with-local-prefix=$INSTALL_DIR/$TARGET/$TARGET --with-gnu-as --with-gnu-ld --disable-libssp --enable-languages=c"
    echo "Logging output to $GCC_VER-rebuild-configure.log"
    
    $BUILD_TOP/$GCC_VER/configure --target=$TARGET --prefix=$INSTALL_DIR/$TARGET --with-local-prefix=$INSTALL_DIR/$TARGET/$TARGET --with-gnu-as --with-gnu-ld --disable-libssp --enable-languages=c > $GCC_VER-rebuild-configure.log 2>&1


    
    echo
    echo "Re-building and re-installing $GCC_VER"
    echo "Logging output to $GCC_VER-rebuild-make.log"
    make all install > $GCC_VER-rebuild-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $INSTALL_DIR/$TARGET/$TARGET
    ln -s ../include sys-include
    cd lib
    cp -f -dR ../../lib/* .
    cd $BUILD_TOP
    
fi
    
##########################finish re-Building GCC########################


## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild gdb ##
    echo "Rebuild $GDB_VER?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Delete existing gdb dir ##
	rm -rf $BUILD_TOP/$GDB_VER
    fi
fi


########################## GDB build ################
if [ $BUILD_THIS = "y" ]
    then

    echo
    echo "############################## Building GDB ####################################"
    echo
    echo "Decompressing source"
    
    
    cd $BUILD_TOP
    
    tar xjf $DOWNLOAD_DIR/$ZBALL8
    
    echo
    echo "Patching $GDB_VER with $GDB_PATCH"
    
    cd $GDB_VER
    
    bzcat -dc $DOWNLOAD_DIR/$GDB_PATCH | patch -p1 > $GDB_VER-patch.log 2>&1
    
## Make sure that patched ok ##
    check_exit_code

    echo
    echo "Configuring $GDB_VER: --target=$TARGET --prefix=$INSTALL_DIR/$TARGET"
    
    ./configure --target=$TARGET --prefix=$INSTALL_DIR/$TARGET > $GDB_VER-configure.log 2>&1

    echo
    echo "Making and installing $GDB_VER"
    
    make all install > $GDB_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
    
fi
########################## finish GDB build ################

## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild busybox ##
    echo "Rebuild $BUSY_VER?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Delete existing busybox stuff ##
	rm -rf $BUILD_TOP/$BUSY_VER
    fi
fi

##########################Building Busybox########################
if [ $BUILD_THIS = "y" ]
    then

    echo
    echo "############################## Building BusyBox ################################"
    echo
    echo "Decompressing source"

    
    cd $BUILD_TOP
    
    tar xjf $DOWNLOAD_DIR/$ZBALL5
    

    cd $BUSY_VER
    cp $DOWNLOAD_DIR/$CONFIG2  $BUILD_TOP/$BUSY_VER/.config
    cp $DOWNLOAD_DIR/$CONFIG2  $BUILD_TOP/$BUSY_VER/.config.old

    echo
    echo "Making $BUSY_VER oldconfig"
    make oldconfig > $BUSY_VER-make-oldconfig.log 2>&1

## Make Sure that built ok ##
    check_exit_code

## Put the Linux headers in BusyBox's include dir ##

    ln -s $BUILD_TOP/$LINUX_VER/include/linux $BUILD_TOP/$BUSY_VER/include/.
    ln -s $BUILD_TOP/$LINUX_VER/include/asm $BUILD_TOP/$BUSY_VER/include/.
    
    echo
    echo "Making $BUSY_VER"
    echo "Logging output to $BUSY_VER-make.log"
    make ARCH=or32 CROSS_COMPILE=$INSTALL_DIR/$TARGET/bin/$TARGET- > $BUSY_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code

    echo 
    echo "Installing $BUSY_VER"
    echo "Logging output to $BUSY_VER-install.log"    
    make install ARCH=or32 CROSS_COMPILE=$INSTALL_DIR/$TARGET/bin/$TARGET- > $BUSY_VER-install.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
    
## Copy and extract the ramdisk image file
    
    cp $DOWNLOAD_DIR/$ZBALL7 . ## Ramdisk file ##
    
    bunzip2 $RAMDISK_FILE.bz2
    
## We can only play with the ext2 image if we're not on Cygwin ##
    if [ -z $ON_CYGWIN ]
	then
	
	RT=`whoami`
	if [ $RT = "root" ];then
	    mkdir rd_mount
	    chmod 777 rd_mount
	    mount -t ext2 -o loop $RAMDISK_FILE rd_mount
	    cp -f -dR $BUILD_TOP/busy_out.1.7.5/* $BUILD_TOP/rd_mount
	    umount rd_mount
	    cp -f $RAMDISK_FILE $LINUX_VER/$LINUX_RAMDISK_FILE
	else
	    echo
	    echo "User is not root. Unable to mount Linux ramdisk."
	    echo "Note that this program built BusyBox but did not"
	    echo "install it on the ramdisk. Defaults are used."
	    cp -f $RAMDISK_FILE $LINUX_VER/$LINUX_RAMDISK_FILE
	fi
	
    fi ## if [ -z $ON_CYGWIN ]
    
    cd $BUILD_TOP
fi

########################## Finish BusyBox build ################

## default build option is yes ##
BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild linux again##
    echo "Rebuild the $LINUX_VER rebuild (only ever yes (y) if performed previous linux rebuild)?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

fi

#echo "Start Building Linux kernel?"
#read Q
##########################Final Linux kernel build################
if [ $BUILD_THIS = "y" ]
    then
    
    echo
    echo "########################### Linux image generation #############################"
    cd $BUILD_TOP
    cd $LINUX_VER
    
    echo
    echo "Making vmlinux"
    echo "Logging output to vmlinux-remake.log"
    make vmlinux ARCH=or32 CROSS_COMPILE=$INSTALL_DIR/$TARGET/bin/$TARGET- > vmlinux-remake.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
fi
##########################Finished Final Linux kernel build################

BUILD_THIS="y"

if [ $DBG_BUILD -eq 1 ]
    then
    ## Ask if we should rebuild or1ksim##
    echo "Rebuild $SIM_VER ?"
    echo "[y/N]:"
    read YN
    ## $YN will be zero length string if user just pressed enter ##
    if [ -z $YN ]
	then
	BUILD_THIS="n" # default is no
    else
	BUILD_THIS=$(echo $YN | tr [:upper:] [:lower:])
    fi

    if [ $BUILD_THIS = "y" ]
	then
	## Clean previous or1ksim directories ##
	rm -rf $BUILD_TOP/$SIM_VER
    fi
fi


##########################Simulator build#########################
if [ $BUILD_THIS = "y" ]
    then
    
    echo
    echo "############################## Building or1ksim ################################"
    echo
    echo "Decompressing source"
    
    cd $BUILD_TOP
    
    tar xjf $DOWNLOAD_DIR/$ZBALL6
    
    cd $SIM_VER
    
    echo
    echo "Configuring $SIM_VER: --target=$TARGET --prefix=$INSTALL_DIR/$TARGET"
    
    ./configure --target=$TARGET --prefix=$INSTALL_DIR/$TARGET > $SIM_VER-configure.log 2>&1
    
    echo
    echo "Making and installing $SIM_VER"
    echo "Logging output to $SIM_VER-make.log"
    make all install > $SIM_VER-make.log 2>&1
    
## Make sure that built ok ##
    check_exit_code
    
    cd $BUILD_TOP
fi
##########################Finish Simulator build#########################


##########################Code Test###############################
cd $BUILD_TOP
echo "Launch the simulator with the newly compiled Linux image and BusyBox apps?"
echo "[y/N]:"
read SI

if [ -z $SI ]
    then
    SVAL="n" ## Default is no ##
else
    SVAL=$(echo $SI | tr [:upper:] [:lower:])
fi

if [ $SVAL = "y" ];then

    ## Check for X's xterm, if we find it, and a valid $DISPLAY variable, we'll open the simulator with an xterm tty ##
    which xterm


    if [ $? -eq 0 ]
	then
	# Xterm exists, check if the $DISPLAY variable is set
	# Get the display variable
	DISPLAY=`printenv DISPLAY`
	echo "DISPLAY variable is set to $DISPLAY"
	if [ -n $DISPLAY ]
	    then
	    ## It appears display is set, let's set the or1ksim_linux.cfg file to use an xterm instead of telnet ##
	    cd $LINUX_VER
	    ## Rename the original script, adding .orig to the end ##
	    mv or1ksim_linux.cfg or1ksim_linux.cfg.orig
	    ## Now use sed to comment the line specifiying a telnet tty and uncomment the xterm line, restoring the original script ##
	    sed 's/channel\ \=\ \"tcp\:10084\"/\/\*\ channel\ \=\ \"tcp\:10084\"\ \*\//' or1ksim_linux.cfg.orig | sed 's/\/\*\ channel\ \=\ \"xterm\:\"\ \*\//\ channel\ \=\ \"xterm\:\"\ /' > or1ksim_linux.cfg
	    echo "########################## or1ksim ###############################"
	    echo
	    echo "or1ksim will open an xterm for console output as Linux is booting."
	    echo
	    echo "########################## or1ksim ###############################"
	    cd $BUILD_TOP
	fi
	
    else
	echo "########################## or1ksim ###############################"
	echo
	echo "     To connect to the simulator run: telnet 127.0.0.1 10084" 
	echo
	echo "########################## or1ksim ###############################"
	
    fi
    
    
    cd $BUILD_TOP/$LINUX_VER
    $INSTALL_DIR/$TARGET/bin/$TARGET-sim -f or1ksim_linux.cfg vmlinux
    
else
    ## User didn't run the sim, but tell them how to anyway ##
    echo "The compiled linux image is found in $BUILD_TOP/$LINUX_VER/vmlinux"
    echo "To run it in the simulator, cd to $BUILD_TOP/$LINUX_VER"
    echo "and run:"
    echo "$TARGET-sim -f or1ksim_linux.cfg vmlinux"
    echo

fi

echo
echo "OpenRISC toolchain and architectural simulator build is complete!"
echo
echo "Your tools are installed in: $INSTALL_DIR/$TARGET"
echo
echo "Please add $INSTALL_DIR/$TARGET/bin to your PATH variable"
echo
exit 0

##########################End Script##############################
