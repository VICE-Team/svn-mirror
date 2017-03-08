## Makefile.am for the native GTK3 version of `libarch.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/joyport \
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
	catweaselmkiii-drv.c \
	cbm2ui.c \
	cbm5x0ui.c \
	console.c \
	coproc.c \
	cw-pci.c \
	dynlib.c \
	fullscreen.c \
	gtk3main.c \
	hardsid-drv.c \
	hs-isa.c \
	hs-pci.c \
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
	parsid-drv.c \
	pci-drv.c \
	petui.c \
	plus4ui.c \
	ps-device.c \
	ps-ieee1284.c \
	ps-io.c \
	rawnetarch.c \
	rs232dev.c \
	scpu64ui.c \
	signals.c \
	ssi2001-drv.c \
	ui.c \
	uicmdline.c \
	uimon.c \
	uistatusbar.c \
	unix-io-access.c \
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
	catweaselmkiii-unix-drv.c \
	catweaselmkiii-win32-drv.c \
	coproc.h \
	cw-unix.h \
	cw-unix-pci.c \
	cw-win32.h \
	cw-win32-pci.c \
	debug_gtk3.h \
	dynlib-unix.c \
	dynlib-win32.c \
	hardsid-unix-drv.c \
	hardsid-win32-drv.c \
	hs-unix.h \
	hs-unix-isa.c \
	hs-unix-pci.c \
	hs-win32.h \
	hs-win32-isa.c \
	hs-win32-pci.c \
	io-access.h \
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
	parsid-unix-drv.c \
	parsid-win32-drv.c \
	pci-drv.h \
	pci-unix-drv.c \
	ps-ieee1284-unix.c \
	ps-ieee1284-win32.c \
	ps-unix.h \
	ps-unix-file.c \
	ps-unix-io.c \
	ps-win32.h \
	ps-win32-dll.c \
	ps-win32-io.c \
	rawnetarch_unix.c \
	rawnetarch_win32.c \
	rs232-unix-dev.c \
	rs232-win32-dev.c \
	ssi2001-unix-drv.c \
	ssi2001-win32-drv.c \
	types.h \
	ui.h \
	unix-io-access.h \
	videoarch.h
