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

gccver="4.2.2"
gcc="gcc-core-$gccver.tar.bz2"
gcc_url="http://mirrors.kernel.org/gnu/gcc/gcc-$gccver";
gcc_patch="gcc-$gccver-or32-fp.patch.bz2"
gcc_patch_url="ftp://ocuser:oc@opencores.org/toolchain"

gdbver="6.8"
gdb="gdb-$gdbver.tar.bz2"
gdb_url="http://mirrors.kernel.org/gnu/gdb";
gdb_patch="or32-gdb-$gdbver-patch-2.5.bz2"
gdb_patch_url=$gcc_patch_url

binutilsver="2.18.50" # snapshot
binutils="binutils-$binutilsver.tar.bz2"
binutils_url="http://mirrors.kernel.org/sources.redhat.com/binutils/snapshots"
binutils_patch="binutils-$binutilsver.or32_fixed_patch-v2.2.bz2"
binutils_patch_url=$gcc_patch_url

simver="0.4.0"
sim="or1ksim-$simver.tar.bz2"
sim_url=$gcc_patch_url

# These are the tools this script requires and depends upon.
reqtools="gcc bzip2 make patch"

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

build() {
    toolname="$1"
    version="$2"
    file="$3"
    file_url="$4"
    patch="$5"
    patch_url="$6"
    configure_prefix="$7"
    configure_params="$8"
    
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

    cd $builddir

    echo "CROSSBUILD: extracting $file"
    tar xjf $dlwhere/$file

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

    echo "CROSSBUILD: mkdir build-$toolname"
    mkdir build-$toolname
    echo "CROSSBUILD: cd build-$toolname"
    cd build-$toolname
    echo "CROSSBUILD: $toolname/configure"
    echo "CROSSBUILD: "../$toolname-$version/configure --target=$target --prefix=$configure_prefix "$configure_params"
    ../$toolname-$version/configure --target=$target --prefix=$configure_prefix $configure_params
    echo "CROSSBUILD: $toolname/make"
    $make -j8
    echo "CROSSBUILD: $toolname/make install"
    $make install
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

build "binutils" "$binutilsver" "$binutils" "$binutils_url" "$binutils_patch" "$binutils_patch_url" "$prefix" "--disable-checking"

PATH="$prefix/bin:${PATH}" # add binutils to $PATH for gcc build

build "gcc" "$gccver" "$gcc" "$gcc_url" "$gcc_patch" "$gcc_patch_url" "$prefix" "--enable-languages=c --disable-libssp --with-local-prefix=$prefix/$target"

build "gdb" "$gdbver" "$gdb" "$gdb_url" "$gdb_patch" "$gdb_patch_url" "$prefix" "--disable-werror"

build "or1ksim" "$simver" "$sim" "$sim_url" "" "" "$prefix/or1ksim" "--enable-ethphy"

### builds done

echo "CROSSBUILD: Deleting build folder"
rm -rf $builddir

echo ""
echo "CROSSBUILD: Done!"
echo ""
echo "CROSSBUILD: Now add $prefix/bin and $prefix/or1ksim/bin to your \$PATH."
echo ""
