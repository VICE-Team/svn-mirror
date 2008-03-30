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
	archdefs.h \
	c128kbd.c \
	c128kbd.h \
	c128ui.c \
	c128ui.h \
	c610kbd.c \
	c610kbd.h \
	c610ui.c \
	c610ui.h \
	c64kbd.c \
	c64kbd.h \
	c64ui.c \
	c64ui.h \
	dirent.c \
	dirent.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	lose32.h \
	petkbd.c \
	petkbd.h \
	petui.c \
	petui.h \
	print.c \
	resc64.r \
	resc64.h \
	ui.c \
	ui.h \
	uiattach.c \
	uiattach.h \
	uidrive.c \
	uidrive.h \
	uilib.c \
	uilib.h \
	uisnapshot.h \
	uisnapshot.c \
	uivicii.h \
	uivicii.c \
	vic20kbd.c \
	vic20kbd.h \
	vic20ui.c \
	vic20ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h \
	winmain.c \
	winmain.h

libarch_a_DEPENDENCIES = \
	resc64.o

EXTRA_DIST = \
        c64.ico

resc64.o : resc64.r resc64.h
	windres -i $< -o $@ --include-dir $(top_srcdir)/src/arch/win32 \
 		--include-dir $(top_srcdir)/src
