#!/bin/bash

# Copyright (C) 2011 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to stop a group of Linux instances

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

# Stop up a batch of linux sessions.

# The input for this is a string of PIDs - the output of start-linuxes.sh. We
# use the PID to kill the Linux process.

for pid in $*
do
    # Find all the processes of which this is parent and kill them
    for cpid in `ps h --ppid ${pid} | sed -e 's/ *\([[:digit:]]*\).*$/\1/'`
    do
	kill -KILL ${cpid} > /dev/null 2>&1
    done

    # Kill the main process. Using -INT should allow the session to finish
    # writing to the log.
    kill -INT ${pid} > /dev/null 2>&1
    kill -HUP ${pid} > /dev/null 2>&1

done
 
# We could add more brutal commands here to get rid of xterms and TUN/TAP
# connections. However this would just kill everything, not the named linuxes,
# potentially inconveniencing others.
# kill `ps x | grep 'xterm *-S.*/.*' | sed -e 's/\(.....\).*$/\1/'`
# fuser -k /dev/net/tun
