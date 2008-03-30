SUBDIRS = widgets

INCLUDES = \
	-DPREFIX=\"@prefix@\" \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/true1541 \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet

noinst_LIBRARIES = libxawui.a

libxawui_a_SOURCES = \
	c64icon.xpm \
	c64ui.c \
	c64ui.h \
	checkmark.xbm \
	peticon.xpm \
	petui.c \
	petui.h \
	right_arrow.xbm \
	ui.c \
	ui.h \
	uicommands.c \
	uicommands.h \
	uimenu.c \
	uimenu.h \
	uisettings.c \
	uisettings.h \
	vic20icon.xpm \
	vic20ui.c \
	vic20ui.h \
	xdebug.c \
	xdebug.h

