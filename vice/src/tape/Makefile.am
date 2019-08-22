AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = $(VICE_CFLAGS) @CFLAGS@

AM_CXXFLAGS = $(VICE_CXXFLAGS) @CXXFLAGS@

noinst_LIBRARIES = libtape.a

libtape_a_SOURCES = \
	t64.c \
	t64.h \
	tap.c \
	tape-internal.c \
	tape-internal.h \
	tape-snapshot.c \
	tape-snapshot.h \
	tape.c \
	tapeimage.c \
	tapeimage.h

