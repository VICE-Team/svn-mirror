## Makefile.am for the BEOS version of `libarch.a'.

SUFFIXES=.def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/raster

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	beos.h \
	c128kbd.c \
	c128ui.c \
	c128ui.h \
	c610kbd.c \
	c610ui.c \
	c610ui.h \
	c64kbd.c \
	c64ui.c \
	c64ui.h \
	console.c \
	constants.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	main.cc \
	main.h \
	main_exit.c \
	main_exit.h \
	mouse.c \
	mouse.h \
	petkbd.c \
	petui.c \
	petui.h \
	print.c \
	types.h \
	ui.cc \
	ui.h \
	ui_file.cc \
	ui_file.h \
	uicmdline.c \
	uicmdline.h \
	vic20kbd.c \
	vic20ui.c \
	vic20ui.h \
	vicewindow.cc \
	vicewindow.h \
	video.cc \
	videoarch.h \
	vsidui.c \
	vsync.c
