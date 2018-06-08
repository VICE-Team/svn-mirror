all: $(builddir)/vice.gresource

vice.gresource: $(srcdir)/vice.gresource.xml
	glib-compile-resources $< --target $@

