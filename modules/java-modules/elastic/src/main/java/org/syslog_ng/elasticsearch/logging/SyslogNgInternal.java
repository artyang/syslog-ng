package org.syslog_ng.elasticsearch.logging;

import org.syslog_ng.InternalMessageSender;

public class SyslogNgInternal implements Logger {

	@Override
	public void fatal(String message) {
		InternalMessageSender.fatal(message);
	}

	@Override
	public void error(String message) {
		InternalMessageSender.error(message);
	}

	@Override
	public void warning(String message) {
		InternalMessageSender.warning(message);
	}

	@Override
	public void notice(String message) {
		InternalMessageSender.notice(message);
	}

	@Override
	public void info(String message) {
		InternalMessageSender.info(message);
	}

	@Override
	public void debug(String message) {
		InternalMessageSender.debug(message);
	}

}
