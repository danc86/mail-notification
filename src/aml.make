if WITH_EVOLUTION
evolution_eplug = org-jylefort-mail-notification.eplug
evolution_plugin_DATA = $(evolution_eplug)
endif

IDLFLAGS = $(BONOBO_IDLFLAGS)

CLEANFILES += $(evolution_eplug)
EXTRA_DIST = automake.xml $(evolution_eplug).in

%.eplug: %.eplug.in
	sed -e 's|\@PLUGINDIR\@|$(evolution_plugindir)|' $< > $@
