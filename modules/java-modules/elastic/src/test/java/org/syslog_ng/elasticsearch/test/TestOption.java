package org.syslog_ng.elasticsearch.test;

import static org.junit.Assert.*;

import java.util.Hashtable;

import org.syslog_ng.LogDestination;
import org.syslog_ng.elasticsearch.options.Option;
import org.syslog_ng.elasticsearch.options.OptionException;

public class TestOption {
	public LogDestination owner;
	public Hashtable<String, String> options;
	
	public void setUp() throws Exception {
		options = new Hashtable<String, String>();
		owner = new MockLogDestination(options);
	}
	
	
	public void assertInitOptionSuccess(Option option) {
		try {
			option.init();
		}
		catch (OptionException e) {
			throw new AssertionError("Initialization failed: " + e.getMessage());
		}
	}
	
	public void assertInitOptionFailed(Option option) {
		assertInitOptionFailed(option, null);
	}
	
	public void assertInitOptionFailed(Option option, String expectedErrorMessage) {
		try {
			option.init();
			throw new AssertionError("Initialization should be failed");
		}
		catch (OptionException e) {
			if (expectedErrorMessage != null) {
				assertEquals(expectedErrorMessage, e.getMessage().substring(0, expectedErrorMessage.length()));
			}
		}
	}
}
