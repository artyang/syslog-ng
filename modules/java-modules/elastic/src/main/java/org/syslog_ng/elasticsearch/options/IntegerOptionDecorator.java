package org.syslog_ng.elasticsearch.options;

public class IntegerOptionDecorator extends OptionDecorator {

	public IntegerOptionDecorator(Option decoratedOption) {
		super(decoratedOption);
	}
	
	public void init() throws OptionException {
		decoratedOption.init();
		try {
			Integer.parseInt(decoratedOption.getValue());
		}
		catch (NumberFormatException e) {
			throw new OptionException("option " + decoratedOption.getName() + " must be numerical");
			
		}
	}
	

}
