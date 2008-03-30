# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = xaw
else
SUBDIRS = xaw readline
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	coproc.h \
	coproc.c \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	kbdef.h \
	kmap.c \
	mouse.c \
	mouse.h \
	petkmap.c \
	print.c \
	rs232.c \
	ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

.PHONY: xaw
xaw:
	(cd xaw; $(MAKE))
