if WIN32_COMPILE

# FIXME: final target of gresource is called _TARGET, but here the final target
#        is _COFF
WINDRES_RC_PROTO = $(top_srcdir)/src/arch/gtk3/data/icon.rc.proto

WINDRES_RC_TARGET = $(top_builddir)/src/arch/gtk3/data/icon.rc
WINDRES_COFF = $(top_builddir)/src/arch/gtk3/data/icon.res

$(WINDRES_RC_TARGET): $(WINDRES_RC_PROTO)
	cat $< | sed -e "s@__PREFIX__@$(top_srcdir)@g" > $@

$(WINDRES_COFF): $(WINDRES_RC_TARGET)
	$(WINDRES) $< -O coff -o $@


CLEANFILES = \
	$(WINDRES_COFF) \
	$(WINDRES_RC_TARGET)


endif
