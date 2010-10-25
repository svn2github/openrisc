#!/bin/sh

# Abort execution as soon as an error is encountered
# That way the script do not let the user think the process completed correctly
# and leave the opportunity to fix the problem and restart compilation where
# it stopped
set -e

# this is where this script will store downloaded files and check for already
# downloaded files
dlwhere="${CROSSBUILD_DOWNLOAD:-$PWD/crossbuild-dl}"

# This directory is used to extract all files and to build everything in. It
# must not exist before this script is invoked (as a security measure).
# This dir should be an absolute path
builddir="${CROSSBUILD_BUILD:-$PWD/crossbuild-build}"

# will append the target string to the prefix dir mentioned here
# Note that the user running this script must be able to do make install in
# this given prefix directory. Also make sure that this given root dir
# exists.
prefix="${CROSSBUILD_PREFIX:-/usr/local}"

# This script needs to use GNU Make. On Linux systems, GNU Make is invoked
# by running the "make" command, on most BSD systems, GNU Make is invoked
# by running the "gmake" command. Set the "make" variable accordingly.
if [ -f "`which gmake 2>/dev/null`" ]; then
    make="gmake"
else
    make="make"
fi

##############################################################################
# Variables:

target="or32-elf"

releasever="or32-1.0rc1"
releasever_gcc="or32-1.0rc2"

binutilsver="2.20.1"
binutils="binutils-$binutilsver.tar.bz2"
binutils_url="http://mirrors.kernel.org/sources.redhat.com/binutils/releases"
binutils_patch="binutils-$binutilsver-$releasever.patch.bz2"
binutils_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

newlibver="1.18.0"
newlib="newlib-$newlibver.tar.gz"
newlib_url="http://mirrors.kernel.org/sourceware/newlib"
newlib_patch="newlib-$newlibver-$releasever.patch.bz2"
newlib_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

gccver="4.5.1"
gcc="gcc-$gccver.tar.bz2"
gcc_url="http://mirrors.kernel.org/gnu/gcc/gcc-$gccver";
gcc_patch="gcc-$gccver-$releasever_gcc.patch.bz2"
gcc_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

gdbver="7.2"
gdb="gdb-$gdbver.tar.bz2"
gdb_url="http://mirrors.kernel.org/gnu/gdb";
gdb_patch="gdb-$gdbver-$releasever.patch.bz2"
gdb_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

# Options passed to all GNU tools during configure
or32configure="--disable-checking --enable-fast-install=N/A --disable-libssp --enable-languages=c,c++ --with-or1ksim=$prefix/or1ksim --with-newlib"

simver="0.5.0rc1"
sim="or1ksim-$simver.tar.bz2"
sim_url="ftp://ocuser:oc@opencores.org/toolchain"

linuxver="2.6.35"
linux="linux-$linuxver.tar.bz2"
linux_url="http://www.kernel.org/pub/linux/kernel/v2.6"
linux_patch="linux-$linuxver-or32.patch.bz2"
linux_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

uclibcver="0.9.31"
uclibc="uClibc-$uclibcver.tar.bz2"
uclibc_url="http://www.uclibc.org/downloads"
uclibc_patch="uClibc-$uclibcver-or32.patch.bz2"
uclibc_patch_url="ftp://ocuser:oc@opencores.org/toolchain"
uclibc_config="extra/Configs/defconfigs/or32"

# These are the tools this script requires and depends upon.
reqtools="gcc bzip2 make patch file makeinfo"

##############################################################################
# Functions:

findtool(){
  file="$1"

  IFS=":"
  for path in $PATH
  do
    # echo "Checks for $file in $path" >&2
    if test -f "$path/$file"; then
      echo "$path/$file"
      return
    fi
  done
}

getfile() {
  # $1 file
  # $2 URL

  tool=`findtool curl`
  if test -z "$tool"; then
    tool=`findtool wget`
    if test -n "$tool"; then
      # wget download
      echo "CROSSBUILD: Downloading $2/$1 using wget"
      $tool -O "$dlwhere/$1" "$2/$1"
    fi
  else
     # curl download
      echo "CROSSBUILD: Downloading $2/$1 using curl"
     $tool -Lo "$dlwhere/$1" "$2/$1"
  fi

  if [ $? -ne 0 ] ; then
      echo "CROSSBUILD: couldn't download the file!"
      echo "CROSSBUILD: check your internet connection"
      exit
  fi

  if test -z "$tool"; then 
    echo "CROSSBUILD: No downloader tool found!"
    echo "CROSSBUILD: Please install curl or wget and re-run the script"
    exit
  fi
}

extract_tool() {
    toolname="$1"
    version="$2"
    file="$3"
    
    cd $builddir

#    if [ ! -d $toolname-$version ]; then
	echo "CROSSBUILD: extracting $file"
	if [ `file $dlwhere/$file | grep -c bzip2` -ne 0 ]; then
	    tar xjf $dlwhere/$file
	fi
	if [ `file $dlwhere/$file | grep -c gzip` -ne 0 ]; then
	    tar xzf $dlwhere/$file
	fi
 #   else
#	echo "CROSSBUILD: $file already extracted"
#    fi

}

patch_tool() {
    toolname="$1"
    version="$2"
    patch="$3"
    
    # do we have a patch?
    if test -n "$patch"; then
        echo "CROSSBUILD: applying patch $patch"

        # apply the patch
        (cd $builddir/$toolname-$version && bzcat "$dlwhere/$patch" | patch -p1)

        # check if the patch applied cleanly
        if [ $? -gt 0 ]; then
            echo "CROSSBUILD: failed to apply patch $patch"
            exit
        fi
    fi

}


download_extract_patch () {
    toolname="$1"
    version="$2"
    file="$3"
    file_url="$4"
    patch="$5"
    patch_url="$6"

    if test -f "$dlwhere/$file"; then
        echo "CROSSBUILD: $file already downloaded"
    else
        getfile "$file" "$file_url"
    fi

    if test -n "$patch"; then
        if test -f "$dlwhere/$patch"; then
            echo "CROSSBUILD: $patch already downloaded"
        else
            getfile "$patch" "$patch_url"
        fi
    fi

    extract_tool "$toolname" "$version" "$file"

    patch_tool "$toolname" "$version" "$patch"

 }

build_gnu_tool() {
    toolname="$1"
    version="$2"
    file="$3"
    file_url="$4"
    patch="$5"
    patch_url="$6"
    configure_prefix="$7"
    configure_params="$8"

    download_extract_patch "$toolname" "$version" "$file" "$file_url" "$patch" \
	"$patch_url"
    
    cd $builddir
    
    echo "CROSSBUILD: mkdir build-$toolname"
    mkdir -p build-$toolname
    echo "CROSSBUILD: cd build-$toolname"
    cd build-$toolname
    echo "CROSSBUILD: $toolname/configure"
    echo "CROSSBUILD: "../$toolname-$version/configure --target=$target \
	--prefix=$configure_prefix \
	--with-pkgversion=$releasever-built-`date +%Y%m%d` \
	--with-bugurl=http://www.opencores.org/ \
	"$configure_params"
    ../$toolname-$version/configure --target=$target \
	--prefix=$configure_prefix \
	--with-pkgversion=$releasever-built-`date +%Y%m%d` \
	--with-bugurl=http://www.opencores.org/ \
	$configure_params
    echo "CROSSBUILD: $toolname/make"
    $make
    echo "CROSSBUILD: $toolname/make install"
    $make install
}

# For now, just prep the GCC path, extract newlib, link in newlib and libgloss
# paths.
prepare_gcc() {
    prepare_gccver="$1"
    file="$2"
    newlibver="$3"
    file_url="$4"
    patch="$5"
    patch_url="$6"

    download_extract_patch "newlib" "$newlibver" "$file" "$file_url" "$patch" \
	"$patch_url"

    cd $builddir

    echo "CROSSBUILD: creating GCC source directory"
    
    mkdir -p gcc-$prepare_gccver

    echo "CROSSBUILD: linking newlib and libgloss into GCC source directory"
    if [ ! -L gcc-$prepare_gccver/newlib ]; then
	ln -s $PWD/newlib-$newlibver/newlib $PWD/gcc-$prepare_gccver/newlib
    fi

    if [ ! -L gcc-$prepare_gccver/libgloss ]; then
	ln -s $PWD/newlib-$newlibver/libgloss $PWD/gcc-$prepare_gccver/libgloss
    fi
    echo "CROSSBUILD: newlib and libgloss linked into GCC source directory"
}

# Move newlib libraries and includes to own sysroot
post_gcc_install() {

    mkdir -p ${prefix}/or32-elf/newlib
    rm -rf ${prefix}/or32-elf/newlib-include
    
    if [ -d ${prefix}/or32-elf/include ]
    then
	mv ${prefix}/or32-elf/include \
	    ${prefix}/or32-elf/newlib-include
    fi
    
    if [ -d ${prefix}/or32-elf/lib ]
    then
	afiles=`ls -1 ${prefix}/or32-elf/lib | grep '\.a' | head -1`

	if [ "x$afiles" != "x" ]
	then
	    mv ${prefix}/or32-elf/lib/*.a ${prefix}/or32-elf/newlib
	fi
    fi

    if [ -f ${prefix}/or32-elf/lib/crt0.o ]
    then
	mv ${prefix}/or32-elf/lib/crt0.o ${prefix}/or32-elf/newlib
    fi
}

# Download, patch kernel, install headers to $prefix/$target
install_linux_headers() {
    file="$linux"
    file_url="$linux_url"
    patch="$linux_patch"
    patch_url="$linux_patch_url"
    toolname="linux"
    version=$linuxver

    download_extract_patch "$toolname" "$version" "$file" "$file_url" "$patch" \
	"$patch_url"

    cd $builddir/linux-$linuxver

    echo "CROSSBUILD: defconfig kernel"
    $make ARCH=or32 defconfig
    echo "CROSSBUILD: installing headers to $prefix/or32-elf"
    $make INSTALL_HDR_PATH=$prefix/$target headers_install
}

build_uclibc () {
    file="$uclibc"
    file_url="$uclibc_url"
    patch="$uclibc_patch"
    patch_url="$uclibc_patch_url"
    toolname="uClibc"
    version="$uclibcver"

    download_extract_patch "$toolname" "$version" "$file" "$file_url" "$patch" \
	"$patch_url"

    cd $builddir

    cd uClibc-$uclibcver
    # Substitute appropriate paths for kernel headers and install path
    echo "CROSSBUILD: configuring .config for install path"
    # Create a single variable with all paths expanded
    kheaders="KERNEL_HEADERS=\\\"$prefix\\/$target\\/include\\\""
    # In-place SED
    sed -i -e "s|KERNEL_HEADERS.*|$kheaders|g" $uclibc_config
    develprefix="DEVEL_PREFIX=\\\"$prefix\\/$target\\\""
    sed -i -e "s|DEVEL_PREFIX.*|$develprefix|g" $uclibc_config
    
    echo "CROSSBUILD: defconfig uClibc"
    $make ARCH=or32 defconfig
    echo "CROSSBUILD: building uClibc"
    $make
    echo "CROSSBUILD: installing uClibc"
    $make ARCH=or32 all
    $make install
 # TODO - maybe move uClibc's libc.a where GCC can find it ($prefix/$target/lib)
    echo "CROSSBUILD: uClibc build complete"

}

##############################################################################
# Code:

for t in $reqtools; do
  tool=`findtool $t`
  if test -z "$tool"; then
    echo "CROSSBUILD: \"$t\" is required for this script to work."
    echo "CROSSBUILD: Please install \"$t\" and re-run the script."
    exit
  fi
done

# Verify build directory or create it
if test -d $builddir; then
    if test ! -w $builddir; then
        echo "CROSSBUILD: ERROR: No write permissions for the build directory!"
        exit
    fi
else
    mkdir -p $builddir
fi

# Verify download directory or create it
if test -d "$dlwhere"; then
  if ! test -w "$dlwhere"; then
    echo "CROSSBUILD: $dlwhere exists, but doesn't seem to be writable for you"
    exit
  fi
else
  mkdir -p $dlwhere
  if test $? -ne 0; then
    echo "CROSSBUILD: $dlwhere is missing and we failed to create it!"
    exit
  fi
  echo "CROSSBUILD: $dlwhere has been created"
fi

# Verify installation directory
if test ! -d $prefix; then
  mkdir -p $prefix
  if test $? -ne 0; then
    echo "CROSSBUILD: $prefix is missing and we failed to create it!"
    exit
  fi
fi
if test ! -w $prefix; then
  echo "CROSSBUILD: ERROR: This script is set to install in $prefix but has no write permissions for it"
  echo "CROSSBUILD: Please fix this and re-run this script"
  exit
fi

echo "CROSSBUILD: Download dir: $dlwhere"
echo "CROSSBUILD: Build dir   : $builddir"
echo "CROSSBUILD: Install dir : $prefix"

### start the builds

build_gnu_tool "or1ksim" "$simver" "$sim" "$sim_url" "" "" "$prefix/or1ksim" "--enable-ethphy"

build_gnu_tool "binutils" "$binutilsver" "$binutils" "$binutils_url" "$binutils_patch" "$binutils_patch_url" "$prefix" "$or32configure"

PATH="$prefix/bin:${PATH}" # add binutils to $PATH for gcc build

prepare_gcc "$gccver" "$newlib" "$newlibver" "$newlib_url" "$newlib_patch" "$newlib_patch_url"

build_gnu_tool "gcc" "$gccver" "$gcc" "$gcc_url" "$gcc_patch" "$gcc_patch_url" "$prefix" "$or32configure"

post_gcc_install

build_gnu_tool "gdb" "$gdbver" "$gdb" "$gdb_url" "$gdb_patch" "$gdb_patch_url" "$prefix" "$or32configure"

install_linux_headers

build_uclibc

### builds done

#echo "CROSSBUILD: Deleting build folder"
#rm -rf $builddir

echo ""
echo "CROSSBUILD: Done!"
echo ""
echo "CROSSBUILD: Remove temporary build directory, $builddir, manually"
echo ""
echo "CROSSBUILD: Now add $prefix/bin and $prefix/or1ksim/bin to your \$PATH."
echo ""
