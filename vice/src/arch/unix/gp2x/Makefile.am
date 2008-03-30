# Makefile.am for the GP2X version of `libarchgp2x.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarchgp2x.a

libarchgp2x_a_SOURCES = \
	Char_ROM.h \
	audio_gp2x.c \
	audio_gp2x.h \
	c128ui.c \
	c64ui.c \
	cbm2ui.c \
	fullscreenarch.h \
	gp2xui.h \
	input_gp2x.c \
	input_gp2x.h \
	minimal.c \
	minimal.h \
	minimal_940t.h \
	petui.c \
	plus4ui.c \
	prefs_gp2x.c \
	prefs_gp2x.h \
	ui.c \
	ui_gp2x.c \
	ui_gp2x.h \
	uiarch.h \
	uicmdline.c \
	uimenu.h \
	uimon.c \
	uisound.h \
	uitext_gp2x.c \
	uitext_gp2x.h \
	vic20ui.c \
	video.c \
	videoarch.h \
	vkeyboard_gp2x.c \
	vkeyboard_gp2x.h \
	vsidui.c

EXTRA_DIST = \
	gpe-files/vsid.gpe \
	gpe-files/x128.gpe \
	gpe-files/x64.gpe \
	gpe-files/xvic.gpe \
	gpe-files/xpet.gpe \
	gpe-files/xplus4.gpe \
	gpe-files/xcbm2.gpe \
	make-bindist.sh
