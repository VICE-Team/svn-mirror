SUBDIRS = widgets

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/arch/unix/x11

noinst_LIBRARIES = libxawui.a

libxawui_a_SOURCES = \
	checkmark.xbm \
	right_arrow.xbm \
	ui.c \
	uiarch.h \
	uiedisk.c \
	uihotkey.c \
	uihotkey.h \
	uimenu.c \
	uimenu.h \
	uisnapshot.c \
	videoarch.h \
	x11video.c \
	x11kbd.c \
	about.c
