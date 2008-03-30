# Makefile.am for the Unix X11 specific code `libx11ui.a'.

if GNOMEUI
SUBDIRS = gnome
else
SUBDIRS = xaw
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libx11ui.a

EXTRA_libx11ui_a_SOURCES = \
        fullscreen.c

libx11ui_a_SOURCES = \
	c128icon.c \
	c610icon.c \
	c64icon.c \
	kbd.c \
	kbdef.h \
	peticon.c \
	ui-cmdline-options.c \
	ui-resources.c \
	uihotkey.c \
	vic20icon.c \
	video.c \
	videoarch.h \
	x11kbd.c \
	x11kbd.h \
	x11menu.c \
	x11menu.h

libx11ui_a_DEPENDENCIES = @FULLSCREEN_DRIVERS@

libx11ui_a_LIBADD = @FULLSCREEN_DRIVERS@

.PHONY: xaw gnome
xaw:
	(cd xaw; $(MAKE))
gnome:
	(cd gnome; $(MAKE))
