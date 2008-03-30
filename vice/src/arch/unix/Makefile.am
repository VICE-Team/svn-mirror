# Makefile.am for the Unix version of `libarch.a'.

if NEED_READLINE
SUBDIRS = x11 readline
else
SUBDIRS = x11
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.h \
	c128ui.c \
	c610ui.h \
	c610ui.c \
	c64ui.h \
	c64ui.c \
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
	petui.c \
	print.c \
	rs232.c \
	types.h \
	ui.h \
	uicmdline.c \
	uicmdline.h \
	uicommands.h \
	uicommands.c \
        uiedisk.h \
	uimenu.h \
	uisettings.h \
	uisettings.c \
	uisnapshot.h \
	vic20ui.h \
	vic20ui.c \
	videoarch.h \
	vsync.c

.PHONY: x11
x11:
	(cd x11; $(MAKE))
