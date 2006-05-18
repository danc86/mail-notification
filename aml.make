GOBFLAGS = --always-private-header --no-touch --exit-on-warn

Makefile.am: automake.xml $(AML)
	$(AML) $< > $@.aml && mv -f $@.aml $@

# intentionally do not add $(GOB2) to sources (generated files are distributed)
%.gob.stamp: %.gob
	$(GOB2) $(GOBFLAGS) $<
	@touch $@

%-common.c %-stubs.c %-skels.c %.h: %.idl $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $<
