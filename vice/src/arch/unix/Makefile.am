# Makefile.am for the Unix version of `libarch.a'.

if GNOMEUI

UI = gnome

else

UI = xaw

endif

if NEED_READLINE

SUBDIRS = $(UI) readline

else

SUBDIRS = $(UI)

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
	main_exit.c \
        main_exit.h \
	mouse.c \
	mouse.h \
	petkmap.c \
	print.c \
	rs232.c \
	types.h \
	ui.h \
        uicmdline.c \
        uicmdline.h \
	vsync.c

#	video.c \
#	video.h \

.PHONY: xaw
xaw:
	(cd xaw; $(MAKE))

.PHONY: gnome
gnome:
	(cd gnome; $(MAKE))
