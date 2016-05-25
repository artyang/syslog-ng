moduledir = @moduledir@
export top_srcdir

AM_CPPFLAGS = -I$(top_srcdir)/libtest -I$(top_srcdir)/lib -I../../lib -I$(top_srcdir)/modules/kvformat

LDADD = $(top_builddir)/libtest/libsyslog-ng-test.a $(top_builddir)/lib/libsyslog-ng.la @TOOL_DEPS_LIBS@

check_PROGRAMS = test_format_welf test_kv_scanner test_linux_audit_scanner test_kv_parser

test_format_welf_SOURCES = test_format_welf.c
test_format_welf_DEPENDENCIES = $(top_builddir)/modules/kvformat/libkvformat.la
test_format_welf_LDFLAGS = \
 -dlpreopen ../../syslogformat/libsyslogformat.la \
 -dlpreopen $(top_builddir)/modules/kvformat/libkvformat.la

test_kv_parser_SOURCES = test_kv_parser.c
test_kv_parser_DEPENDENCIES = $(top_builddir)/modules/kvformat/libkvformat.la
test_kv_parser_LDFLAGS = \
 -dlpreopen ../../syslogformat/libsyslogformat.la \
 -dlpreopen $(top_builddir)/modules/kvformat/libkvformat.la

test_kv_scanner_SOURCES = test_kv_scanner.c
test_kv_scanner_DEPENDENCIES = $(top_builddir)/modules/kvformat/libkvformat.la
test_kv_scanner_LDFLAGS = \
 -dlpreopen $(top_builddir)/modules/kvformat/libkvformat.la

test_linux_audit_scanner_SOURCES = test_linux_audit_scanner.c
test_linux_audit_scanner_DEPENDENCIES = $(top_builddir)/modules/kvformat/libkvformat.la
test_linux_audit_scanner_LDFLAGS = \
 -dlpreopen $(top_builddir)/modules/kvformat/libkvformat.la

TESTS = $(check_PROGRAMS)
