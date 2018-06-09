GRESOURCE_TARGET = $(builddir)/vice.gresource

all: $(GRESOURCE_TARGET)

$(GRESOURCE_TARGET): $(srcdir)/vice.gresource.xml
	glib-compile-resources $< --target $@

.PHONY: clean
clean: $(GRESOURCE_TARGET)
	rm -f $<

.PHONY: distclean
distclean: clean
