applicationsdir = $(datadir)/applications
applications_in_files = mail-notification.desktop.in
applications_DATA = mail-notification.desktop

settingsdir = $(datadir)/control-center-2.0/capplets
settings_in_files = mail-notification-properties.desktop.in
settings_DATA = mail-notification-properties.desktop

serverdir = $(libdir)/bonobo/servers
server_in_files = GNOME_MailNotification_Automation.server.in
server_DATA = GNOME_MailNotification_Automation.server

schemasdir = $(GCONF_SCHEMA_FILE_DIR)
schemas_in_files = mail-notification.schemas.in
schemas_DATA = mail-notification.schemas

soundlistdir = $(sysconfdir)/sound/events
soundlist_in_files = mail-notification.soundlist.in
soundlist_DATA = mail-notification.soundlist

@INTLTOOL_DESKTOP_RULE@
@INTLTOOL_SERVER_RULE@
@INTLTOOL_SCHEMAS_RULE@
@INTLTOOL_SOUNDLIST_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		$(GCONFTOOL) --makefile-install-rule mail-notification.schemas
endif

EXTRA_DIST = \
	mail-notification.desktop.in			\
	mail-notification-properties.desktop.in		\
	GNOME_MailNotification_Automation.server.in.in	\
	mail-notification.schemas.in			\
	mail-notification.soundlist.in

CLEANFILES = \
	mail-notification.desktop			\
	mail-notification-properties.desktop		\
	GNOME_MailNotification_Automation.server	\
	GNOME_MailNotification_Automation.server.in	\
	mail-notification.schemas			\
	mail-notification.soundlist

GNOME_MailNotification_Automation.server.in: GNOME_MailNotification_Automation.server.in.in
	sed -e 's|\@bindir\@|$(bindir)|' GNOME_MailNotification_Automation.server.in.in > GNOME_MailNotification_Automation.server.in
