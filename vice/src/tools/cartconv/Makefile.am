# vim: set noet ts=8 sw=8 sts=8:
#
# Makefile for cartconv


# Make sure we use Windows' console mode since this is a command line tool
if WIN32_COMPILE
cartconv_LDFLAGS = -mconsole
else
cartconv_LDFLAGS = 
endif


# Generate svnversion.h if it doesn't exist yet (for `make cartconv`)
$(top_builddir)/src/svnversion.h:
	(cd ../..; $(MAKE) svnversion.h)

# cartconv.c needs to include a built header
cartconv.$(OBJEXT): $(top_builddir)/src/svnversion.h

# This is the binary we want to create
bin_PROGRAMS = cartconv

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src

LIBS = 

# Sources used cartconv
cartconv_SOURCES = \
	cartconv.c \
	c64-cartridges.c \
	c64-saver.c \
	plus4-cartridges.c \
	plus4-saver.c \
	vic20-cartridges.c \
	vic20-saver.c \
	crt.c

# Extra files used for cartconv that should also end up in the `make dist`
EXTRA_DIST = \
	cartconv.h \
	c64-cartridges.h \
	c64-saver.h \
	plus4-cartridges.h \
	plus4-saver.h \
	vic20-cartridges.h \
	vic20-saver.h \
	crt.h
