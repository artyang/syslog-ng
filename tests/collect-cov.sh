#!/bin/sh
#############################################################################
# Copyright (c) 2009-2016 Balabit
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As an additional exemption you are allowed to compile & link against the
# OpenSSL libraries as published by the OpenSSL project. See the file
# COPYING for details.
#
#############################################################################

cd ../

GCDATA="`find . -name *.gcda 2>/dev/null`"
if [ -z "$GCDATA" ]; then
    echo "syslog-ng was not compiled with coverage information, unable to collect them"
    exit 0
fi
for i in $GCDATA; do
    cfile=`echo $i | sed -e 's,\.gcda$,.c,'`;
    dirname=`dirname $cfile`
    cfile=`basename $cfile`
    (cd $dirname; gcov $cfile  | grep ^Lines | tail -1 | sed -e "s,^Lines executed:,$cfile,g" -e 's/ of /,/g' -e 's/%//g';)
done | awk -F , '
BEGIN {  };
{
  tested_lines=$3*$2 / 100;
  sum_tested_lines=sum_tested_lines + tested_lines;
  sum_lines=sum_lines+$3; print $1, $2 "%", $3, $3-tested_lines;
};
END {
  print "Total coverage:", sum_tested_lines/sum_lines*100 "%", sum_lines, sum_tested_lines;
};'
