moduledir = @moduledir@
export top_srcdir

if !IS_WINDOWS

AM_CPPFLAGS = -I$(top_srcdir)/libtest -I$(top_srcdir)/lib -I../../lib -I$(top_srcdir)/modules/cef

AM_LDFLAGS = -dlpreopen ../../syslogformat/libsyslogformat.la -dlpreopen $(top_builddir)/modules/cef/libcef.la

LDADD = $(JSON_LIBS) $(top_builddir)/lib/libsyslog-ng.la $(top_builddir)/libtest/libsyslog-ng-test.a @TOOL_DEPS_LIBS@

check_PROGRAMS = test-format-cef-extension

test_format_cef_extension_SOURCES = test-format-cef-extension.c
test_format_cef_extension_DEPENDENCIES = $(top_builddir)/modules/cef/libcef.la

TESTS = $(check_PROGRAMS)

endif
