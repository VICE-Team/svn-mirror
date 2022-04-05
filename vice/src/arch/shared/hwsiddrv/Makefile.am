# Makefile.am for `libhwsiddrv.a'.

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/platform \
	-I$(top_srcdir)/src/arch/shared \
	-I$(top_srcdir)/src/arch/shared/iodrv

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libhwsiddrv.a

# Determine which drivers need to be built, start with an empty list
# (HardSID is currently enabled by default on system that have drivers for it)
HWSIDDRIVERS=

# Windows drivers
if WINDOWS_COMPILE
HWSIDDRIVERS += \
	hardsid-win32-drv.o \
	hs-win32-dll.o \
	hs-win32-isa.o \
	hs-win32-pci.o
if HAVE_CATWEASELMKIII
HWSIDDRIVERS += \
	catweaselmkiii-win32-drv.o \
	cw-win32-dll.o \
	cw-win32-pci.o
endif HAVE_CATWEASELMKIII
if HAVE_PARSID
HWSIDDRIVERS += \
	parsid-win32-drv.o \
	ps-ieee1284.o \
	ps-win32-dll.o \
	ps-win32-io.o
endif HAVE_PARSID
if HAVE_SSI2001
HWSIDDRIVERS += ssi2001-win32-drv.o
endif HAVE_SSI2001
endif WINDOWS_COMPILE

if BEOS_COMPILE
HWSIDDRIVERS += \
	hardsid-beos-drv.o \
	hs-beos-isa.o \
	hs-beos-pci.o
if HAVE_CATWEASELMKIII
HWSIDDRIVERS += catweaselmkiii-beos-drv.o
endif HAVE_CATWEASELMKIII
if HAVE_PARSID
HWSIDDRIVERS += \
	parsid-beos-drv.o \
	ps-ieee1284.o
endif HAVE_PARSID
if HAVE_SSI2001
HWSIDDRIVERS += ssi2001-beos-drv.o
endif HAVE_SSI2001
endif BEOS_COMPILE

if UNIX_COMPILE
if !MACOS_COMPILE
HWSIDDRIVERS += \
	hardsid-unix-drv.o \
	hs-unix-isa.o \
	hs-unix-pci.o
if LINUX_COMPILE
HWSIDDRIVERS += hs-unix-linux.o
endif LINUX_COMPILE
if HAVE_CATWEASELMKIII
HWSIDDRIVERS += \
	catweaselmkiii-unix-drv.o \
	cw-unix-device.o \
	cw-unix-pci.o
endif HAVE_CATWEASELMKIII
if HAVE_PARSID
HWSIDDRIVERS += \
	parsid-unix-drv.o \
	ps-ieee1284.o \
	ps-unix-file.o \
	ps-unix-io.o
endif HAVE_PARSID
if HAVE_SSI2001
HWSIDDRIVERS += ssi2001-unix-drv.o
endif HAVE_SSI2001
endif !MACOS_COMPILE
endif UNIX_COMPILE


# Built depending on configure options:
EXTRA_libhwsiddrv_a_SOURCES = \
	catweaselmkiii-beos-drv.c \
	catweaselmkiii-unix-drv.c \
	catweaselmkiii-win32-drv.c \
	cw-unix-device.c \
	cw-unix-pci.c \
	cw-win32-dll.c \
	cw-win32-pci.c \
	hardsid-beos-drv.c \
	hardsid-unix-drv.c \
	hardsid-win32-drv.c \
	hs-beos-isa.c \
	hs-beos-pci.c \
	hs-unix-isa.c \
	hs-unix-linux.c \
	hs-unix-pci.c \
	hs-win32-dll.c \
	hs-win32-isa.c \
	hs-win32-pci.c \
	parsid-beos-drv.c \
	parsid-unix-drv.c \
	parsid-win32-drv.c \
	ps-ieee1284.c \
	ps-unix-file.c \
	ps-unix-io.c \
	ps-win32-dll.c \
	ps-win32-io.c \
	ssi2001-beos-drv.c \
	ssi2001-unix-drv.c \
	ssi2001-win32-drv.c

EXTRA_DIST = \
	cw-unix.h \
	cw-win32.h \
	hs-beos.h \
	hs-unix.h \
	hs-win32.h \
	ps-unix.h \
	ps-win32.h \
	wininpoutp.h
						 
# Always built:
# (needs to be declared empty to avoid `make` to try to build libhwsiddrv.o
# from libhwsiddrv.c)
libhwsiddrv_a_SOURCES = 

# Make sure we actually link the generated object files into the library:
libhwsiddrv_a_LIBADD = $(HWSIDDRIVERS)
