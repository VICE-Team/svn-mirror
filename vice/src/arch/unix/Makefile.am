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
	c610ui.c \
	c610ui.h \
	c64ui.c \
	c64ui.h \
	console.c \
	coproc.h \
	coproc.c \
	icon.h \
	joystick.c \
	joystick.h \
	kbd.h \
	kmap.c \
	main_exit.c \
	main_exit.h \
	mouse.c \
	mouse.h \
	petkmap.c \
	petui.c \
	petui.h \
	rs232.c \
	types.h \
	ui.h \
	uicartridge.h \
	uicmdline.c \
	uicmdline.h \
	uicommands.h \
	uicommands.c \
	uicrtc.c \
	uicrtc.h \
	uidatasette.c \
	uidatasette.h \
	uiedisk.h \
	uihotkey.h \
	uimenu.h \
	uimon.c \
	uipalette.c \
	uipalette.h \
	uireu.c \
	uireu.h \
	uiscreenshot.h \
	uisettings.h \
	uisettings.c \
	uisnapshot.h \
	uivicii.c \
	uivicii.h \
	vic20ui.c \
	vic20ui.h \
	videoarch.h \
	vsyncarch.c \
	vsidui.c

bin_SCRIPTS = vsid

EXTRA_DIST = vsid

.PHONY: x11
x11:
	(cd x11; $(MAKE))
