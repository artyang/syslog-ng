moduledir = @moduledir@
AM_CPPFLAGS = -I$(top_srcdir)/lib -I../../lib
export top_srcdir

module_LTLIBRARIES := libaffile.la

include filemonitor/Makefile.am

libaffile_la_SOURCES =   \
  affile-grammar.y       \
  affile-parser.c        \
  affile-parser.h        \
  affile-plugin.c        \
  uniq_queue.c           \
  uniq_queue.h           \
  affile_common.h        \
  affile_common.c        \
  affile.h               \
  $(filemonitor_headers) \
  $(filemonitor_sources)

if IS_WINDOWS
libaffile_la_SOURCES += affile_win.c
else
libaffile_la_SOURCES += affile_unix.c
endif

BUILT_SOURCES = affile-grammar.y affile-grammar.c affile-grammar.h
EXTRA_DIST = $(BUILT_SOURCES) affile-grammar.ym

libaffile_la_CPPFLAGS = $(AM_CPPFLAGS)
libaffile_la_LIBADD = $(MODULE_DEPS_LIBS) $(PCRE_LIBS)
libaffile_la_LDFLAGS = $(MODULE_LDFLAGS)

include $(top_srcdir)/build/lex-rules.am
