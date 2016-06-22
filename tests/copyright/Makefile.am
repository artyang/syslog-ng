EXTRA_DIST = \
	check.sh \
	policy \
	license.text.GPLv2+.txt \
	license.text.GPLv2+_SSL.txt \
	license.text.LGPLv2.1+.txt \
	license.text.LGPLv2.1+_SSL.txt

check-local: check-copyright

check-copyright:
	@$(top_srcdir)/tests/copyright/check.sh $(top_srcdir) $(top_builddir) policy

CLEAN_HOOKS = clean-check-copyright

clean-check-copyright:
	rm -f $(top_builddir)/copyright-run.log $(top_builddir)/copyright-err.log
