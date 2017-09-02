## Makefile.am for the native GTK3 version of `libwidgets.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/joyport \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/platform \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/rs232drv \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libwidgets.a

libwidgets_a_SOURCES = \
	widgethelpers.c \
	refreshratewidget.c \
	speedwidget.c \
	kbdmappingwidget.c \
	kbdlayoutwidget.c \
	openfiledialog.c

EXTRA_DIST = \
	widgethelpers.h \
	refreshratewidget.h \
	speedwidget.h \
	kbdmappingwidget.h \
	kbdlayoutwidget.h \
	openfiledialog.h

