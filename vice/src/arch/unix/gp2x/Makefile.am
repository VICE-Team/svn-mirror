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
	icon.h \
	input_gp2x.c \
	input_gp2x.h \
	minimal.c \
	minimal.h \
	minimal_940t.h \
	petui.c \
	plus4ui.c \
	prefs_gp2x.c \
	prefs_gp2x.h \
	rs232.c \
	ui.c \
	ui_gp2x.c \
	ui_gp2x.h \
	uiacia1.c \
	uiarch.h \
	uiattach.c \
	uic64_256k.c \
	uic64cart.c \
	uicmdline.c \
	uicommands.c \
	uicrtc.c \
	uidatasette.c \
	uidrive.c \
	uidrivec128.c \
	uidrivec64vic20.c \
	uidriveiec.c \
	uidrivepetcbm2.c \
	uidriveplus4.c \
	uifliplist.c \
	uigeoram.c \
	uiide64.c \
	uijoystick2.c \
	uikeyboard.c \
	uilib.c \
	uilib.h \
	uimenu.h \
	uimon.c \
	uipalemu.c \
	uipalette.c \
	uiperipheral.c \
	uiperipheraliec.c \
	uiperipheralieee.c \
	uipetreu.c \
 	uiplus256k.c \
	uiplus60k.c \
	uiprinter.c \
	uiprinteriec.c \
	uiprinteriecplus4.c \
	uiprinterieee.c \
	uiramcart.c \
	uireu.c \
	uiromset.c \
	uirs232.c \
	uirs232c64c128.c \
	uirs232petplus4cbm2.c \
	uirs232user.c \
	uisettings.c \
	uisid.c \
	uisound.c \
	uited.c \
	uitext_gp2x.c \
	uitext_gp2x.h \
	uivdc.c \
	uivic.c \
	uivicii.c \
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
