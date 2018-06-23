gtk3datadir = $(VICEDIR)/gui

GRESOURCE_TARGET = $(builddir)/vice.gresource
GRESOURCE_PROTO = $(srcdir)//vice.gresource.xml.proto
GRESOURCE_XML = $(builddir)/vice.gresource.xml

all: $(GRESOURCE_TARGET)

$(GRESOURCE_XML): $(GRESOURCE_PROTO)
	echo "srcdir = $(srcdir)"
	echo "buildir = $(builddir)"
	cat $(GRESOURCE_PROTO) | sed -e "s@VICE_SRC_DIR@$(srcdir)@g" \
		> $(GRESOURCE_XML)

$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	echo "srcdir = $(srcdir)"
	echo "buildir = $(builddir)"
	glib-compile-resources $< --target $@

gtk3data_DATA = $(GRESOURCE_TARGET)

.PHONY: clean
clean:
	rm -f $(GRESOURCE_TARGET)
	rm -f $(GRESOURCE_XML)

.PHONY: distclean
distclean: clean
