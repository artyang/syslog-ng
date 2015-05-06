package org.syslog_ng.elasticsearch.options;

import org.syslog_ng.LogMessage;
import org.syslog_ng.LogTemplate;

public class TemplateOption extends OptionDecorator {
	private LogTemplate template;
	private String strTemplate;
	private long configHandle;
	
	public TemplateOption(long configHandle, Option decoratedOption) {
		super(decoratedOption);
		this.configHandle = configHandle;
	}
	
	@Override
	public void init() throws OptionException {
		decoratedOption.init();
		strTemplate = decoratedOption.getValue();
		if (strTemplate != null) {
			template = new LogTemplate(configHandle);
			if(!template.compile(strTemplate)) {
				throw new OptionException("Can't compile template: '" + strTemplate + "'");
			}
		}
	}
	
	@Override
	public void deinit() {
		if (template != null)
			template.release();
		decoratedOption.deinit();
	}

	public String getResolvedString(LogMessage msg) {
		if (template != null)
			return template.format(msg);
		return null;
	}
}
