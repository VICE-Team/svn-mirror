## Makefile.am for the MS-DOS version of `libarch.a'.

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

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	c128kbd.c \
	c128kbd.h \
	c128ui.c \
	c128ui.h \
	c64kbd.c \
	c64kbd.h \
	c64ui.c \
	c64ui.h \
        grabkey.c \
        grabkey.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	log.c \
	log.h \
	mouse.c \
	mouse.h \
	menudefs.c \
	menudefs.h \
	petkbd.c \
	petkbd.h \
	petui.c \
	petui.h \
	tui.c \
	tui.h \
	tui_backend.c \
	tui_backend.h \
	tuifs.c \
	tuifs.h \
	tuimenu.c \
	tuimenu.h \
	tuiview.c \
	tuiview.h \
	ui.c \
	ui.h \
	vic20kbd.c \
	vic20kbd.h \
	vic20ui.c \
	vic20ui.h \
        vicesb.c \
        vicesb.h \
	video.c \
	video.h \
	vmidas.c \
	vmidas.h \
	vsync.c \
        vsync.h
