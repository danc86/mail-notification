iconsdir = $(datadir)/icons/hicolor
nobase_dist_icons_DATA = \
	16x16/apps/mail-notification.png \
	22x22/apps/mail-notification.png \
	24x24/apps/mail-notification.png \
	32x32/apps/mail-notification.png \
	48x48/apps/mail-notification.png \
	scalable/apps/mail-notification.svg

dist_pkgdata_DATA = logo.png

if WITH_GMAIL
dist_pkgdata_DATA += gmail.png
endif

if WITH_YAHOO
dist_pkgdata_DATA += yahoo.png
endif

if WITH_HOTMAIL
dist_pkgdata_DATA += hotmail.png
endif

install-data-hook: update-icon-cache

uninstall-hook: update-icon-cache

update-icon-cache:
	-gtk-update-icon-cache -f -t $(iconsdir)
