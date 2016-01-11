moduledir = @moduledir@
export top_srcdir

SUBDIRS = . tests

AM_CPPFLAGS = -I$(top_srcdir)/lib -I../../lib -I$(top_srcdir)/modules/cef -I$(top_builddir)/modules/cef
module_LTLIBRARIES = libcef.la

libcef_la_SOURCES	=	\
	format-cef-extension.c	\
	format-cef-extension.h	\
	cef-plugin.c

libcef_la_CPPFLAGS = $(AM_CPPFLAGS)
libcef_la_LIBADD = $(MODULE_DEPS_LIBS)
libcef_la_LDFLAGS = $(MODULE_LDFLAGS)
