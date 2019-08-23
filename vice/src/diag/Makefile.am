AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = $(VICE_CFLAGS)

AM_CXXFLAGS = $(VICE_CXXFLAGS)

noinst_LIBRARIES = libdiag.a

libdiag_a_SOURCES = \
	c64_diag_586220_harness.c \
	c64_diag_586220_harness.h
