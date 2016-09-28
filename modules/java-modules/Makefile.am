if ENABLE_JAVA

export GRADLE_OPTS
export abs_top_builddir
export abs_top_srcdir

SYSLOG_NG_CORE_JAR=$(top_builddir)/modules/java/syslog-ng-core/libs/syslog-ng-core.jar

JAVA_MOD_DST_DIR=$(DESTDIR)/$(moduledir)/java-modules
MOD_JARS=$(shell find $(abs_top_builddir)/modules/java-modules -name '*.jar' -not -path "$(abs_top_builddir)/modules/java-modules/.gradle/*")
GRADLE_WORKDIR=$(abs_top_builddir)/modules/java-modules/.gradle

java-modules: $(SYSLOG_NG_CORE_JAR)
	$(AM_V_GEN) $(GRADLE) --project-cache-dir $(GRADLE_WORKDIR) -g $(GRADLE_WORKDIR) -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java/syslog-ng-core/libs build

all-local: java-modules

log4j-copy-jar:
	$(AM_V_GEN) $(GRADLE) --project-cache-dir $(GRADLE_WORKDIR) -g $(GRADLE_WORKDIR) -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java/syslog-ng-core/libs -PjarDestDir=$(JAVA_MOD_DST_DIR) copyLog4j

jest-copy-jar:
	$(AM_V_GEN) $(GRADLE) --project-cache-dir $(GRADLE_WORKDIR) -g $(GRADLE_WORKDIR) -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java/syslog-ng-core/libs -PjarDestDir=$(JAVA_MOD_DST_DIR) copyJestRuntimeDeps

java-modules-install-exec-hook: log4j-copy-jar jest-copy-jar
	$(mkinstalldirs) $(JAVA_MOD_DST_DIR)
	cp $(MOD_JARS) $(JAVA_MOD_DST_DIR)

java-modules-uninstall-exec-hook:
	rm -f $(JAVA_MOD_DST_DIR)/*.jar

java-modules-clean-hook:
	rm -rf $(abs_top_builddir)/modules/java-modules/*.log
	$(GRADLE) --project-cache-dir $(GRADLE_WORKDIR) -q -g $(GRADLE_WORKDIR) -p $(abs_top_srcdir)/modules/java-modules -PsyslogBuildDir=$(abs_top_builddir)/modules/java-modules -PsyslogDepsDir=$(abs_top_builddir)/modules/java/syslog-ng-core/libs clean
	rm -rf $(GRADLE_WORKDIR)

install-exec-hook: java-modules-install-exec-hook
uninstall-hook: java-modules-uninstall-exec-hook
clean-local: java-modules-clean-hook

endif

EXTRA_DIST = \
	common/build.gradle  \
	elastic/build.gradle \
	elastic-v2/build.gradle \
	kafka/build.gradle \
	hdfs/build.gradle    \
	dummy/build.gradle   \
	http/build.gradle   \
	settings.gradle \
	build.gradle \
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
	kafka/src/main/java/org/syslog_ng/KafkaDestination.java \
	kafka/src/main/java/org/syslog_ng/KafkaDestinationOptions.java \
	kafka/src/main/java/org/syslog_ng/KafkaDestinationProperties.java \
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
	hdfs/src/main/java/org/syslog_ng/hdfs/HdfsOptions.java \
	http/src/main/java/org/syslog_ng/http/HTTPDestination.java \
	http/src/main/java/org/syslog_ng/http/HTTPDestinationOptions.java \
	dummy/src/main/java/org/syslog_ng/DummyTextDestination.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/ESIndex.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/ESMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/esnative/ESNativeMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/esnative/DummyProcessorNative.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/esnative/ESBulkNativeMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/ESMessageProcessorFactory.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/esnative/ESSingleNativeMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/ElasticSearchDestination.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/ESClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/esnative/ESTransportClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/esnative/ESNativeClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/esnative/ESNodeClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/UnknownESClientModeException.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/ESClientFactory.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/ElasticSearchOptions.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/esnative/ESTransportShieldClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/client/http/ESHttpClient.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/http/HttpBulkMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/http/HttpMessageProcessor.java \
	elastic-v2/src/main/java/org/syslog_ng/elasticsearch_v2/messageprocessor/http/HttpSingleMessageProcessor.java
