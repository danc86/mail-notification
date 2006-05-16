if WITH_EVOLUTION
evolution_server_in = GNOME_MailNotification_Evolution.server.in
evolution_server = GNOME_MailNotification_Evolution.server
endif

# gnome_cappletdir is defined by configure
gnome_capplet_in_files = mail-notification-properties.desktop.in
gnome_capplet_DATA = mail-notification-properties.desktop

autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = mail-notification.desktop.in
autostart_DATA = mail-notification.desktop

serverdir = $(libdir)/bonobo/servers
server_in_files = $(evolution_server_in) GNOME_MailNotification.server.in
server_DATA = $(evolution_server) GNOME_MailNotification.server

schemasdir = $(GCONF_SCHEMA_FILE_DIR)
schemas_in_files = mail-notification.schemas.in
schemas_DATA = mail-notification.schemas

@INTLTOOL_DESKTOP_RULE@
@INTLTOOL_SERVER_RULE@
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		$(GCONFTOOL) --makefile-install-rule mail-notification.schemas
endif

EXTRA_DIST = \
	GNOME_MailNotification.server.in.in		\
	GNOME_MailNotification_Evolution.server.in	\
	mail-notification-properties.desktop.in		\
	mail-notification.desktop.in			\
	mail-notification.schemas.in

CLEANFILES = \
	$(evolution_server)			\
	GNOME_MailNotification.server		\
	GNOME_MailNotification.server.in	\
	mail-notification-properties.desktop	\
	mail-notification.desktop		\
	mail-notification.schemas

GNOME_MailNotification.server.in: GNOME_MailNotification.server.in.in
	sed -e 's|\@bindir\@|$(bindir)|' GNOME_MailNotification.server.in.in > GNOME_MailNotification.server.in
