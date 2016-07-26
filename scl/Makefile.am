SCL_SUBDIRS = system pacct syslogconf snmp sql elasticsearch kafka hdfs
SCL_CONFIGS = scl.conf                  \
              modules.conf              \
              syslog-ng.conf            \
              elasticsearch/plugin.conf \
              hdfs/plugin.conf          \
              pacct/plugin.conf         \
              kafka/plugin.conf         \
              syslogconf/plugin.conf    \
              syslogconf/README         \
              syslogconf/convert-syslogconf.awk \
              system/plugin.conf        \
              system/generate-system-source.sh

EXTRA_DIST = $(SCL_CONFIGS)

scldir = $(datadir)/include/scl

install-data-local:
	for cfg in $(SCL_CONFIGS); do \
		if [ -f $(DESTDIR)$(sysconfdir)/$${cfg} ]; then \
			echo "Not overwriting existing configuration file, you might want to upgrade manually: $${cfg}"; \
		else \
			$(install_sh_DATA) $(srcdir)/$${cfg}  $(DESTDIR)$(sysconfdir)/$${cfg}; \
		fi; \
	done
	$(mkinstalldirs) $(DESTDIR)$(scldir)
	(cd $(srcdir); tar cfh - $(SCL_SUBDIRS)) | (cd $(DESTDIR)$(scldir) && tar xf -)

uninstall-local:
	for cfg in $(SCL_CONFIGS); do \
		rm -f $(DESTDIR)$(sysconfdir)/$${cfg}; \
	done
	rm -rf $(DESTDIR)$(scldir)
