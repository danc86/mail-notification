figdir = figures
docname = mail-notification
lang = C
omffile = mail-notification-C.omf
entities = documentation-license.xml software-license.xml

dist_noinst_DATA = validate.stamp
MAINTAINERCLEANFILES = validate.stamp

validate.stamp: $(docname).xml $(entities)
	xmllint --noout --valid $(docname).xml
	@touch $@

include $(top_srcdir)/help/xmldocs.make

dist-hook: app-dist-hook
