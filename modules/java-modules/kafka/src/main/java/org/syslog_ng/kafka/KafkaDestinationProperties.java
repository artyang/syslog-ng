package org.syslog_ng.kafka;

import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.common.serialization.StringSerializer;
import org.apache.log4j.Logger;
import org.syslog_ng.logging.SyslogNgInternalLogger;
import org.syslog_ng.options.KafkaDestinationOptions;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class KafkaDestinationProperties {
    private KafkaDestinationOptions options;
    private Properties properties;
    private Logger logger;

    public KafkaDestinationProperties(KafkaDestinationOptions options) {
        this.options = options;
        logger = Logger.getLogger("root");
    }

    public void init() {
        String propertiesFile = options.getPropertiesFile();
        initPropertiesWithDefaultValues();

        if (propertiesFile != null)
            mergePropertiesFromFile(propertiesFile);
    }

    public Properties getProperties() {
        return properties;
    }

    private void initPropertiesWithDefaultValues() {
        properties = new Properties();
        properties.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, options.getKafkaBootstrapServers());
        properties.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        properties.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
    }

    private boolean mergePropertiesFromFile(String propertiesFile) {
        InputStream inputStream = openPropertiesFile(propertiesFile);

        if (inputStream != null)
        {
            return mergePropertiesFromStream(inputStream);
        }

        return false;
    }

    private InputStream openPropertiesFile(String propertiesFile) {
        InputStream inputStream = null;

        try {
            inputStream = new FileInputStream(propertiesFile);
            logger.debug(String.format("properties file successfully opened: %s", propertiesFile));
        } catch (FileNotFoundException e) {
            logger.error(String.format("unable to open properties file: %s", propertiesFile));
            e.printStackTrace();
        }
        return inputStream;
    }

    private boolean mergePropertiesFromStream(InputStream inputStream) {
        Properties mergedProperties = new Properties();

        try {
            mergedProperties.load(inputStream);
            mergedProperties.putAll(properties);
            properties = mergedProperties;
            logger.debug("properties successfully loaded");
            return true;
        } catch (IOException e) {
            logger.error("unable to load properties");
            e.printStackTrace();
        }
        return false;
    }
}
