# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = pm

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c64kbd.c \
	c64ui.c \
	c64ui.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	types_new.h \
	ui.c \
	ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

EXTRA_DIST = \
	vice.def

.PHONY: pm
:pm
	(cd pm; $(MAKE))
