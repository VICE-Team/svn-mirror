# Main makefile to generate per-OS icons, launcher etc.
#
# The per-OS files are in subdirs and the things needed for every OS are
# handled here.

SUBDIRS = macos unix win32

# Generate gresource binary file, required by all ports
GRESOURCE_TARGET = $(builddir)/vice.gresource
GRESOURCE_XML = $(srcdir)/vice.gresource.xml

$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	glib-compile-resources --sourcedir $(top_srcdir) $< --target $@

gtk3datadir = $(VICE_DATADIR)/common
gtk3data_DATA = $(GRESOURCE_TARGET)


CLEANFILES = \
	$(GRESOURCE_TARGET)

EXTRA_DIST = \
	$(GRESOURCE_XML) \
	icon.rc


# vim: set noet ts=8 sts=8 sw=8:
