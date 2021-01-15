# vim: set noet ts=8 sw=8 sts=8:
#
# Makefile for petcat


# Make sure we use Windows' console mode since this is a command line tool
if WIN32_COMPILE
petcat_LDFLAGS = -mconsole
endif


# Generate svnversion.h if doesn't yet exits (for `make cartconv`)
$(top_builddir)/src/svnversion.h:
	(cd $(top_srcdir)/src; $(MAKE) svnversion.h)


# This is the binary we want to create
bin_PROGRAMS = petcat


AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/arch/shared

AM_CFLAGS =  @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


# Sources used for petcat
petcat_SOURCES = petcat.c

# Extra files used for cartconv that should also end up in the `make dist`
