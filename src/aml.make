if WITH_EVOLUTION
evolution_eplug = org-gnome-mail-notification.eplug
evolution_plugin_DATA = $(evolution_eplug)
endif

IDLFLAGS = $(BONOBO_IDLFLAGS)

CLEANFILES += $(evolution_eplug)
EXTRA_DIST = $(evolution_eplug).in

%.eplug: %.eplug.in
	sed -e 's|\@PLUGINDIR\@|$(evolution_plugindir)|' $< > $@
