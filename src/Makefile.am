bin_PROGRAMS = mail-notification

if WITH_MBOX
mbox_gob_sources = mn-mbox-mailbox.gob
mbox_gob_built_sources = \
	mn-mbox-mailbox-private.h	\
	mn-mbox-mailbox.c		\
	mn-mbox-mailbox.h
endif

if WITH_MH
mh_gob_sources = mn-mh-mailbox.gob
mh_gob_built_sources = \
	mn-mh-mailbox-private.h		\
	mn-mh-mailbox.c			\
	mn-mh-mailbox.h
endif

if WITH_MAILDIR
maildir_gob_sources = mn-maildir-mailbox.gob
maildir_gob_built_sources = \
	mn-maildir-mailbox-private.h	\
	mn-maildir-mailbox.c		\
	mn-maildir-mailbox.h
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
	mn-pi-mailbox-properties.gob
pi_gob_built_sources = \
	mn-auth-combo-box-private.h		\
	mn-auth-combo-box.c			\
	mn-auth-combo-box.h			\
	mn-pi-mailbox-properties-private.h	\
	mn-pi-mailbox-properties.c		\
	mn-pi-mailbox-properties.h
endif

if WITH_POP3_OR_IMAP_OR_GMAIL
authenticated_mailbox_gob_sources = \
	mn-authenticated-mailbox.gob
authenticated_mailbox_gob_built_sources = \
	mn-authenticated-mailbox-private.h	\
	mn-authenticated-mailbox.c		\
	mn-authenticated-mailbox.h
endif

if WITH_SYLPHEED
sylpheed_gob_sources = mn-sylpheed-mailbox.gob
sylpheed_gob_built_sources = \
	mn-sylpheed-mailbox-private.h	\
	mn-sylpheed-mailbox.c		\
	mn-sylpheed-mailbox.h
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
gmail_sources = \
	mn-sgml-ref.c	\
	mn-sgml-ref.h	\
	mn-soup.c	\
	mn-soup.h
gmail_built_sources = \
	mn-sgml-entities.h

if WITH_GMAIL_TIMESTAMPS
icu_sources = mn-icu.c mn-icu.h
endif
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

idl_sources = MNAutomation.idl
idl_built_sources = \
	MNAutomation-common.c			\
	MNAutomation-stubs.c			\
	MNAutomation-skels.c			\
	MNAutomation.h

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
	$(mime_gob_sources)			\
	mn-about-dialog.gob			\
	mn-autodetect-mailbox-properties.gob	\
	mn-automation.gob			\
	mn-blinking-image.gob			\
	mn-mail-icon.gob			\
	mn-mailbox-view.gob			\
	mn-mailbox.gob				\
	mn-mailboxes.gob			\
	mn-message-view.gob			\
	mn-message.gob				\
	mn-pending-mailbox.gob			\
	mn-shell.gob				\
	mn-system-mailbox-properties.gob	\
	mn-tooltips.gob				\
	mn-unsupported-mailbox.gob		\
	mn-uri.gob
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
	mn-mail-icon-private.h				\
	mn-mail-icon.c					\
	mn-mail-icon.h					\
	mn-mailbox-view-private.h			\
	mn-mailbox-view.c				\
	mn-mailbox-view.h				\
	mn-mailbox-private.h				\
	mn-mailbox.c					\
	mn-mailbox.h					\
	mn-mailboxes-private.h				\
	mn-mailboxes.c					\
	mn-mailboxes.h					\
	mn-message-view-private.h			\
	mn-message-view.c				\
	mn-message-view.h				\
	mn-message-private.h				\
	mn-message.c					\
	mn-message.h					\
	mn-pending-mailbox-private.h			\
	mn-pending-mailbox.c				\
	mn-pending-mailbox.h				\
	mn-shell-private.h				\
	mn-shell.c					\
	mn-shell.h					\
	mn-system-mailbox-properties-private.h		\
	mn-system-mailbox-properties.c			\
	mn-system-mailbox-properties.h			\
	mn-tooltips-private.h				\
	mn-tooltips.c					\
	mn-tooltips.h					\
	mn-unsupported-mailbox-private.h		\
	mn-unsupported-mailbox.c			\
	mn-unsupported-mailbox.h			\
	mn-uri-private.h				\
	mn-uri.c					\
	mn-uri.h

egg_built_sources = \
	eggmarshalers.c	\
	eggmarshalers.h

nodist_mail_notification_SOURCES = $(idl_built_sources)
mail_notification_SOURCES = \
	$(idl_sources)				\
	$(gob_sources)				\
	$(gob_built_sources)			\
	$(egg_built_sources)			\
	$(gmail_sources)			\
	$(gmail_built_sources)			\
	$(md5_sources)				\
	$(client_session_sources)		\
	$(ssl_sources)				\
	$(sasl_sources)				\
	$(mime_sources)				\
	$(icu_sources)				\
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
	mn-mailbox-properties-dialog.c		\
	mn-mailbox-properties-dialog.h		\
	mn-mailbox-properties-util.c		\
	mn-mailbox-properties-util.h		\
	mn-mailbox-properties.c			\
	mn-mailbox-properties.h			\
	mn-main-window.c			\
	mn-main-window.h			\
	mn-main.c				\
	mn-properties-dialog.c			\
	mn-properties-dialog.h			\
	mn-stock.c				\
	mn-stock.h				\
	mn-summary-popup.c			\
	mn-summary-popup.h			\
	mn-util.c				\
	mn-util.h				\
	mn-vfs.c				\
	mn-vfs.h
BUILT_SOURCES = \
	$(idl_built_sources)			\
	$(gob_built_sources)			\
	$(egg_built_sources)			\
	$(gmail_built_sources)

AM_CPPFLAGS = $(WARN_CFLAGS) $(GNOME_CFLAGS) $(GMIME_CFLAGS) $(SOUP_CFLAGS) $(OPENSSL_CFLAGS) $(SASL_CFLAGS) $(ICU_CFLAGS) \
	-I$(top_srcdir) \
	-DPREFIX="\"$(prefix)\"" \
	-DSYSCONFDIR="\"$(sysconfdir)\"" \
	-DDATADIR="\"$(datadir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DGNOMELOCALEDIR="\"$(datadir)/locale\"" \
	-DGNOMEPIXMAPSDIR="\"$(datadir)/pixmaps\"" \
	-DUIDIR="\"$(pkgdatadir)/ui\"" \
	-DG_LOG_DOMAIN="\"$(PACKAGE)\""
AM_LDFLAGS = $(INTLLIBS) $(GNOME_LIBS) $(GMIME_LIBS) $(SOUP_LIBS) $(OPENSSL_LIBS) $(SASL_LIBS) $(ICU_LIBS)

CLEANFILES = $(idl_built_sources)
MAINTAINERCLEANFILES = $(gob_built_sources) $(egg_built_sources) $(gmail_built_sources)

entity_sets = xhtml-lat1.ent xhtml-special.ent xhtml-symbol.ent
EXTRA_DIST = ent2h.sh $(entity_sets) eggmarshalers.list

%.c %.h %-private.h: %.gob
	$(GOB2) --always-private-header --exit-on-warn $<
%-common.c %-stubs.c %-skels.c %.h: %.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -I$(LIBBONOBO_IDL) -I$(BONOBO_ACTIVATION_IDL) $<

mn-sgml-entities.h: ent2h.sh $(entity_sets)
	./ent2h.sh $(entity_sets) > $@

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
