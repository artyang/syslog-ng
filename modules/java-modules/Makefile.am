export abs_top_srcdir
export abs_top_builddir

AUTOMAKE_OPTIONS = subdir-objects

if ENABLE_JAVA

JAVA_MOD_DST_DIR=$(DESTDIR)/$(moduledir)/java-modules
JARS=$(shell find $(abs_top_builddir)/modules/java-modules -name '*.jar')

all-local:
	gradle -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java build

install-exec-local:
	$(mkinstalldirs) $(JAVA_MOD_DST_DIR)
	cp $(JARS) $(JAVA_MOD_DST_DIR)

uninstall-local:
	rm -f $(JAVA_MOD_DST_DIR)/*.jar

eclipse:
	gradle -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java eclipse

endif

EXTRA_DIST = \
	settings.gradle 	\
	dummy/build.gradle	\
	dummy/src/main/java/org/syslog_ng/DummyTextDestination.java  \
	hdfs/build.gradle	\
	hdfs/src/main/java/org/syslog_ng/hdfs/HdfsDestination.java \
  elastic/build.gradle \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/InvalidOptionException.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/RequiredOptionDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/Option.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/TemplateOption.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/PortCheckDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/OptionDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/EnumOptionDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/Options.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/IntegerRangeCheckOptionDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/IntegerOptionDecorator.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/ElasticSearchOptions.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/options/StringOption.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESMessageProcessor.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/DummyProcessor.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESBulkMessageProcessor.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESMessageProcessorFactory.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESSingleMessageProcessor.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/ElasticSearchDestination.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/logging/SyslogNgInternalLogger.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESTransportClient.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESClient.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESNodeClient.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/client/UnknownESClientModeException.java \
  elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESClientFactory.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestEnumOptionDecorator.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestRequiredOptionDecorator.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestStringOption.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestPortCheckDecorator.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestIntegerRangeCheckOptionDecorator.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestOption.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/TestIntegerOptionDecorator.java \
  elastic/src/test/java/org/syslog_ng/elasticsearch/test/MockLogDestination.java
