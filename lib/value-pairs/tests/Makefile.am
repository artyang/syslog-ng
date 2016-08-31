value_pairs_tests_TESTS			=  \
	value-pairs/tests/test_value_pairs     \
	value-pairs/tests/test_value_pairs_walk

value_pairs_tests_test_value_pairs_LDADD	= \
	$(top_builddir)/lib/libsyslog-ng.la	\
	$(top_builddir)/libtest/libsyslog-ng-test.a	\
	@TOOL_DEPS_LIBS@

value_pairs_tests_test_value_pairs_LDFLAGS	= -dlpreopen $(top_builddir)/modules/syslogformat/libsyslogformat.la

value_pairs_tests_test_value_pairs_walk_CPPFLAGS = -I$(top_srcdir)/libtest
value_pairs_tests_test_value_pairs_walk_LDADD	= \
	$(top_builddir)/lib/libsyslog-ng.la	\
	$(top_builddir)/libtest/libsyslog-ng-test.a	\
	@TOOL_DEPS_LIBS@
