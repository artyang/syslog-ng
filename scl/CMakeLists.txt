set(SCL_DIRS
    apache
    cee
    cim
    checkpoint
    cisco
    collectd
    default-network-drivers
    discord
    elasticsearch
    ewmm
    fortigate
    graphite
    graylog2
    hdfs
    iptables
    junos
    kafka
    linux-audit
    loadbalancer
    loggly
    logmatic
    logscale
    mariadb
    mbox
    netskope
    nodejs
    osquery
    pacct
    paloalto
    python
    rewrite
    slack
    snmptrap
    solaris
    splunk
    sudo
    sumologic
    syslogconf
    system
    telegram
    websense
    windowseventlog
)

install(DIRECTORY ${SCL_DIRS} DESTINATION share/syslog-ng/include/scl)
install(FILES scl.conf DESTINATION share/syslog-ng/include)

if (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/etc/syslog-ng.conf)
    install(FILES syslog-ng.conf DESTINATION etc)
endif()
