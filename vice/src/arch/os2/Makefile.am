# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = pm Snippets

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
        config.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
        types.h \
	ui.c \
	ui.h \
        ui_status.c \
        ui_status.h \
        uicmdline.c \
        uicmdline.h \
        pm_cmdline.c \
        pm_cmdline.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

EXTRA_DIST = \
	vice.def

.PHONY: pm
:pm
	(cd pm; $(MAKE))
