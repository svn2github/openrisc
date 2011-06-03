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

# IP list excluding all known IPs
ip_list="192.168.0.2-39,41-59,61-79,81-127,129,130,133-255"
ip_file=`dirname ${DEJAGNU}`/ip-avail.txt

# Keep using nmap until we get the same number of IP addresses twice running.
nmap -sP -n ${ip_list} | \
    sed -n -e 's/Nmap scan report for //p' | \
    sort > ${ip_file}
len1=`wc -l ${ip_file} | cut -d " " -f 1`
nmap -sP -n ${ip_list} | \
    sed -n -e 's/Nmap scan report for //p' | \
    sort > ${ip_file}
len2=`wc -l ${ip_file} | cut -d " " -f 1`

while [ ${len1} -ne ${len2} ]
do
    len1=${len2}
    nmap -sP -n ${ip_list} | \
	sed -n -e 's/Nmap scan report for //p' | \
	sort > ${ip_file}
    len2=`wc -l ${ip_file} | cut -d " " -f 1`
done

# Print the results
cat ${ip_file}
