package org.syslog_ng.elasticsearch.logging;


public class InternalLogger {
	private static Logger logger;
	private static String prefix = "[ElasticSearchDestination] ";

	public static void setLogger(Logger logger) {
		InternalLogger.logger = logger;
	}

	public static void fatal(String message) {
		if (logger != null) {
			logger.fatal(prefix + message);
		}
	}

	public static void error(String message) {
		if (logger != null) {
			logger.error(prefix + message);
		}
	}

	public static void warning(String message) {
		if (logger != null) {
			logger.warning(prefix + message);
		}
	}

	public static void notice(String message) {
		if (logger != null) {
			logger.notice(prefix + message);
		}
	}

	public static void info(String message) {
		if (logger != null) {
			logger.info(prefix + message);
		}
	}

	public static void debug(String message) {
		if (logger != null) {
			logger.debug(prefix + message);
		}
	}
}
