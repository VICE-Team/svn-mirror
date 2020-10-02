if WIN32_COMPILE

# FIXME: final target of gresource is called _TARGET, but here the final target
#        is _COFF

WINDRES_RC_TARGET = $(top_srcdir)/src/arch/gtk3/data/icon.rc
WINDRES_COFF = $(top_builddir)/src/arch/gtk3/data/icon.res

$(WINDRES_COFF): $(WINDRES_RC_TARGET)
	$(WINDRES) $< -O coff -o $@ -I$(top_srcdir)/data/common


CLEANFILES = \
	$(WINDRES_COFF)

gtk3datadir = $(VICE_DATADIR)/common
gtk3data_DATA = $(WINDRES_COFF)

endif
