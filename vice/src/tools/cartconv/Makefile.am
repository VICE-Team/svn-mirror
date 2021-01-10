# vim: set noet ts=8 sw=8 sts=8:
#
# Makefile for cartconv


# Make sure we use Windows' console mode since this is a command line tool
if WIN32_COMPILE
cartconv_LDFLAGS = -mconsole
endif


# Generate svnversion.h if doesn't yet exits (for `make cartconv`)
$(top_builddir)/src/svnversion.h:
	(cd $(top_srcdir)/src; $(MAKE) svnversion.h)


# This is the binary we want to create
bin_PROGRAMS = cartconv


AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src

AM_CFLAGS =  @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


# Sources used cartconv
cartconv_SOURCES = \
	cartconv.c \
	c64-cartridges.c \
	c64-saver.c \
	crt.c \
	$(top_builddir)/src/svnversion.h

# Extra files used for cartconv that should also end up in the `make dist`
EXTRA_DIST = \
	cartconv.h \
	c64-cartridges.h \
	c64-saver.h \
	crt.h

