
AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_LIBRARIES = libp64.a

libp64_a_SOURCES = p64.c

noinst_HEADERS = p64.h p64config.h
