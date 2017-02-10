## Makefile.am for the native GTK3 version of `libarch.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	gtk3main.c \
	ssi2001-drv.c

EXTRA_DIST = \
	archdep.h \
	archdep_unix.h \
	coproc.h \
	io-access.h \
	joy.h \
	kbd.h \
	mousedrv.h \
	socketimpl.h \
	socketimpl_unix.h \
	ssi2001-unix-drv.c \
	ssi2001-win32-drv.c \
	types.h \
	ui.h \
	videoarch.h
