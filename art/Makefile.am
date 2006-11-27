iconsdir = $(datadir)/icons/hicolor
nobase_dist_icons_DATA = \
	16x16/apps/mail-notification.png \
	22x22/apps/mail-notification.png \
	24x24/apps/mail-notification.png \
	32x32/apps/mail-notification.png \
	48x48/apps/mail-notification.png \
	scalable/apps/mail-notification.svg

app_pixmapsdir = $(pkgdatadir)/ui
dist_app_pixmaps_DATA = logo.png

if WITH_GMAIL
dist_app_pixmaps_DATA += gmail.png
endif

install-data-hook:
	-gtk-update-icon-cache -f -t $(iconsdir)
