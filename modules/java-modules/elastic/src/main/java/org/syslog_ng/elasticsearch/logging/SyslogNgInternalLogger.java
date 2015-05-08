package org.syslog_ng.elasticsearch.logging;

import org.apache.log4j.AppenderSkeleton;
import org.apache.log4j.Level;
import org.apache.log4j.spi.LoggingEvent;
import org.syslog_ng.InternalMessageSender;

public class SyslogNgInternalLogger extends AppenderSkeleton {

	@Override
	public void close() {
	}

	@Override
	public boolean requiresLayout() {
		return false;
	}

	@Override
	protected void append(LoggingEvent event) {
		 String message = null;
	        if(event.locationInformationExists()){
	            StringBuilder formatedMessage = new StringBuilder();
	            formatedMessage.append(event.getLocationInformation().getClassName());
	            formatedMessage.append(".");
	            formatedMessage.append(event.getLocationInformation().getMethodName());
	            formatedMessage.append(":");
	            formatedMessage.append(event.getLocationInformation().getLineNumber());
	            formatedMessage.append(" - ");
	            formatedMessage.append(event.getMessage().toString());
	            message = formatedMessage.toString();
	        }else{
	            message = event.getMessage().toString();
	        }

	        switch(event.getLevel().toInt()){
	        case Level.INFO_INT:
	            InternalMessageSender.info(message);
	            break;
	        case Level.DEBUG_INT: 
	            InternalMessageSender.debug(message);
	            break;
	        case Level.ERROR_INT:
	            InternalMessageSender.error(message);
	            break;
	        case Level.WARN_INT:
	        	InternalMessageSender.warning(message);
	            break;
	        case Level.TRACE_INT:
	            InternalMessageSender.debug(message);
	            break;
	        default:
	            break;
	        }
	}

}
