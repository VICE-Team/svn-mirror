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
	c128ui.cc \
	c128ui.h \
	c610kbd.c \
	c610ui.cc \
	c610ui.h \
	c64kbd.c \
	c64ui.cc \
	c64ui.h \
	console.c \
	constants.h \
	joystick.cc \
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
	petui.cc \
	petui.h \
	print.c \
	statusbar.cc \
	statusbar.h \
	types.h \
	ui.cc \
	ui.h \
	ui_datasette.cc \
	ui_datasette.h \
	ui_drive.cc \
	ui_drive.h \
	ui_device.cc \
	ui_device.h \
	ui_file.cc \
	ui_file.h \
	ui_joystick.cc \
	ui_joystick.h \
	ui_sound.cc \
	ui_sound.h \
	ui_vicii.cc \
	ui_vicii.h \
	uicmdline.c \
	uicmdline.h \
	vic20kbd.c \
	vic20ui.cc \
	vic20ui.h \
	vicemenu.cc \
	vicemenu.h \
	vicewindow.cc \
	vicewindow.h \
	video.cc \
	videoarch.h \
	vsidui.c \
	vsync.c
