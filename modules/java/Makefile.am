export top_srcdir

AUTOMAKE_OPTIONS = subdir-objects

if ENABLE_JAVA

JAVA_DST_DIR=$(DESTDIR)/$(moduledir)/java-modules
JAVA_BUILDDIR=$(builddir)
SYSLOG_NG_CORE_SRC_DIR = $(srcdir)/src/main/java/org/syslog_ng
SYSLOG_NG_CORE_CLASSES_DIR=$(builddir)/syslog-ng-core/classes
SYSLOG_NG_CORE_PACKAGE_DIR=$(builddir)/syslog-ng-core/classes/org/syslog_ng
SYSLOG_NG_CORE_LIBS_DIR=$(builddir)/syslog-ng-core/libs
SYSLOG_NG_CORE_JAR=$(top_builddir)/modules/java/syslog-ng-core/libs/syslog-ng-core.jar

LogDestination_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogPipe.class

LogPipe_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/InternalMessageSender.class

LogTemplate_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogMessage.class

StructuredLogDestination_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogDestination.class

SyslogNgClassLoader_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/InternalMessageSender.class

TextLogDestination_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogDestination.class

DummyTextDestination_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/TextLogDestination.class \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/InternalMessageSender.class

DummyStructuredDestination_CLASS_DEPS = \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/StructuredLogDestination.class \
  $(SYSLOG_NG_CORE_PACKAGE_DIR)/InternalMessageSender.class

java-headers: $(JAVA_HEADER_FILES)

java-binaries: $(SYSLOG_NG_CORE_JAR)

$(SYSLOG_NG_CORE_JAR): $(JAVA_CLASS_FILES) $(SYSLOG_NG_CORE_LIBS_DIR)
	$(AM_V_GEN)$(JAR) cvf $(SYSLOG_NG_CORE_JAR) -C $(SYSLOG_NG_CORE_CLASSES_DIR) .

$(SYSLOG_NG_CORE_LIBS_DIR) $(SYSLOG_NG_CORE_CLASSES_DIR) $(SYSLOG_NG_CORE_PACKAGE_DIR) $(JAVA_BUILDDIR):
	mkdir -p $@

$(JAVA_HEADER_FILES): $(JAVA_BUILDDIR)/org_syslog_ng_%.h: $(SYSLOG_NG_CORE_PACKAGE_DIR)/%.class
	$(AM_V_GEN)$(JAVAH) -classpath  $(SYSLOG_NG_CORE_CLASSES_DIR) -d $(JAVA_BUILDDIR) org.syslog_ng.$*

all-local: java-binaries java-headers

install-exec-hook:
	$(mkinstalldirs) $(JAVA_DST_DIR)
	cp $(SYSLOG_NG_CORE_JAR) $(JAVA_DST_DIR)

clean-local:
	rm -f $(SYSLOG_NG_CORE_PACKAGE_DIR)/*.class
	rm -f $(SYSLOG_NG_CORE_JAR)
	rm -f $(JAVA_HEADER_FILES)

uninstall-local:
	rm -f $(JAVA_DST_DIR)/*.jar


module_LTLIBRARIES = libmod-java.la

libmod_java_la_CFLAGS = \
    $(AM_CFLAGS) \
    $(JNI_CFLAGS)  \
    $(INCUBATOR_CFLAGS) \
    -I$(srcdir)    \
    -I$(builddir)  \
    -I$(srcdir)/native \
    -I$(builddir)/native \
    -I$(top_srcdir)/modules/license

JAVA_HEADER_FILES = \
   $(JAVA_BUILDDIR)/org_syslog_ng_LogMessage.h \
   $(JAVA_BUILDDIR)/org_syslog_ng_LogDestination.h \
   $(JAVA_BUILDDIR)/org_syslog_ng_LogTemplate.h \
   $(JAVA_BUILDDIR)/org_syslog_ng_InternalMessageSender.h

libmod_java_la_SOURCES = \
    native/java-grammar.y \
    native/java-plugin.c \
    native/java-destination.c \
    native/java-class-loader.c \
    native/java-class-loader.h \
    native/java-parser.c \
    native/java-parser.h \
    native/java_machine.c \
    native/java_machine.h \
    native/java-destination.h \
    $(JAVA_HEADER_FILES) \
    proxies/java-destination-proxy.c \
    proxies/java-destination-proxy.h \
    proxies/java-logmsg-proxy.c \
    proxies/java-logmsg-proxy.h \
    proxies/java-template-proxy.c \
    proxies/java-template-proxy.h \
    proxies/internal-message-sender-proxy.c 

libmod_java_la_LIBADD =  $(JNI_LIBS) $(MODULE_DEPS_LIBS)

libmod_java_la_LDFLAGS = \
    -avoid-version -module -no-undefined $(MODULE_LDFLAGS)

libmod_java_la_DEPENDENCIES  = \
        $(MODULE_DEPS_LIBS)

JAVA_FILES = \
    $(SYSLOG_NG_CORE_SRC_DIR)/InternalMessageSender.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/SyslogNgClassLoader.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/LogMessage.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/LogTemplate.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/LogPipe.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/LogDestination.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/TextLogDestination.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/StructuredLogDestination.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/DummyTextDestination.java \
    $(SYSLOG_NG_CORE_SRC_DIR)/DummyStructuredDestination.java


JAVA_CLASS_FILES = \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/InternalMessageSender.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/SyslogNgClassLoader.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogMessage.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogTemplate.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogPipe.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/LogDestination.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/TextLogDestination.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/StructuredLogDestination.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/DummyTextDestination.class \
   $(SYSLOG_NG_CORE_PACKAGE_DIR)/DummyStructuredDestination.class

endif

include $(top_srcdir)/build/lex-rules.am

BUILT_SOURCES = \
    native/java-grammar.y \
    native/java-grammar.c \
    native/java-grammar.h

EXTRA_DIST = \
    native/java-grammar.ym \
    $(JAVA_FILES)

if ENABLE_JAVA

BUILT_SOURCES += $(JAVA_HEADER_FILES)

endif

.SECONDEXPANSION:

$(JAVA_CLASS_FILES): $(SYSLOG_NG_CORE_PACKAGE_DIR)/%.class: $(SYSLOG_NG_CORE_SRC_DIR)/%.java $(SYSLOG_NG_CORE_CLASSES_DIR) $$($$*_CLASS_DEPS)
	$(AM_V_GEN)$(JAVAC) -classpath  $(SYSLOG_NG_CORE_CLASSES_DIR) -d $(SYSLOG_NG_CORE_CLASSES_DIR) $<

.PHONY: java-binaries java-headers java-install-exec-hook java-uninstall-exec-hook java-clean-hook all-local
