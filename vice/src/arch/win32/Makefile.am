## Makefile.am for the Win32 version of `libarch.a'.

SUFFIXES = .r

INCLUDES = \
	-DPREFIX=\"@prefix@\" \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	c64kbd.c \
	c64ui.c \
	c64ui.h \
	dirent.c \
	dirent.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
        lose32.h \
        print.c \
        resc64.r \
        resc64.h \
	ui.c \
	ui.h \
        uiattach.c \
        uiattach.h \
	video.c \
	video.h \
	vsync.c \
        vsync.h \
        winmain.c \
        winmain.h

libarch_a_LIBADD = \
	resc64.o

libarch_a_DEPENDENCIES = \
	resc64.o

EXTRA_DIST = \
        c64.ico

resc64.o : resc64.r resc64.h
	windres -i $< -o $@
