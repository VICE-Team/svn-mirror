# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = gui x11 readline
else
SUBDIRS = gui x11
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	@FFMPEG_INCLUDES@ \
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
	catweaselmkiii.c \
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
	parsid.c \
	rs232.c \
	signals.c \
	tfearch.c \
	types.h \
	ui.h \
	uiacia1.h \
	uiattach.h \
	uic64_256k.h \
	uic64cart.h \
	uicartridge.h \
	uicommands.h \
	uicrtc.h \
	uidatasette.h \
	uidrive.h \
	uidrivec128.h \
	uidrivec64vic20.h \
	uidriveiec.h \
	uidrivepetcbm2.h \
	uidriveplus4.h \
	uiedisk.h \
	uifliplist.h \
	uigeoram.h \
	uihotkey.h \
	uiide64.h \
	uijoystick2.h \
	uikeyboard.h \
	uilib.h \
	uimenu.h \
	uinetplay.h \
	uipalemu.h \
	uipalette.h \
	uiperipheral.h \
	uiperipheraliec.h \
	uiperipheralieee.h \
	uipetreu.h \
 	uiplus256k.h \
	uiplus60k.h \
	uiprinter.h \
	uiprinteriec.h \
	uiprinteriecplus4.h \
	uiprinterieee.h \
	uiramcart.h \
	uireu.h \
	uiromset.h \
	uirs232.h \
	uirs232c64c128.h \
	uirs232petplus4cbm2.h \
	uirs232user.h \
	uiscreenshot.h \
	uisettings.h \
	uisid.h \
	uisnapshot.h \
	uisound.h \
	uited.h \
	uitfe.h \
	uivdc.h \
	uivic.h \
	uivicii.h \
	unixmain.c \
	videoarch.h \
	vsyncarch.c \
	vsiduiunix.h

bin_SCRIPTS = vsid

EXTRA_DIST = \
	vsid \
	minix/minix-ar.sh \
	qnx/make-bindist.sh \
	solaris/make-bindist.sh \
	solaris/make_package.pl

.PHONY: x11
x11:
	(cd x11; $(MAKE))

.PHONY: gui
gui:
	(cd gui; $(MAKE))

