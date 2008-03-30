## Makefile.am for the MS-DOS version of `libarch.a'.

if USE_MIDAS_SOUND
libarch_a_LIBADD = vmidas.o
else
libarch_a_LIBADD = vicesb.o
endif

INCLUDES = \
	-DPREFIX=\"@prefix@\" \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128kbd.c \
	c128ui.c \
	c128ui.h \
	c610ui.c \
	c610ui.h \
	c610kbd.c \
	c64kbd.c \
	c64ui.c \
	c64ui.h \
	cbmcharsets.c \
	cbmcharsets.h \
	grabkey.c \
	grabkey.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	mouse.c \
	mouse.h \
	menudefs.c \
	menudefs.h \
	petkbd.c \
	petui.c \
	petui.h \
	sidui.c \
	sidui.h \
	tui.c \
	tui.h \
	tui_backend.c \
	tui_backend.h \
	tuicharset.c \
	tuicharset.h \
	tuiimagebrowser.c \
	tuiimagebrowser.h \
	tuifs.c \
	tuifs.h \
	tuimenu.c \
	tuimenu.h \
	tuiview.c \
	tuiview.h \
	ui.c \
	ui.h \
	uisnapshot.c \
	uisnapshot.h \
	vic20kbd.c \
	vic20ui.c \
	vic20ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

EXTRA_DIST = \
	vicesb.c \
	vicesb.h \
	vmidas.c \
	vmidas.h
