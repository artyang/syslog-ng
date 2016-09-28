EXTRA_DIST = \
	check.sh \
	policy \
	license.text.GPLv2+.txt \
	license.text.GPLv2+_SSL.txt \
	license.text.LGPLv2.1+.txt \
	license.text.LGPLv2.1+_SSL.txt \
	license.text.expat.txt

check-local: check-copyright

check-copyright:
	@$(abs_top_srcdir)/tests/copyright/check.sh $(abs_top_srcdir) $(abs_top_builddir) policy

clean-local:
	rm -f $(abs_top_builddir)/copyright-run.log $(abs_top_builddir)/copyright-err.log
