# we need the dotdot version of the include path to find generated headers
AM_CFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/libtest -I../../lib -I$(top_srcdir)/modules/basic-proto -I$(top_srcdir)/modules
AM_LDFLAGS = \
 -dlpreopen $(top_builddir)/modules/syslogformat/libsyslogformat.la \
 -dlpreopen $(top_builddir)/lib/libsyslog-ng.la \
  -dlpreopen $(top_builddir)/modules/basic-proto/libbasic-proto.la
LIBS = @TOOL_DEPS_LIBS@
LDADD = $(top_builddir)/lib/libsyslog-ng.la $(top_builddir)/libtest/libsyslog-ng-test.a @TOOL_DEPS_LIBS@

export top_srcdir

check_PROGRAMS =			\
    test_compat \
	test_utf8utils

test_utf8utils_SOURCES = test_utf8utils.c
test_compat_SOURCES = test_compat.c

TESTS = $(check_PROGRAMS)
