package org.syslog_ng.elasticsearch.logging;

public interface Logger {
	public void fatal(String message);
	public void error(String message);
	public void warning(String message);
	public void notice(String message);
	public void info(String message);
	public void debug(String message);
}
