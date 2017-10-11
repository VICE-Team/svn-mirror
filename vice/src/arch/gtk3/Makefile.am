## Makefile.am for the native GTK3 version of `libarch.a'.

SUBDIRS = widgets

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
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/arch/gtk3/widgets \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c64dtv \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/scpu64 \
	-I$(top_srcdir)/src/vic20


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
	uiabout.c \
	uiautostart.c \
	uiaccelerators.c \
	uidatasette.c \
	uidiskattach.c \
	uidrivesettings.c \
	uiedit.c \
	uifliplist.c \
	uimenu.c \
	uimon.c \
	uimon-fallback.c \
	uikeyboard.c \
	uisettings.c \
	uismartattach.c \
	uisound.c \
	uispeed.c \
	uistatusbar.c \
	uitapeattach.c \
	vic20ui.c \
	video.c \
	vsidui.c \
	vsyncarch.c \
	uimodel.c \
	uimisc.c \
	uivideo.c \
	uivideosettings.c

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
	make-bindist_win32.sh \
	mousedrv.h \
	not_implemented.h \
	rawnetarch_unix.c \
	rawnetarch_win32.c \
	rs232-unix-dev.c \
	rs232-win32-dev.c \
	ui.h \
	uiabout.h \
	uiautostart.h \
	uiaccelerators.h \
	uidatasette.h \
	uidiskattach.h \
	uidrivesettings.h \
	uifliplist.h \
	uikeyboard.h \
	uiedit.h \
	uimenu.h \
	uisettings.h \
	uismartattach.h \
	uisound.h \
	uispeed.h \
	uistatusbar.h \
	uitapeattach.h \
	videoarch.h \
	uimodel.h \
	uimisc.h \
	uivideo.h \
	uivideosettings.h
