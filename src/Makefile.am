bin_PROGRAMS = mail-notification

if WITH_MBOX
mbox_gob_sources = mn-mbox-mailbox-backend.gob
mbox_gob_built_sources = \
	mn-mbox-mailbox-backend-private.h	\
	mn-mbox-mailbox-backend.c		\
	mn-mbox-mailbox-backend.h
endif

if WITH_MH
mh_gob_sources = mn-mh-mailbox-backend.gob
mh_gob_built_sources = \
	mn-mh-mailbox-backend-private.h		\
	mn-mh-mailbox-backend.c			\
	mn-mh-mailbox-backend.h
endif

if WITH_MAILDIR
maildir_gob_sources = mn-maildir-mailbox-backend.gob
maildir_gob_built_sources = \
	mn-maildir-mailbox-backend-private.h	\
	mn-maildir-mailbox-backend.c		\
	mn-maildir-mailbox-backend.h
endif

if WITH_POP3
pop3_gob_sources = \
	mn-pop3-mailbox-properties.gob	\
	mn-pop3-mailbox.gob
pop3_gob_built_sources = \
	mn-pop3-mailbox-properties-private.h	\
	mn-pop3-mailbox-properties.c		\
	mn-pop3-mailbox-properties.h		\
	mn-pop3-mailbox-private.h		\
	mn-pop3-mailbox.c			\
	mn-pop3-mailbox.h
md5_sources = mn-md5.c mn-md5.h
endif

if WITH_IMAP
imap_gob_sources = \
	mn-imap-mailbox-properties.gob	\
	mn-imap-mailbox.gob
imap_gob_built_sources = \
	mn-imap-mailbox-properties-private.h	\
	mn-imap-mailbox-properties.c		\
	mn-imap-mailbox-properties.h		\
	mn-imap-mailbox-private.h		\
	mn-imap-mailbox.c			\
	mn-imap-mailbox.h
endif

if WITH_POP3_OR_IMAP
client_session_sources = mn-client-session.c mn-client-session.h
pi_gob_sources = \
	mn-auth-combo-box.gob			\
	mn-pi-mailbox-properties.gob		\
	mn-pi-mailbox.gob
pi_gob_built_sources = \
	mn-auth-combo-box-private.h			\
	mn-auth-combo-box.c				\
	mn-auth-combo-box.h				\
	mn-pi-mailbox-properties-private.h		\
	mn-pi-mailbox-properties.c			\
	mn-pi-mailbox-properties.h			\
	mn-pi-mailbox-private.h				\
	mn-pi-mailbox.c					\
	mn-pi-mailbox.h
endif

if WITH_POP3_OR_IMAP_OR_GMAIL
authenticated_mailbox_gob_sources = \
	mn-authenticated-mailbox-properties.gob	\
	mn-authenticated-mailbox.gob
authenticated_mailbox_gob_built_sources = \
	mn-authenticated-mailbox-properties-private.h	\
	mn-authenticated-mailbox-properties.c		\
	mn-authenticated-mailbox-properties.h		\
	mn-authenticated-mailbox-private.h		\
	mn-authenticated-mailbox.c			\
	mn-authenticated-mailbox.h
endif

if WITH_SYLPHEED
sylpheed_gob_sources = mn-sylpheed-mailbox-backend.gob
sylpheed_gob_built_sources = \
	mn-sylpheed-mailbox-backend-private.h	\
	mn-sylpheed-mailbox-backend.c		\
	mn-sylpheed-mailbox-backend.h
endif

if WITH_GMAIL
gmail_gob_sources = \
	mn-gmail-mailbox-properties.gob	\
	mn-gmail-mailbox.gob
gmail_gob_built_sources = \
	mn-gmail-mailbox-properties-private.h	\
	mn-gmail-mailbox-properties.c		\
	mn-gmail-mailbox-properties.h		\
	mn-gmail-mailbox-private.h		\
	mn-gmail-mailbox.c			\
	mn-gmail-mailbox.h
endif

if WITH_MBOX_OR_MH_OR_MAILDIR_OR_SYLPHEED
vfs_mailbox_gob_sources = \
	mn-custom-vfs-mailbox.gob		\
	mn-system-vfs-mailbox-properties.gob	\
	mn-system-vfs-mailbox.gob		\
	mn-vfs-mailbox-backend.gob 		\
	mn-vfs-mailbox.gob
vfs_mailbox_gob_built_sources = \
	mn-custom-vfs-mailbox-private.h			\
	mn-custom-vfs-mailbox.c				\
	mn-custom-vfs-mailbox.h				\
	mn-system-vfs-mailbox-properties-private.h	\
	mn-system-vfs-mailbox-properties.c		\
	mn-system-vfs-mailbox-properties.h		\
	mn-system-vfs-mailbox-private.h			\
	mn-system-vfs-mailbox.c				\
	mn-system-vfs-mailbox.h				\
	mn-vfs-mailbox-backend-private.h		\
	mn-vfs-mailbox-backend.c			\
	mn-vfs-mailbox-backend.h			\
	mn-vfs-mailbox-private.h			\
	mn-vfs-mailbox.c				\
	mn-vfs-mailbox.h
endif

if WITH_MBOX_OR_MH_OR_MAILDIR_OR_SYLPHEED_OR_EVOLUTION
reentrant_mailbox_gob_sources = mn-reentrant-mailbox.gob
reentrant_mailbox_gob_built_sources = \
	mn-reentrant-mailbox-private.h	\
	mn-reentrant-mailbox.c		\
	mn-reentrant-mailbox.h
endif

if WITH_MIME
mime_gob_sources = \
	mn-gmime-stream-vfs.gob
mime_gob_built_sources = \
	mn-gmime-stream-vfs-private.h	\
	mn-gmime-stream-vfs.c		\
	mn-gmime-stream-vfs.h
mime_sources = \
	mn-message-mime.c	\
	mn-message-mime.h
endif

if WITH_SSL
ssl_sources = mn-ssl.c mn-ssl.h
endif

if WITH_SASL
sasl_sources = mn-sasl.c mn-sasl.h
endif

if WITH_EVOLUTION
evolution_plugin_eplug = org-gnome-mail-notification.eplug

evolution_shared_sources = mn-evolution.h

evolution_shared_idl_sources = GNOME_MailNotification_Evolution.idl
evolution_shared_idl_built_sources = \
	GNOME_MailNotification_Evolution-common.c	\
	GNOME_MailNotification_Evolution-stubs.c	\
	GNOME_MailNotification_Evolution-skels.c	\
	GNOME_MailNotification_Evolution.h

evolution_plugin_gob_sources = mn-evolution-glue.gob
evolution_plugin_gob_built_sources = \
	mn-evolution-glue-private.h	\
	mn-evolution-glue.c		\
	mn-evolution-glue.h

evolution_gob_sources = \
	mn-corba-object.gob			\
	mn-evolution-mailbox-properties.gob	\
	mn-evolution-mailbox.gob
evolution_gob_built_sources = \
	mn-corba-object-private.h			\
	mn-corba-object.c				\
	mn-corba-object.h				\
	mn-evolution-mailbox-properties-private.h	\
	mn-evolution-mailbox-properties.c		\
	mn-evolution-mailbox-properties.h		\
	mn-evolution-mailbox-private.h			\
	mn-evolution-mailbox.c				\
	mn-evolution-mailbox.h

evolution_plugin_DATA = $(evolution_plugin_eplug)
evolution_plugin_LTLIBRARIES = liborg-gnome-mail-notification.la

nodist_liborg_gnome_mail_notification_la_SOURCES = \
	$(evolution_shared_idl_built_sources)
liborg_gnome_mail_notification_la_SOURCES = \
	$(evolution_shared_sources)		\
	$(evolution_shared_idl_sources)		\
	$(evolution_plugin_gob_sources)		\
	$(evolution_plugin_gob_built_sources)	\
	mn-evolution-folder-tree-control.c	\
	mn-evolution-folder-tree-control.h	\
	mn-evolution-plugin.c

liborg_gnome_mail_notification_la_CPPFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN="\"$(PACKAGE)\"" \
	$(EVOLUTION_PLUGIN_CFLAGS)
liborg_gnome_mail_notification_la_LDFLAGS = -avoid-version -module $(EVOLUTION_PLUGIN_LIBS)
endif

idl_sources = $(evolution_shared_idl_sources) GNOME_MailNotification.idl
idl_built_sources = \
	$(evolution_shared_idl_built_sources)	\
	GNOME_MailNotification-common.c		\
	GNOME_MailNotification-stubs.c		\
	GNOME_MailNotification-skels.c		\
	GNOME_MailNotification.h

gob_sources = \
	$(mbox_gob_sources)			\
	$(mh_gob_sources)			\
	$(maildir_gob_sources)			\
	$(pop3_gob_sources)			\
	$(imap_gob_sources)			\
	$(authenticated_mailbox_gob_sources)	\
	$(pi_gob_sources)			\
	$(sylpheed_gob_sources)			\
	$(gmail_gob_sources)			\
	$(evolution_gob_sources)		\
	$(vfs_mailbox_gob_sources)		\
	$(reentrant_mailbox_gob_sources)	\
	$(mime_gob_sources)			\
	mn-about-dialog.gob			\
	mn-autodetect-mailbox-properties.gob	\
	mn-automation.gob			\
	mn-blinking-image.gob			\
	mn-dialog.gob				\
	mn-mail-icon.gob			\
	mn-mail-summary-popup.gob		\
	mn-mailbox-properties-dialog.gob	\
	mn-mailbox-properties.gob		\
	mn-mailbox-view.gob			\
	mn-mailbox.gob				\
	mn-mailboxes.gob			\
	mn-main-window.gob			\
	mn-message-view.gob			\
	mn-message.gob				\
	mn-properties-dialog.gob		\
	mn-shell.gob				\
	mn-tooltips.gob
gob_built_sources = \
	$(mbox_gob_built_sources)			\
	$(mh_gob_built_sources)				\
	$(maildir_gob_built_sources)			\
	$(pop3_gob_built_sources)			\
	$(imap_gob_built_sources)			\
	$(authenticated_mailbox_gob_built_sources)	\
	$(pi_gob_built_sources)				\
	$(sylpheed_gob_built_sources)			\
	$(gmail_gob_built_sources)			\
	$(evolution_gob_built_sources)			\
	$(vfs_mailbox_gob_built_sources)		\
	$(reentrant_mailbox_gob_built_sources)		\
	$(mime_gob_built_sources)			\
	mn-about-dialog-private.h			\
	mn-about-dialog.c				\
	mn-about-dialog.h				\
	mn-autodetect-mailbox-properties-private.h	\
	mn-autodetect-mailbox-properties.c		\
	mn-autodetect-mailbox-properties.h		\
	mn-automation-private.h				\
	mn-automation.c					\
	mn-automation.h					\
	mn-blinking-image-private.h			\
	mn-blinking-image.c				\
	mn-blinking-image.h				\
	mn-dialog-private.h				\
	mn-dialog.c					\
	mn-dialog.h					\
	mn-mail-icon-private.h				\
	mn-mail-icon.c					\
	mn-mail-icon.h					\
	mn-mail-summary-popup-private.h			\
	mn-mail-summary-popup.c				\
	mn-mail-summary-popup.h				\
	mn-mailbox-properties-dialog-private.h		\
	mn-mailbox-properties-dialog.c			\
	mn-mailbox-properties-dialog.h			\
	mn-mailbox-properties-private.h			\
	mn-mailbox-properties.c				\
	mn-mailbox-properties.h				\
	mn-mailbox-view-private.h			\
	mn-mailbox-view.c				\
	mn-mailbox-view.h				\
	mn-mailbox-private.h				\
	mn-mailbox.c					\
	mn-mailbox.h					\
	mn-mailboxes-private.h				\
	mn-mailboxes.c					\
	mn-mailboxes.h					\
	mn-main-window-private.h			\
	mn-main-window.c				\
	mn-main-window.h				\
	mn-message-view-private.h			\
	mn-message-view.c				\
	mn-message-view.h				\
	mn-message-private.h				\
	mn-message.c					\
	mn-message.h					\
	mn-properties-dialog-private.h			\
	mn-properties-dialog.c				\
	mn-properties-dialog.h				\
	mn-shell-private.h				\
	mn-shell.c					\
	mn-shell.h					\
	mn-tooltips-private.h				\
	mn-tooltips.c					\
	mn-tooltips.h

egg_built_sources = \
	eggmarshalers.c	\
	eggmarshalers.h

nodist_mail_notification_SOURCES = $(idl_built_sources)
mail_notification_SOURCES = \
	$(idl_sources)				\
	$(gob_sources)				\
	$(gob_built_sources)			\
	$(egg_built_sources)			\
	$(md5_sources)				\
	$(client_session_sources)		\
	$(ssl_sources)				\
	$(sasl_sources)				\
	$(mime_sources)				\
	$(evolution_shared_sources)		\
	egg-editable-toolbar.c			\
	egg-editable-toolbar.h			\
	egg-toolbar-editor.c			\
	egg-toolbar-editor.h			\
	egg-toolbars-model.c			\
	egg-toolbars-model.h			\
	eggtrayicon.c				\
	eggtrayicon.h				\
	mn-conf.c				\
	mn-conf.h				\
	mn-main.c				\
	mn-stock.c				\
	mn-stock.h				\
	mn-util.c				\
	mn-util.h				\
	mn-vfs.c				\
	mn-vfs.h				\
	nautilus-cell-renderer-pixbuf-emblem.c	\
	nautilus-cell-renderer-pixbuf-emblem.h
BUILT_SOURCES = \
	$(evolution_plugin_gob_built_sources)	\
	$(idl_built_sources)			\
	$(gob_built_sources)			\
	$(egg_built_sources)

mail_notification_CPPFLAGS = $(WARN_CFLAGS) $(GNOME_CFLAGS) $(GMIME_CFLAGS) $(OPENSSL_CFLAGS) $(SASL_CFLAGS) \
	-I$(top_srcdir) \
	-DPREFIX="\"$(prefix)\"" \
	-DSYSCONFDIR="\"$(sysconfdir)\"" \
	-DDATADIR="\"$(datadir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DGNOMELOCALEDIR="\"$(datadir)/locale\"" \
	-DGNOMEPIXMAPSDIR="\"$(datadir)/pixmaps\"" \
	-DUIDIR="\"$(pkgdatadir)/ui\"" \
	-DG_LOG_DOMAIN="\"$(PACKAGE)\""
mail_notification_LDFLAGS = $(INTLLIBS) $(GNOME_LIBS) $(GMIME_LIBS) $(OPENSSL_LIBS) $(SASL_LIBS)

CLEANFILES = $(evolution_plugin_eplug) $(idl_built_sources)
MAINTAINERCLEANFILES = $(evolution_plugin_gob_built_sources) $(gob_built_sources) $(egg_built_sources)

EXTRA_DIST = eggmarshalers.list org-gnome-mail-notification.eplug.in

%.c %.h %-private.h: %.gob
	$(GOB2) --always-private-header --exit-on-warn $<

%-common.c %-stubs.c %-skels.c %.h: %.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -I$(LIBBONOBO_IDL) -I$(BONOBO_ACTIVATION_IDL) $<

eggmarshalers.h: eggmarshalers.list
	cd $(srcdir) \
	&& $(GLIB_GENMARSHAL) --prefix=_egg_marshal eggmarshalers.list --header > xgen-emh \
	&& cp xgen-emh eggmarshalers.h \
	&& rm -f xgen-emh xgen-emh~

eggmarshalers.c: eggmarshalers.list
	cd $(srcdir) \
	&& $(GLIB_GENMARSHAL) --prefix=_egg_marshal eggmarshalers.list --body > xgen-emc \
	&& cp xgen-emc eggmarshalers.c \
	&& rm -f xgen-emc xgen-emc~

%.eplug: %.eplug.in
	sed -e 's|\@PLUGINDIR\@|$(evolution_plugindir)|' $< > $@
