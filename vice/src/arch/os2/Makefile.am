# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = dialogs doc fonts icons kbd snippets unused watcom

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	console.c \
	fullscr.c \
	fullscr.h \
	fullscros2.h \
	gradd.h \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	makedist.cmd \
	mousedrv.c \
	mousedrv.h \
	os2main.c \
	rs232.c \
	signals.c \
	sounddrv.c \
	sounddrv.h \
	rawnetarch.c \
	types.h \
	ui.c \
	ui.h \
	ui_status.h \
	uicmdline.c \
	uimon.c \
	video.c \
	videoarch.h \
	vsidui.c \
	vsyncarch.c

EXTRA_DIST = \
	vice.def \
	ide-siddefs.h \
	ide-siddtvdefs.h \
	install.cmd \
	makedist.cmd \
	makeclean.cmd

