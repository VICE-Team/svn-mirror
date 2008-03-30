# Makefile.am for the Unix version of `libarch.a'.

if UNIX_GP2X_COMPILE
SUBDIRS = gp2x
else
if NEED_READLINE
SUBDIRS = gui x11 readline
else
SUBDIRS = gui x11
endif
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	@FFMPEG_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	catweaselmkiii.c \
	console.c \
	coproc.h \
	coproc.c \
	ffmpeglib.c \
	fullscreenarch.h \
	hardsid.c \
	icon.h \
	joy.c \
	joy.h \
	joy_usb.c \
	kbd.h \
	mousedrv.c \
	mousedrv.h \
	opencbmlib.c \
	parsid.c \
	rs232.c \
	signals.c \
	tfearch.c \
	types.h \
	ui.h \
	uimenu.h \
	unixmain.c \
	videoarch.h \
	vsyncarch.c \
	vsiduiunix.h

bin_SCRIPTS = vsid

EXTRA_DIST = \
	joy_mac.c \
	macosx/Info.plist \
	macosx/VICE.icns \
	macosx/make-bindist.sh \
	minix/minix-ar.sh \
	qnx/make-bindist.sh \
	solaris/make-bindist.sh \
	solaris/make_package.pl \
	vsid

.PHONY: x11
x11:
	(cd x11; $(MAKE))

.PHONY: gp2x
gp2x:
	(cd gp2x; $(MAKE))

.PHONY: gui
gui:
	(cd gui; $(MAKE))

