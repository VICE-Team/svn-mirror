##

# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = xaw
else
SUBDIRS = xaw readline
endif

INCLUDES = \
	-DPREFIX=\"@prefix@\" \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	kbdef.h \
	ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

libarch_a_LIBADD = xaw/libxawui.a

.PHONY: xaw
xaw:
	(cd xaw; $(MAKE))