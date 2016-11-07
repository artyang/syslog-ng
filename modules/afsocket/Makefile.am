moduledir = @moduledir@
AM_CPPFLAGS = -I$(top_srcdir)/lib -I../../lib
export top_srcdir

if ENABLE_SYSTEMD
if WITH_LIBSYSTEMD
SYSTEMD_SOURCES =
else
SYSTEMD_SOURCES = sd-daemon.c sd-daemon.h
endif
else
SYSTEMD_SOURCES =
endif

module_LTLIBRARIES = libafsocket.la
libafsocket_la_SOURCES =  \
  afsocket-grammar.y      \
  afsocket-parser.c       \
  afsocket-parser.h       \
  afsocket-plugin.c       \
  afsocket.c              \
  afsocket.h              \
  afinet.c                \
  afinet.h                \
  socket-options-inet.c   \
  socket-options-inet.h   \
  socket-options.h        \
  socket-options.c

if !IS_WINDOWS
libafsocket_la_SOURCES += \
	afunix.c afunix.h $(SYSTEMD_SOURCES)
endif

libafsocket_la_CPPFLAGS = $(AM_CPPFLAGS) $(libsystemd_daemon_CFLAGS) 
libafsocket_la_LIBADD = $(MODULE_DEPS_LIBS) $(OPENSSL_LIBS) $(ZLIB_LIBS) $(LIBNET_LIBS) $(LIBWRAP_LIBS) $(PCRE_LIBS) $(libsystemd_daemon_LIBS)
libafsocket_la_LDFLAGS = $(MODULE_LDFLAGS)

BUILT_SOURCES = afsocket-grammar.y afsocket-grammar.c afsocket-grammar.h
EXTRA_DIST = $(BUILT_SOURCES) afsocket-grammar.ym

CLEANFILES = libafsocket.la

include $(top_srcdir)/build/lex-rules.am
