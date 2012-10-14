#!/bin/bash

# Copyright (C) 2010 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to look at log file timings

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

# Look for the different GNU results in different directories. We put the
# results to a temporary file, to allow us to suck out the summary as well.

# The argument is the list of log files to process.

# Intermediate result
tmpf=/tmp/check-timings-$$

tottime=0
count=0
TIMEOUT=1200

# Option to sort
if [ $# -gt 0 -a $1 == "-s" ]
then
    shift
    dosort="true"
else
    dosort="false"
fi

# Get each result
for logfile in $*
do
    logfile_base=`basename ${logfile}`
    grosstime=`grep "completed in" ${logfile} | sed -e 's/^.*completed in //' | sed -e 's/ seconds.*$//'`
    unres=`grep -c UNRESOLVED ${logfile}`
    nettime=$(( ${grosstime} - ${TIMEOUT} * ${unres} ))
    printf "%-25s %5d %5d" ${logfile_base} ${grosstime} ${nettime} >> ${tmpf}

    if [ ${unres} -gt 0 ]
    then
	printf "  %d unresolved\n" ${unres} >> ${tmpf}
    else
	printf "\n" >> ${tmpf}
    fi

    if [ "x${nettime}" != "x" ]
    then
	tottime=$(( ${tottime} + ${nettime} ))
	count=$(( ${count} + 1 ))
    fi
done

average=$(( ${tottime} / ${count} ))

# Option to sort
if [ ${dosort} == "true" ]
then
    sort -n +1 -2 ${tmpf}
else
    cat ${tmpf}
fi

printf "%-25s %5d\n" "Total" ${tottime}
printf "%-25s %5d\n" "Average" ${average}

rm ${tmpf}