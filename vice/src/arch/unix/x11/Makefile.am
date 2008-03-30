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

libx11ui_a_SOURCES = \
	kbd.c \
	kbdef.h \
	video.c \
	videoarch.h \
	c128icon.xpm \
	c610icon.xpm \
	c64icon.xpm \
	peticon.xpm \
	vic20icon.xpm

.PHONY: xaw gnome
xaw:
	(cd xaw; $(MAKE))
gnome:
	(cd gnome; $(MAKE))
