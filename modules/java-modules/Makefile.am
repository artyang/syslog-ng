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
        elastic/build.gradle \
        kafka/build.gradle	\
        common/build.gradle  \
        hdfs/build.gradle    \
        dummy/build.gradle   \
        settings.gradle \
        elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESMessageProcessor.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/DummyProcessor.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESBulkMessageProcessor.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESMessageProcessorFactory.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/messageprocessor/ESSingleMessageProcessor.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/ElasticSearchDestination.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESTransportClient.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESClient.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESNodeClient.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/client/UnknownESClientModeException.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/client/ESClientFactory.java \
        elastic/src/main/java/org/syslog_ng/elasticsearch/ElasticSearchOptions.java \
        kafka/src/main/java/org/syslog_ng/kafka/KafkaDestination.java \
        common/src/main/java/org/syslog_ng/logging/SyslogNgInternalLogger.java \
        common/src/main/java/org/syslog_ng/options/InvalidOptionException.java \
        common/src/main/java/org/syslog_ng/options/RequiredOptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/Option.java \
        common/src/main/java/org/syslog_ng/options/TemplateOption.java \
        common/src/main/java/org/syslog_ng/options/PortCheckDecorator.java \
        common/src/main/java/org/syslog_ng/options/OptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/BooleanOptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/EnumOptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/Options.java \
        common/src/main/java/org/syslog_ng/options/IntegerRangeCheckOptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/IntegerOptionDecorator.java \
        common/src/main/java/org/syslog_ng/options/StringOption.java \
        common/src/test/java/org/syslog_ng/options/test/TestEnumOptionDecorator.java \
        common/src/test/java/org/syslog_ng/options/test/TestRequiredOptionDecorator.java \
        common/src/test/java/org/syslog_ng/options/test/TestStringOption.java \
        common/src/test/java/org/syslog_ng/options/test/TestPortCheckDecorator.java \
        common/src/test/java/org/syslog_ng/options/test/TestBooleanOptionDecorator.java \
        common/src/test/java/org/syslog_ng/options/test/TestIntegerRangeCheckOptionDecorator.java \
        common/src/test/java/org/syslog_ng/options/test/TestOption.java \
        common/src/test/java/org/syslog_ng/options/test/TestIntegerOptionDecorator.java \
        common/src/test/java/org/syslog_ng/logging/test/MockLogDestination.java \
        hdfs/src/main/java/org/syslog_ng/hdfs/HdfsDestination.java \
        dummy/src/main/java/org/syslog_ng/DummyTextDestination.java \ 

