AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/lib/p64

AM_CFLAGS = $(VICE_CFLAGS) @CFLAGS@

AM_CXXFLAGS = $(VICE_CXXFLAGS) @CXXFLAGS@

noinst_LIBRARIES = libdriveiecieee.a

libdriveiecieee_a_SOURCES = \
	iecieee.c \
	via2d.c

