uidir = $(pkgdatadir)/ui

if WITH_POP3_OR_IMAP_OR_GMAIL
authentication_interfaces = authentication.glade
authentication_projects = authentication.gladep
endif

interfaces =					\
	$(authentication_interfaces)		\
	mailbox-properties.glade		\
	properties.glade			\
	summary-dialog.glade			\
	summary-popup.glade			\
	welcome.glade

projects =					\
	$(authentication_projects)		\
	mailbox-properties.gladep		\
	properties.gladep			\
	summary-dialog.gladep			\
	summary-popup.gladep			\
	welcome.gladep

dist_ui_DATA = $(interfaces)

EXTRA_DIST = $(projects)
