# Makefile.am for `libgfxoutputdrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	@VICE_LIBPNG_INCLUDES@ \
	@VICE_ZLIB_INCLUDES@ \
	@FFMPEG_INCLUDES@ \
	@QUICKTIME_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libgfxoutputdrv.a

# These sources are linked only under certain circumstances.
EXTRA_libgfxoutputdrv_a_SOURCES = \
	ffmpegdrv.c \
	ffmpegdrv.h \
	ffmpeglib.h \
	gifdrv.c \
	jpegdrv.c \
	jpegdrv.h \
	pngdrv.c \
	pngdrv.h \
	quicktimedrv.h \
	quicktimedrv.c

# These sources are always built.
libgfxoutputdrv_a_SOURCES = \
	bmpdrv.c \
	bmpdrv.h \
	gfxoutput.c \
	iffdrv.c \
	iffdrv.h \
	pcxdrv.c \
	pcxdrv.h \
	ppmdrv.c \
	ppmdrv.h

libgfxoutputdrv_a_DEPENDENCIES = @GFXOUTPUT_DRIVERS@
libgfxoutputdrv_a_LIBADD = @GFXOUTPUT_DRIVERS@

