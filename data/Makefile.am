applicationsdir = $(datadir)/applications
applications_in_files = mail-notification.desktop.in
applications_DATA = mail-notification.desktop

schemasdir = $(sysconfdir)/gconf/schemas
schemas_in_files = mail-notification.schemas.in
schemas_DATA = mail-notification.schemas

@INTLTOOL_DESKTOP_RULE@
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		$(GCONFTOOL) --makefile-install-rule mail-notification.schemas
endif

EXTRA_DIST = mail-notification.desktop.in mail-notification.schemas.in
CLEANFILES = mail-notification.desktop mail-notification.schemas
