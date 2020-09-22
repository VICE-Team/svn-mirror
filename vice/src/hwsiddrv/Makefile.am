# Makefile.am for `libhwsiddrv.a'.

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/platform \
	-I$(top_srcdir)/src/iodrv \
	-I$(top_srcdir)/src/arch/shared

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libhwsiddrv.a

libhwsiddrv_a_SOURCES = \
	catweaselmkiii-beos-drv.c \
	catweaselmkiii-unix-drv.c \
	catweaselmkiii-win32-drv.c \
	cw-unix.h \
	cw-unix-device.c \
	cw-unix-pci.c \
	cw-win32.h \
	cw-win32-dll.c \
	cw-win32-pci.c \
	hardsid-beos-drv.c \
	hardsid-unix-drv.c \
	hardsid-win32-drv.c \
	hs-beos.h \
	hs-beos-isa.c \
	hs-beos-pci.c \
	hs-unix.h \
	hs-unix-isa.c \
	hs-unix-linux.c \
	hs-unix-pci.c \
	hs-win32.h \
	hs-win32-dll.c \
	hs-win32-isa.c \
	hs-win32-pci.c \
	parsid-beos-drv.c \
	parsid-unix-drv.c \
	parsid-win32-drv.c \
	ps-ieee1284.c \
	ps-unix.h \
	ps-unix-file.c \
	ps-unix-io.c \
	ps-win32.h \
	ps-win32-dll.c \
	ps-win32-io.c \
	ssi2001-beos-drv.c \
	ssi2001-unix-drv.c \
	ssi2001-win32-drv.c \
	wininpoutp.h

