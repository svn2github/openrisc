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


# IP list excluding all known IPs
ip_list="192.168.0.2-39,41-59,61-79,82-127,129,130,133-255"
ip_file=`dirname ${DEJAGNU}`/ip-avail.txt
tmp_file=/tmp/find-linuxes-$$

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

# If we have a -c flag, check that the targets respond to telnet and ftp
if [ $# > 0 -a "x$1" == "x-c" ]
then
    mv ${ip_file} ${tmp_file}
    touch ${ip_file}

    len=`wc -l ${tmp_file} | cut -d " " -f 1`
    while [ ${len} -gt 0 ]
    do
	# Get each IP address in turn
	ip=`tail -${len} ${tmp_file} | head -1`
	len=$(( ${len} - 1 ))

	check_telnet_ftp ${ip}
	res=$?

	if [ ${res} -eq 0 ]
	then
	    echo ${ip} >> ${ip_file}
	else
	    echo "${ip} failed to respond to telnet/FTP: result ${res}"
	fi
    done

    rm ${tmp_file}
fi

# Print the results
cat ${ip_file}
