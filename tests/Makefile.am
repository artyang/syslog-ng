EXTRA_DIST = \
	collect-cov.sh \
	copyright/check.sh \
	copyright/policy \
	copyright/license.text.GPLv2+.txt \
	copyright/license.text.GPLv2+_SSL.txt \
	copyright/license.text.LGPLv2.1+.txt \
	copyright/license.text.LGPLv2.1+_SSL.txt

SUBDIRS = unit loggen complex

check-copyright:
	@$(top_srcdir)/tests/copyright/check.sh $(top_srcdir) $(top_builddir) policy

CLEAN_HOOKS = clean-check-copyright

clean-check-copyright:
	rm $(top_builddir)/copyright-run.log $(top_builddir)/copyright-err.log

TESTS = collect-cov.sh check-copyright
