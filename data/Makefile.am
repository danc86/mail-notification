applicationsdir = $(datadir)/applications
schemasdir = $(sysconfdir)/gconf/schemas

dist_applications_DATA = mail-notification.desktop
dist_schemas_DATA = mail-notification.schemas

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) \
		$(GCONFTOOL) --makefile-install-rule mail-notification.schemas
else
#
# From the Automake 1.7.5 Info documentation, "Conditionals" section:
#
#	"Automake conditionals will work with any make program."
#
# Using my FreeBSD make, with --disable-schemas-install in use:
#
#	$ make install
#	[...]
#	make: don't know how to make install-data-local. Stop
#
# Hence, we need the following kludge to stay compatible.
# Thanks autofuck.
#
install-data-local:
endif
