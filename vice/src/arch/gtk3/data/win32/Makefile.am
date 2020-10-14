if WIN32_COMPILE

WINDRES_ICON = icon.res

$(WINDRES_ICON): icon.rc
	$(WINDRES) $< -O coff -o $@ -I$(top_srcdir)/data/common


CLEANFILES = \
	$(WINDRES_ICON)

EXTRA_DIST = \
	icon.rc

gtk3datadir = $(VICE_DATADIR)/common
gtk3data_DATA = $(WINDRES_ICON)

endif
