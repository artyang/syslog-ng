export top_srcdir
moduledir = @moduledir@
AM_CPPFLAGS = -I$(top_srcdir)/lib -I../lib

sbin_PROGRAMS = syslog-ng-query

syslog_ng_query_SOURCES = syslog-ng-query.c  ../syslog-ng-ctl/control_client.h ../lib/gsocket.c ../lib/gsockaddr.c
if IS_WINDOWS
syslog_ng_query_SOURCES += ../syslog-ng-ctl/control_client_windows.c
else
syslog_ng_query_SOURCES += ../syslog-ng-ctl/control_client_posix.c
endif

syslog_ng_query_LDADD =  $(SYSLOG_NG_LIBS)
