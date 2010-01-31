# Makefile.am for the GP2X version of `libarchgp2xsys.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/imagecontents \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarchgp2xsys.a

libarchgp2xsys_a_SOURCES = \
	gp2x_funcs.c \
	gp2x_usb.c \
	minimal.c \
	minimal.h
