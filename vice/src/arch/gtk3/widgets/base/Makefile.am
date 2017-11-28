## Makefile.am for the native GTK3 version of `libwidgetsbase.a'.


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
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c64dtv \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/scpu64 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/userport \
	-I$(top_srcdir)/src/c64/cart

noinst_LIBRARIES = libbasewidgets.a

libbasewidgets_a_SOURCES = \
	basedialogs.c \
	widgethelpers.c \
	resourcehelpers.c \
	resourcecheckbutton.c \
	resourcecombobox.c \
	resourceentry.c \
	resourceradiogroup.c \
	resourcescale.c \
	resourcespinbutton.c \
	cartimagewidget.c \
	openfiledialog.c \
	savefiledialog.c \
	selectdirectorydialog.c \
	filechooserhelpers.c \
	clockportdevicewidget.c \
	carthelpers.c

EXTRA_DIST = \
	basedialogs.h \
	widgethelpers.h \
	basewidgets.h \
	basewidget_types.h \
	resourcehelpers.h \
	resourcecheckbutton.h \
	resourcecombobox.h \
	resourceentry.h \
	resourceradiogroup.h \
	resourcescale.h \
	resourcespinbutton.h \
	cartimagewidget.h \
	openfiledialog.h \
	savefiledialog.h \
	selectdirectorydialog.h \
	filechooserhelpers.h \
	clockportdevicewidget.h \
	carthelpers.h
