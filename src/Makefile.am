bin_PROGRAMS = mail-notification

if WITH_MBOX
mbox_sources = mn-mbox-mailbox.c mn-mbox-mailbox.h
endif
if WITH_MH
mh_sources = mn-mh-mailbox.c mn-mh-mailbox.h
endif
if WITH_MAILDIR
maildir_sources = mn-maildir-mailbox.c mn-maildir-mailbox.h
endif
if WITH_POP3
pop3_sources = mn-pop3-mailbox.c mn-pop3-mailbox.h
endif
if WITH_SYLPHEED
sylpheed_sources = mn-sylpheed-mailbox.c mn-sylpheed-mailbox.h
endif

mail_notification_SOURCES = \
	eggtrayicon.c			\
	eggtrayicon.h			\
	mn-conf.c			\
	mn-conf.h			\
	mn-dialog.c			\
	mn-dialog.h			\
	mn-mail-icon.c			\
	mn-mail-icon.h			\
	mn-mailbox-properties-dialog.c	\
	mn-mailbox-properties-dialog.h	\
	mn-mailbox.c			\
	mn-mailbox.h			\
	mn-mailboxes.c			\
	mn-mailboxes.h			\
	$(mbox_sources)			\
	$(mh_sources)			\
	$(maildir_sources)		\
	$(pop3_sources)			\
	$(sylpheed_sources)		\
	mn-main.c			\
	mn-pending-mailbox.c		\
	mn-pending-mailbox.h		\
	mn-preferences.c		\
	mn-preferences.h		\
	mn-stock.c			\
	mn-stock.h			\
	mn-ui.c				\
	mn-ui.h				\
	mn-unsupported-mailbox.c	\
	mn-unsupported-mailbox.h	\
	mn-uri.c			\
	mn-uri.h			\
	mn-util.c			\
	mn-util.h			\
	mn-vfs.c			\
	mn-vfs.h

mail_notification_LDFLAGS = $(INTLLIBS) $(GNOME_LIBS) $(GNET_LIBS)

gnomelocaledir = $(datadir)/locale

AM_CPPFLAGS = $(WARN_CFLAGS) $(G_ASSERTIONS) $(GNOME_CFLAGS) $(GNET_CFLAGS) \
	-I$(top_srcdir) \
	-DPREFIX="\"$(prefix)\"" \
	-DSYSCONFDIR="\"$(sysconfdir)\"" \
	-DDATADIR="\"$(datadir)\"" \
	-DLIBDIR="\"$(libdir)\"" \
	-DGNOMELOCALEDIR="\"$(gnomelocaledir)\"" \
	-DUIDIR="\"$(pkgdatadir)/ui\""
