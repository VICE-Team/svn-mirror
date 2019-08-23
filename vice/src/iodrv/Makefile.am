# Makefile.am for `libiodrv.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = $(VICE_CFLAGS)

AM_CXXFLAGS = $(VICE_CXXFLAGS)

noinst_LIBRARIES = libiodrv.a

libiodrv_a_SOURCES = \
	io-access.h \
	io-beos-access.c \
	io-unix-access.c \
	pci-beos-drv.c \
	pci-beos-drv.h \
	pci-unix-drv.c \
	pci-unix-drv.h
