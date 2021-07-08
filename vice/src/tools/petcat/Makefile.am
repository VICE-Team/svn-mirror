# vim: set noet ts=8 sw=8 sts=8:
#
# Makefile for petcat


# Make sure we use Windows' console mode since this is a command line tool
if WIN32_COMPILE
petcat_LDFLAGS = -mconsole
else
petcat_LDFLAGS = 
endif

LIBS = 

if USE_SVN_REVISION
# Generate svnversion.h if it doesn't exist yet (for `make petcat`)
$(top_builddir)/src/svnversion.h:
	(cd ../..; $(MAKE) svnversion.h)

# petcat.c needs to include a built header
petcat.$(OBJEXT): $(top_builddir)/src/svnversion.h
endif

# This is the binary we want to create
bin_PROGRAMS = petcat


AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/arch/shared

# Sources used for petcat
petcat_SOURCES = petcat.c

# Extra files used for petcat that also should end up in the `make dist`
