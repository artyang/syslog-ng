SUBDIRS = affile afsocket syslogformat basicfuncs basic-proto confgen convertfuncs csvparser dbparser dummy afprog afsql afstreams afuser pacctformat json cryptofuncs java java-modules

# windows plugins
SUBDIRS += agent-config eventlog windows-resource

# mongodb
SUBDIRS += afmongodb

# smtp
SUBDIRS += afsmtp

# pe modules
SUBDIRS += license confighash diskq logstore-common logstore rltp-proto snmp

if ENABLE_INDEXER
SUBDIRS += indexed_logstore
endif

if !IS_WINDOWS
SUBDIRS += afsqlsource systemd-journal
endif
