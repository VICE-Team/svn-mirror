AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_LIBRARIES = libfileio.a

libfileio_a_SOURCES = \
	cbmfile.c \
	cbmfile.h \
	fileio.c \
	p00.c \
	p00.h
