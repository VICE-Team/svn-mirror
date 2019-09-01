AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/lib/p64

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_LIBRARIES = libiecbus.a

libiecbus_a_SOURCES = \
	iecbus.c

