#!/bin/bash

# Copyright (C) 2010 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to rebuild Linux/Busybox

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

bb_dir=busybox-1.17.3
linux_dir=linux-2.6.37

# Clean, build and install BusyBox
echo "bld-bb.sh: Rebuilding BusyBox"
cd ${bb_dir}

make clean
if [ $? != 0 ];
then
    echo "bld-bb.sh: BusyBox 'make clean' failed"
    exit 1
fi

make
if [ $? != 0 ];
then
    echo "bld-bb.sh: BusyBox 'make' failed"
    exit 1
fi

make install
if [ $? != 0 ];
then
    echo "bld-bb.sh: BusyBox 'make install' failed"
    exit 1
fi

cd -

# Configure, clean and rebuild Linux
echo "bld-bb.sh: Rebuilding Linux"
cd ${linux_dir}

make defconfig ARCH=openrisc CROSS_COMPILE=/opt/or32-new/bin/or32-elf-
if [ $? != 0 ];
then
    echo "bld-bb.sh: Linux 'make defconfig' failed"
    exit 1
fi

make clean ARCH=openrisc CROSS_COMPILE=/opt/or32-new/bin/or32-elf-
if [ $? != 0 ];
then
    echo "bld-bb.sh: Linux 'make clean' failed"
    exit 1
fi

make vmlinux ARCH=openrisc CROSS_COMPILE=/opt/or32-new/bin/or32-elf-
if [ $? != 0 ];
then
    echo "bld-bb.sh: Linux 'make vmlinux' failed"
    exit 1
fi

cd -
