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
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.c \
	c128ui.h \
	c64ui.c \
	c64ui.h \
	cbm2ui.c \
	cbm2ui.h \
	console.c \
	coproc.h \
	coproc.c \
	fullscreenarch.h \
	icon.h \
	joy.c \
	joy.h \
	kbd.h \
	main_exit.c \
	main_exit.h \
	mouse.c \
	mouse.h \
	petui.c \
	petui.h \
	plus4ui.c \
	plus4ui.h \
	rs232.c \
	types.h \
	ui.h \
	uic64cart.c \
	uic64cart.h \
	uicartridge.h \
	uicmdline.c \
	uicmdline.h \
	uicommands.h \
	uicommands.c \
	uicrtc.c \
	uicrtc.h \
	uidatasette.c \
	uidatasette.h \
	uidrive.c \
	uidrive.h \
	uiedisk.h \
	uihotkey.h \
	uijoystick2.c \
	uijoystick2.h \
	uimenu.h \
	uimon.c \
	uipalemu.c \
	uipalemu.h \
	uipalette.c \
	uipalette.h \
	uiperipheral.c \
	uiperipheral.h \
	uiprinter.c \
	uiprinter.h \
	uireu.c \
	uireu.h \
	uirs232.c \
	uirs232.h \
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
	uivdc.c \
	uivdc.h \
	uivic.c \
	uivic.h \
	uivicii.c \
	uivicii.h \
	vic20ui.c \
	vic20ui.h \
	videoarch.h \
	vsyncarch.c \
	vsidui.c \
	vsiduiunix.h

bin_SCRIPTS = vsid

EXTRA_DIST = vsid

.PHONY: x11
x11:
	(cd x11; $(MAKE))
