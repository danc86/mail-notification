GOBFLAGS = --always-private-header --no-touch --exit-on-warn

# intentionally do not add aml/aml to sources (Makefile.am is distributed)
Makefile.am: automake.xml $(top_srcdir)/aml/aml.c $(top_srcdir)/aml/Makefile.aml
	cd $(top_srcdir)/aml && $(MAKE) -f Makefile.aml $(AM_MAKEFLAGS) aml
	$(top_srcdir)/aml/aml $< > $@.aml && mv -f $@.aml $@

# intentionally do not add $(GOB2) to sources (generated files are distributed)
%.gob.stamp: %.gob
	$(GOB2) $(GOBFLAGS) $<
	@touch $@

%-common.c %-stubs.c %-skels.c %.h: %.idl $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $<
