#############################################################################
# Copyright (c) 2016 Balabit
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As an additional exemption you are allowed to compile & link against the
# OpenSSL libraries as published by the OpenSSL project. See the file
# COPYING for details.
#
#############################################################################

SUBDIRS = lib libtest modules syslog-ng syslog-ng-ctl syslog-ng-query persist-tool scripts tests doc contrib scl debian tgz2build build winbuild tools

# tests from pe
SUBDIRS += pe-tests windows-binaries windows-tools

ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = VERSION \
	syslog-ng.spec syslog-ng.spec.in dist.conf dist.conf.in ChangeLog syslog-ng.pc.in balabit-vs-build makefile.msc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = syslog-ng.pc

.PHONY: ChangeLog

check_%:
	@target=$@ ; \
        if test $$target = 'check_unit' ; then \
                $(MAKE) SUBDIRS=tests/unit check ; \
        elif test $$target = 'check_functional' ; then \
                $(MAKE) SUBDIRS=pe-tests/functional check ; \
        elif test $${target:0:19} = 'check_unit_modules_' ; then \
                $(MAKE) SUBDIRS=modules/`echo "$@" | sed -r 's/^check_unit_modules_//g'`/tests check ; \
        elif test $${target:0:11} = 'check_unit_' ; then \
                $(MAKE) SUBDIRS=tests/unit check_PROGRAMS=`echo "$@" | sed -r 's/^check_unit_//g'` check ; \
        elif test $${target:0:17} = 'check_functional_' ; then \
                $(MAKE) SUBDIRS=pe-tests/functional testcase=`echo "$@" | sed -r 's/^check_functional_//g'` check ; \
        else \
                echo "Unknown type of test: '`echo $@ | sed -r 's/^check_([^_]+)_.*/\\1/g'`'" ; \
        fi

ChangeLog:
	(cd $(top_srcdir); git log) > $@
	
