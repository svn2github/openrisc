#!/bin/bash

# Copyright (C) 2011 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to start up a group of Linux instances

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

# Start up a batch of linux sessions ready for testing.

# There are a number of ways of starting up vmlinux to force a particular IP
# address to be used, which are specified by the first argument

#   DHCP   - Linux will get an IP address via DHCP. In this case we must poll
#            the router to get the newly created IP address.

#   STATIC - We force Linux to chose a static address, by an explicit ifconfig
#            in the etc/init.d/rcS of the initramfs. We need to select an IP
#            address that is not currently being used.

# In both cases, potentially we get in a mess if someone else is starting up
# sessions at this time.


# ------------------------------------------------------------------------------
# Tell the user how to use this command

# If we are doing STATIC startup, the <count> must be followed by <count> IP
# addresses.
# ------------------------------------------------------------------------------
function bad_args {
    echo "Usage: start-linuxes.sh DHCP|STATIC <count> <dir> [ <ip> <ip> ... ]"
    echo "  <count>   - Number of Linux sessions to start up"
    echo "  <dir>     - Linux directory"
    echo "  <ip>      - IP addresses (only if STATIC)"
}


# ------------------------------------------------------------------------------
# Check the args

# Remote machine name is optional. For argument usage, see function bad_args.

# @param[in] $1      Type of processing (DHCP or STATIC)
# @pmara[in] $2      Count of Linux instances to start
# @param[in] $3      Linux directory
# @param[in] $4 ...  IP addresses if static startup
# ------------------------------------------------------------------------------
function get_args {
    # Check We have at least the first two arguments
    if [ $# -lt 3 ]
    then
	bad_args
	exit  1
    fi

    type=$1
    shift
    count=$1
    shift
    linux_dir=$1
    shift

    # Check type and get static IP addresses
    case "x${type}" in
	"xSTATIC" )
	    # Get all the IP addresses
	    for (( i=0 ; i < ${count} ; i++ ))
	    do
		if [ "x${i}" == "x" ]
		then
		    bad_args
		    exit  1
		fi

		ip_list[${i}]=$1
		shift
	    done
	    ;;

	"xDHCP" )
	    # No extra args for DHCP
	    ;;

	"x*" )
	    # Not recognized
	    bad_args
	    exit  1
	    ;;
    esac
}


# ------------------------------------------------------------------------------
# Allocate an unused TAP device

# We need /dev/tap?? that is not being used at present. We have a custom
# program to do this.

# @return  The TAP to use.
# ------------------------------------------------------------------------------
function allocate_tap {
    for (( i=0 ; ${i} < 20 ; i=${i} + 1 ))
    do
	if ./check-tap "tap${i}"
	then
	    echo "tap${i}"
	    exit
	fi
    done
}


# ------------------------------------------------------------------------------
# Start up one STATIC instance

# We need to edit the IP address into rcS, rebuild Linux and start Linux using
# the next available TAP.

# This could go wrong if someone else starts up a machine at the same time. We
# could reserve an unlikely block to avoid this, and flock this code, to avoid
# another instance causing trouble. For now we leave it unprotected.

# @param[in] $1  An index number of this instance
# @param[in] $2  The IP address to use

# @return  The Linux process ID
# ------------------------------------------------------------------------------
function start_one_static {

    ip=$2

    # A new Linux instance will require a new TAP in its config
    tap=`allocate_tap $*`

    # Edit the Or1ksim configuration file to use the allocated TAP
    sed -i -e "s/^\( *tap_dev *= *\).*$/\1\"$tap\"/" \
	${linux_dir}/arch/openrisc/or1ksim_eth.cfg

    # Edit the Linux rcS file and rebuild
    sed -i -e "s/ifconfig eth0.*up/ifconfig eth0 ${ip} up/" \
	${linux_dir}/arch/openrisc/support/initramfs/etc/init.d/rcS

    cd ${linux_dir}
    make vmlinux ARCH=openrisc CROSS_COMPILE=/opt/or32-new/bin/or32-elf- \
	> /dev/null 2>&1

    if [ $? != 0 ];
    then
	echo "start-linux.sh: 'make vmlinux' failed"
	exit 1
    fi

    cd - > /dev/null 2>&1

    # Start up Linux and allow time for the ramdisk to unpack, by waiting
    # until ping to the IP address is OK.
    or32-elf-sim -f ${linux_dir}/arch/openrisc/or1ksim_eth.cfg \
	${linux_dir}/vmlinux \
	> `hostname`-linux-console-$$-$1.log 2>&1 & linux_pid=$!

    until ping -c 1 -q ${ip} > /dev/null 2>&1
    do
	sleep 1
    done

    # make us nicer than normal, then return the new process ID
    renice +1 ${linux_pid} > /dev/null 2>&1
    echo "${linux_pid}"
}


# ------------------------------------------------------------------------------
# Start up one DHCP instance

# Start up Linux using the next available TAP.

# @param[in] $1  An index number of this instance

# @return  The Linux process ID
# ------------------------------------------------------------------------------
function start_one_dhcp {
    # A new Linux instance will require a new TAP in its config
    tap=`allocate_tap $*`

    # Edit the Or1ksim configuration file to use the allocated TAP
    sed -i -e "s/^\( *tap_dev *= *\).*$/\1\"$tap\"/" \
	${linux_dir}/arch/openrisc/or1ksim_eth.cfg

    # Start up Linux
    or32-elf-sim -f ${linux_dir}/arch/openrisc/or1ksim_eth.cfg \
	${linux_dir}/vmlinux > `hostname`-linux-output-$$-$1 2>&1 & linux_pid=$!

    # make us nicer than normal, then return the new process ID
    renice +1 ${linux_pid} > /dev/null 2>&1
    echo "${linux_pid}"

}


# ------------------------------------------------------------------------------
# Start up using DHCP on the local machine

# We wait for 30 seconds between each Linux startup, to avoid flooding the
# DHCP server.

# @return  A list of IP address of the Linux instances
# ------------------------------------------------------------------------------
function start_all_dhcp {
    echo "DHCP startup in progress..."

    for (( i=0 ; ${i} < ${count} ; i=${i} + 1 ))
    do
	one_res=`start_one_dhcp ${i}`
	res="${res} ${one_res}"
	sleep 30
    done

    echo ${res}
}


# ------------------------------------------------------------------------------
# Start up using STATIC on the local machine
# ------------------------------------------------------------------------------
function start_all_static {
    echo "STATIC startup in progress..."

    for (( i=0 ; ${i} < ${count} ; i=${i} + 1 ))
    do
	one_res=`start_one_static ${i} ${ip_list[${i}]}`
	res="${res} ${one_res}"
    done

    echo ${res}
}


# ------------------------------------------------------------------------------
# Main program
# ------------------------------------------------------------------------------

# Some useful files
tmp1=/tmp/start-linuxes-1-$$
tmp2=/tmp/start-linuxes-2-$$

# Check the arguments
get_args $*

# Start Linuxes
case "${type}" in
    "DHCP" )
	start_all_dhcp
	;;

    "STATIC" )
	start_all_static
	;;

    "*" )
	echo "Abort at line ${LINENO} in ${BASH_SOURCE[0]}"
	exit 255
	;;
esac
