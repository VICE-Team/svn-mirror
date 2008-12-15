# Makefile.am for the Mac OS X library `libarchmacosx.a'.

if MACOSX_COCOA
SUBDIRS = cocoa Resources
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

noinst_LIBRARIES = libarchmacosx.a

libarchmacosx_a_SOURCES = \
	joy.c \
	joy.h \
	mididrv.c

EXTRA_DIST = \
	Info.plist \
	make-bindist.sh \
	x11-launcher.sh \
	vice-launcher.sh \
	VICE.xcodeproj/project.pbxproj
