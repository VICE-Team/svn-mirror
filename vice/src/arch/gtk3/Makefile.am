## Makefile.am for the native GTK3 version of `libarch.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	gtk3main.c

EXTRA_DIST = \
	archdep.h \
	archdep_unix.h \
	joy.h \
	kbd.h \
	mousedrv.h \
	types.h
