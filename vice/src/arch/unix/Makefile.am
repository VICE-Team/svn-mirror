# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = x11 readline
else
SUBDIRS = x11
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.h \
	c610ui.h \
	c64ui.h \
	coproc.h \
	coproc.c \
	joystick.c \
	joystick.h \
	kbd.h \
	kmap.c \
	main_exit.c \
        main_exit.h \
	mouse.c \
	mouse.h \
	petkmap.c \
	petui.h \
	print.c \
	rs232.c \
	types.h \
	ui.h \
        uicmdline.c \
        uicmdline.h \
	vic20ui.h \
	videoarch.h \
	vsync.c

.PHONY: x11
x11:
	(cd x11; $(MAKE))
