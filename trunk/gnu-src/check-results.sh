#!/bin/bash

# Copyright (C) 2010 Embecosm Limited

# Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

# This file is a script to count the results from a set of test directories.

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

tmpf=/tmp/check-results-$$

# Get the individual results if we have any. Note that we don't check for the
# strings at start of line, since they may have FTP prompts showing.
echo "                           PASS  FAIL XPASS XFAIL UNRES UNSUP UNTES TOTAL"

if ls $* > /dev/null 2>&1
then
    for logfile in $*
    do
	logfile_base=`basename ${logfile}`
	tname=`echo ${logfile_base} | sed -e 's/\.log//'`
	p=`grep 'PASS:' ${logfile} | grep -v 'XPASS' | wc -l`
	f=`grep 'FAIL:' ${logfile} | grep -v 'XFAIL' | wc -l`
	xp=`grep 'XPASS:' ${logfile} | wc -l`
	xf=`grep 'XFAIL:' ${logfile} | wc -l`
	ur=`grep 'UNRESOLVED:' ${logfile} | wc -l`
	us=`grep 'UNSUPPORTED:' ${logfile} | wc -l`
	ut=`grep 'UNTESTED:' ${logfile} | wc -l`
	tot=`echo "${p} ${f} + ${xp} + ${xf} + ${ur} + ${us} + ${ut} + p" | dc`
	printf "%-25s %5d %5d %5d %5d %5d %5d %5d %5d\n" \
  	    ${tname} ${p} ${f} ${xp} ${xf} ${ur} ${us} ${ut} ${tot} | \
	    tee -a ${tmpf}
    done
fi

# Total each column, if we have any results
if ls $* > /dev/null 2>&1
then
    pt=`cut -c 27-31 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    ft=`cut -c 33-37 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    xpt=`cut -c 39-43 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    xft=`cut -c 45-49 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    urt=`cut -c 51-55 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    ust=`cut -c 57-61 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    utt=`cut -c 63-67 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
    tott=`cut -c 69-73 ${tmpf} | sed -e '2,$s/$/ +/' -e '$s/$/ p/' | dc`
else
    pt=0
    ft=0
    xpt=0
    xft=0
    urt=0
    ust=0
    utt=0
    tott=0
fi

rm -f ${tmpf}

echo "-----                     ----- ----- ----- ----- ----- ----- ----- -----"
printf "TOTAL                     %5d %5d %5d %5d %5d %5d %5d %5d\n" \
    ${pt} ${ft} ${xpt} ${xft} ${urt} ${ust} ${utt} ${tott}

