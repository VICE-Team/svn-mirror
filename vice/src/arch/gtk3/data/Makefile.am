gtk3datadir = $(VICEDIR)/common

GRESOURCE_TARGET = $(builddir)/vice.gresource
# TODO: check the double backslash
GRESOURCE_PROTO = $(srcdir)//vice.gresource.xml.proto
GRESOURCE_XML = $(builddir)/vice.gresource.xml

# FIXME: final target of gresource is called _TARGET, but here the final target
#        is _COFF
WINDRES_RC_PROTO = $(top_srcdir)/src/arch/gtk3/data/icon.rc.proto
WINDRES_RC_TARGET = $(top_builddir)/src/arch/gtk3/data/icon.rc
WINDRES_COFF = $(top_builddir)/src/arch/gtk3/data/icon.res

if WIN32_COMPILE
all: $(GRESOURCE_TARGET) $(WINDRES_COFF)
else
all: $(GRESOURCE_TARGET)
endif

$(GRESOURCE_XML): $(GRESOURCE_PROTO)
	cat $(GRESOURCE_PROTO) | sed -e "s@VICE_SRC_DIR@$(srcdir)@g" \
		> $(GRESOURCE_XML)

$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	glib-compile-resources $< --target $@

if WIN32_COMPILE
$(WINDRES_RC_TARGET): $(WINDRES_RC_PROTO)
	cat $< | sed -e "s@__PREFIX__@$(top_srcdir)@g" > $@

$(WINDRES_COFF): $(WINDRES_RC_TARGET)
	$(WINDRES) $< -O coff -o $@
endif

EXTRA_DIST = $(GRESOURCE_PROTO) $(WINDRES_RC_PROTO)


# TODO: figure out what the hell this does
gtk3data_DATA = $(GRESOURCE_TARGET) $(WINDRES_COFF)

.PHONY: clean
clean:
	rm -f $(GRESOURCE_TARGET)
	rm -f $(GRESOURCE_XML)
	rm -f $(WINDRES_COFF)
	rm -f $(WINDRES_RC_TARGET)

.PHONY: distclean
distclean: clean

# vim: set noet ts=8 sts=8 sw=8:
