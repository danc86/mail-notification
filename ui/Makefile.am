uidir = $(pkgdatadir)/ui

if WITH_POP3_OR_IMAP_OR_GMAIL
authentication_interface = authentication.glade
authentication_project = authentication.gladep
endif

dist_ui_DATA = \
	$(authentication_interface)		\
	edit-toolbars.glade			\
	mailbox-properties.glade		\
	main.glade				\
	menus.xml				\
	properties.glade			\
	summary-popup.glade			\
	toolbars.xml				\
	welcome.glade

EXTRA_DIST = \
	$(authentication_project)		\
	edit-toolbars.gladep			\
	mailbox-properties.gladep		\
	main.gladep				\
	properties.gladep			\
	summary-popup.gladep			\
	welcome.gladep
