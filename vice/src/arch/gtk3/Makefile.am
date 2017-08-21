## Makefile.am for the native GTK3 version of `libarch.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/joyport \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/platform \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/rs232drv \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	blockdev.c \
	c128ui.c \
	c64dtvui.c \
	c64scui.c \
	c64ui.c \
	cbm2ui.c \
	cbm5x0ui.c \
	console.c \
	coproc.c \
	dynlib.c \
	fullscreen.c \
	gtk3main.c \
	joy-osx.c \
	joy-osx-hid.c \
	joy-osx-hidmgr.c \
	joy-osx-hidutil.c \
	joy-unix.c \
	joy-unix-usb.c \
	joy-win32.c \
	joy-win32-dinput-handle.c \
	kbd.c \
	mousedrv.c \
	petui.c \
	plus4ui.c \
	rawnetarch.c \
	rs232dev.c \
	scpu64ui.c \
	signals.c \
	ui.c \
	uiaccelerators.c \
	uimenu.c \
	uimon.c \
	uimon-fallback.c \
	uistatusbar.c \
	vic20ui.c \
	video.c \
	vsidui.c \
	vsyncarch.c

EXTRA_DIST = \
	archdep.h \
	archdep_unix.c \
	archdep_unix.h \
	archdep_win32.c \
	archdep_win32.h \
	coproc.h \
	debug_gtk3.h \
	dynlib-unix.c \
	dynlib-win32.c \
	joy.h \
	joy-osx.h \
	joy-osx-hid.h \
	joy-osx-hidlib.h \
	joy-unix.h \
	joy-win32.h \
	joy-win32-dinput-handle.h \
	kbd.h \
	mousedrv.h \
	not_implemented.h \
	rawnetarch_unix.c \
	rawnetarch_win32.c \
	rs232-unix-dev.c \
	rs232-win32-dev.c \
	types.h \
	ui.h \
	uiaccelerators.h \
	uimenu.h \
	uistatusbar.h \
	videoarch.h
