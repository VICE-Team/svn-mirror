# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = x11 readline
else
SUBDIRS = x11
endif

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

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	c128ui.c \
	c64ui.c \
	catweaselmkiii.c \
	cbm2ui.c \
	console.c \
	coproc.h \
	coproc.c \
	ffmpeglib.c \
	fullscreenarch.h \
	hardsid.c \
	icon.h \
	joy.c \
	joy.h \
	joy_usb.c \
	kbd.h \
	mousedrv.c \
	mousedrv.h \
	opencbmlib.c \
	petui.c \
	plus4ui.c \
	rs232.c \
	signals.c \
	tfearch.c \
	types.h \
	ui.c \
	ui.h \
	uiacia1.c \
	uiacia1.h \
	uiattach.c \
	uiattach.h \
	uic64_256k.c \
	uic64_256k.h \
	uic64cart.c \
	uic64cart.h \
	uicartridge.h \
	uicmdline.c \
	uicommands.h \
	uicommands.c \
	uicrtc.c \
	uicrtc.h \
	uidatasette.c \
	uidatasette.h \
	uidrive.c \
	uidrive.h \
	uidrivec128.c \
	uidrivec128.h \
	uidrivec64vic20.h \
	uidrivec64vic20.c \
	uidriveiec.c \
	uidriveiec.h \
	uidrivepetcbm2.c \
	uidrivepetcbm2.h \
	uidriveplus4.c \
	uidriveplus4.h \
	uiedisk.h \
	uifliplist.c \
	uifliplist.h \
	uigeoram.c \
	uigeoram.h \
	uihotkey.h \
	uiide64.c \
	uiide64.h \
	uijoystick2.c \
	uijoystick2.h \
	uikeyboard.c \
	uikeyboard.h \
	uilib.c \
	uilib.h \
	uimenu.h \
	uimon.c \
	uinetplay.c \
	uinetplay.h \
	uipalemu.c \
	uipalemu.h \
	uipalette.c \
	uipalette.h \
	uiperipheral.c \
	uiperipheral.h \
	uiperipheraliec.c \
	uiperipheraliec.h \
	uiperipheralieee.c \
	uiperipheralieee.h \
	uiplus60k.c \
	uiplus60k.h \
	uiprinter.c \
	uiprinter.h \
	uiprinteriec.c \
	uiprinteriec.h \
	uiprinterieee.c \
	uiprinterieee.h \
	uiramcart.c \
	uiramcart.h \
	uireu.c \
	uireu.h \
	uiromset.c \
	uiromset.h \
	uirs232.c \
	uirs232.h \
	uirs232c64c128.c \
	uirs232c64c128.h \
	uirs232petplus4cbm2.c \
	uirs232petplus4cbm2.h \
	uirs232user.c \
	uirs232user.h \
	uiscreenshot.h \
	uisettings.h \
	uisettings.c \
	uisid.c \
	uisid.h \
	uisnapshot.h \
	uisound.c \
	uisound.h \
	uited.c \
	uited.h \
	uitfe.c \
	uitfe.h \
	uivdc.c \
	uivdc.h \
	uivic.c \
	uivic.h \
	uivicii.c \
	uivicii.h \
	unixmain.c \
	vic20ui.c \
	videoarch.h \
	vsyncarch.c \
	vsidui.c \
	vsiduiunix.h

bin_SCRIPTS = vsid

EXTRA_DIST = vsid minix/minix-ar.sh

.PHONY: x11
x11:
	(cd x11; $(MAKE))
