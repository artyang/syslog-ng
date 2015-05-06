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

package org.syslog_ng.elasticsearch.messageprocessor;

import java.util.concurrent.TimeUnit;


import org.elasticsearch.action.bulk.BulkProcessor;
import org.elasticsearch.action.index.IndexRequest;
import org.elasticsearch.common.unit.TimeValue;
import org.syslog_ng.elasticsearch.client.ESClient;
import org.syslog_ng.elasticsearch.options.ElasticSearchOptions;

public class ESBulkMessageProcessor extends ESMessageProcessor {
	private BulkProcessor bulkProcessor;
	
	public ESBulkMessageProcessor(ElasticSearchOptions options, ESClient client, BulkProcessor.Listener listener) {
		super(options, client, listener);
	}

	@Override
	public boolean send(IndexRequest req) {
		bulkProcessor.add(req);
		return true;
	}
	
	@Override
	public void flush() {
		bulkProcessor.flush();
	}
	
	@Override
	public void init() {
		bulkProcessor = BulkProcessor.builder(
				client.getClient(),
				listener
			)
			.setBulkActions(options.getFlushLimit())
			.setFlushInterval(new TimeValue(1000))
			.setConcurrentRequests(0)
			.build();
	}
	
	@Override
	public void deinit() {
		try {
			bulkProcessor.awaitClose(1, TimeUnit.SECONDS);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
