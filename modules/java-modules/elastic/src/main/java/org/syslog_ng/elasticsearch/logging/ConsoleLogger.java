package org.syslog_ng.elasticsearch.logging;


public class ConsoleLogger implements Logger {

	@Override
	public void fatal(String message) {
		System.err.println(message);
	}

	@Override
	public void error(String message) {
		System.err.println(message);
	}

	@Override
	public void warning(String message) {
		System.err.println(message);
	}

	@Override
	public void notice(String message) {
		System.out.println(message);
	}

	@Override
	public void info(String message) {
		System.out.println(message);
	}

	@Override
	public void debug(String message) {
		System.out.println(message);
	}

}
