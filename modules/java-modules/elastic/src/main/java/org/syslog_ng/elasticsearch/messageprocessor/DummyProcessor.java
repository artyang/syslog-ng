package org.syslog_ng.elasticsearch.messageprocessor;

import org.elasticsearch.action.index.IndexRequest;
import org.syslog_ng.elasticsearch.client.ESClient;
import org.syslog_ng.elasticsearch.logging.InternalLogger;
import org.syslog_ng.elasticsearch.options.ElasticSearchOptions;

public class DummyProcessor extends ESMessageProcessor {

	public DummyProcessor(ElasticSearchOptions options, ESClient client) {
		super(options, client);
	}

	@Override
	public void init() {
		InternalLogger.warning("Using option(\"flush_limit\", \"0\"), means only testing the Elasticsearch client site without sending logs to the ES");
	}

	@Override
	public boolean send(IndexRequest req) {
		return true;
	}

}
