SUBDIRS = widgets

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/arch/unix/x11

noinst_LIBRARIES = libxawui.a

libxawui_a_SOURCES = \
	c128icon.xpm \
	c610icon.xpm \
	c64icon.xpm \
	checkmark.xbm \
	peticon.xpm \
	right_arrow.xbm \
	ui.c \
	ui.h \
	uiedisk.c \
	uihotkey.c \
	uihotkey.h \
	uimenu.c \
	uimenu.h \
	uisnapshot.c \
	vic20icon.xpm \
	vsidui.c \
	videoarch.h \
	x11video.c \
	x11kbd.c \
	about.c
