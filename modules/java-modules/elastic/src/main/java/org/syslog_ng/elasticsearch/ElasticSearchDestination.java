/*
 * Copyright (c) 2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2015 Viktor Juhasz <viktor.juhasz@balabit.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

package org.syslog_ng.elasticsearch;

import org.syslog_ng.LogMessage;
import org.syslog_ng.StructuredLogDestination;
import org.syslog_ng.elasticsearch.client.ESClient;
import org.syslog_ng.elasticsearch.client.ESClientFactory;
import org.syslog_ng.elasticsearch.logging.InternalLogger;
import org.syslog_ng.elasticsearch.logging.SyslogNgInternal;
import org.syslog_ng.elasticsearch.messageprocessor.ESMessageProcessor;
import org.syslog_ng.elasticsearch.messageprocessor.ESMessageProcessorFactory;
import org.syslog_ng.elasticsearch.options.ElasticSearchOptions;
import org.syslog_ng.elasticsearch.options.OptionException;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.elasticsearch.action.bulk.BulkProcessor;
import org.elasticsearch.action.bulk.BulkRequest;
import org.elasticsearch.action.bulk.BulkResponse;
import org.elasticsearch.action.index.IndexRequest;

public class ElasticSearchDestination extends StructuredLogDestination implements BulkProcessor.Listener {

	ESClient client;
	ESMessageProcessor msgProcessor;
	ElasticSearchOptions options;
	
	boolean opened;
	
	public ElasticSearchDestination(long handle) {
		super(handle);
		Logger.getRootLogger().setLevel(Level.OFF);
		InternalLogger.setLogger(new SyslogNgInternal());
		opened = false;
		options = new ElasticSearchOptions(this);
		client = ESClientFactory.getESClient(options);
		msgProcessor = ESMessageProcessorFactory.getMessageProcessor(options, client, this);
		if (options.getClientMode().equals(ElasticSearchOptions.CLIENT_MODE_TRANSPORT) && options.getFlushLimit() > 1) {
			InternalLogger.warning("Using transport client mode with bulk message processing (flush_limit > 1) can cause high message dropping rate in case of connection broken, using node client mode is suggested");
		}
	}

	@Override
	protected boolean init() {
		boolean result = false;
		try {
			options.init();
			client.init();
			result = true;
		}
		catch (OptionException e){
			InternalLogger.error(e.getMessage());
			return false;
		}
		return result;
	}

	@Override
	protected boolean isOpened() {
		return opened;
	}

	@Override
	protected boolean open() {
		opened = client.open();
		msgProcessor.init();
		return opened;
	}
	
    protected IndexRequest createIndexRequest(LogMessage msg) {
    	String formattedMessage = options.getMessageTemplate().getResolvedString(msg);
		String customId = options.getCustomId().getResolvedString(msg);
		String index = options.getIndex().getResolvedString(msg);
		String type = options.getType().getResolvedString(msg);
		InternalLogger.debug("Outgoing log entry, json='" + formattedMessage + "'");
	    return new IndexRequest(index, type, customId).source(formattedMessage);
    }

	@Override
	protected boolean send(LogMessage msg) {
		if (!client.isOpened()) {
			close();
			return false;
		}
		return msgProcessor.send(createIndexRequest(msg));
	}

	@Override
	protected void close() {
		if (opened) {
			msgProcessor.flush();
			msgProcessor.deinit();
			client.close();
			opened = false;
		}
	}

	@Override
	protected void deinit() {
		client.deinit();
		options.deinit();
	}

	@Override
	public void beforeBulk(long executionId, BulkRequest request) {
		InternalLogger.debug("Start bulk processing, id='" + executionId + "'");
	}

	@Override
	public void afterBulk(long executionId, BulkRequest request,
			BulkResponse response) {
		InternalLogger.debug("Bulk processing finished successfully, id='" + executionId + "'");
	}

	@Override
	public void afterBulk(long executionId, BulkRequest request,
			Throwable failure) {
		String errorMessage = "Bulk processing failed,";
		errorMessage += " id='" + executionId + "'";
		errorMessage += ", numberOfMessages='" + request.numberOfActions() + "'";
		errorMessage += ", error='" + failure.getMessage() + "'";
		InternalLogger.error(errorMessage);
	}
}
