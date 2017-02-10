## Makefile.am for the native GTK3 version of `libarch.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/joyport \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/rs232drv \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	blockdev.c \
	c64ui.c \
	catweaselmkiii-drv.c \
	console.c \
	coproc.c \
	cw-pci.c \
	dynlib.c \
	fullscreen.c \
	gtk3main.c \
	hardsid-drv.c \
	hs-isa.c \
	hs-pci.c \
	io-access.c \
	joy.c \
	kbd.c \
	mididrv.c \
	mousedrv.c \
	parsid-drv.c \
	pci-drv.c \
	ps-device.c \
	ps-io.c \
	rs232dev.c \
	signals.c \
	ssi2001-drv.c \
	ui.c \
	uicmdline.c \
	uimon.c \
	uistatusbar.c \
	video.c \
	vsidui.c \
	vsyncarch.c

EXTRA_DIST = \
	archdep.h \
	archdep_unix.c \
	archdep_unix.h \
	archdep_win32.h \
	catweaselmkiii-unix-drv.c \
	catweaselmkiii-win32-drv.c \
	coproc.h \
	cw-unix.h \
	cw-unix-pci.c \
	cw-win32.h \
	cw-win32-pci.c \
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
	kbd.h \
	midi-osx-drv.c \
	midi-unix-drv.c \
	midi-win32-drv.c \
	mousedrv.h \
	not_implemented.h \
	parsid-unix-drv.c \
	parsid-win32-drv.c \
	pci-drv.h \
	pci-unix-drv.c \
	ps-unix.h \
	ps-unix-file.c \
	ps-unix-io.c \
	ps-win32.h \
	ps-win32-dll.c \
	ps-win32-io.c \
	rs232-unix-dev.c \
	rs232-win32-dev.c \
	socketimpl.h \
	socketimpl_unix.h \
	socketimpl_win32.h \
	ssi2001-unix-drv.c \
	ssi2001-win32-drv.c \
	types.h \
	ui.h \
	videoarch.h
