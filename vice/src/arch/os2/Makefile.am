# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = pm snippets vac++ kbd dialogs icons

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
        c128ui.c \
        c128ui.h \
	c64ui.c \
	c64ui.h \
	config.h \
	console.c \
	contentsdlg.c \
	contentsdlg.h \
	dialogs.h \
	dialogs.c \
	dialogs.rc \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	main_exit.c \
	main_exit.h \
	petui.c \
	petui.h \
        pm_cmdline.c \
        pm_cmdline.h \
	rs232.c \
	signals.h \
	sounddrv.c \
	sounddrv.h \
	types.h \
	ui.c \
	ui.h \
	ui_status.c \
	ui_status.h \
	uicmdline.c \
	uicmdline.h \
	video_gpi.c \
	video_gpi.h \
	video.c \
	videoarch.h \
	vsidui.c \
	vsync.c \
	vsync_old.c \
	vsync_old.h

EXTRA_DIST = \
	vice.def \
        install.cmd

.PHONY: pm
:pm
	(cd pm; $(MAKE))
