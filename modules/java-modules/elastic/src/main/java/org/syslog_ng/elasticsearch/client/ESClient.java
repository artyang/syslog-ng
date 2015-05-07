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

package org.syslog_ng.elasticsearch.client;

import org.elasticsearch.ElasticsearchException;
import org.elasticsearch.action.admin.cluster.health.ClusterHealthRequestBuilder;
import org.elasticsearch.action.admin.cluster.health.ClusterHealthResponse;
import org.elasticsearch.action.admin.cluster.health.ClusterHealthStatus;
import org.elasticsearch.client.Client;
import org.syslog_ng.elasticsearch.logging.InternalLogger;
import org.syslog_ng.elasticsearch.options.ElasticSearchOptions;

public abstract class ESClient {
	private Client client;
	private static final String TIMEOUT = "5s";

	protected ElasticSearchOptions options;

	public ESClient(ElasticSearchOptions options) {
		this.options = options;
	}

	private boolean waitForStatus(ClusterHealthStatus status) {
		ClusterHealthRequestBuilder healthRequest = client.admin().cluster().prepareHealth();
		healthRequest.setTimeout(TIMEOUT);
		healthRequest.setWaitForStatus(status);
		ClusterHealthResponse response = (ClusterHealthResponse) healthRequest.execute().actionGet();
		return !response.isTimedOut();
	}

	public void connect() throws ElasticsearchException {
		InternalLogger.info("connecting to cluster, cluster_name='" + options.getCluster() + "'");
		if (!waitForStatus(ClusterHealthStatus.GREEN)) {
			InternalLogger.debug("Failed to wait for green");
			InternalLogger.debug("Wait for read yellow status...");

			if (!waitForStatus(ClusterHealthStatus.YELLOW)) {
				InternalLogger.debug("Timedout");
				throw new ElasticsearchException("Can't connect to cluster: " + options.getCluster());
			}
		}
		InternalLogger.info("conneted to cluster, cluster_name='" + options.getCluster() + "'");
	}

	public final boolean open() {
		if (client == null) {
			client = createClient();
		}
		try {
			connect();
		}
		catch (ElasticsearchException e) {
			InternalLogger.error("Failed to connect to " +options.getCluster() + ", reason='" + e.getMessage() + "'");
			return false;
		}
		return true;
	}

	public abstract void close();

	public abstract boolean isOpened();

	public abstract void deinit();

	public final void init() {
		client = createClient();
	}

	public abstract Client createClient();

	public Client getClient() {
		return client;
	}

	protected void resetClient() {
		this.client = null;
	}
}
