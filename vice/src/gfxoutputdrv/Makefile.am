# Makefile.am for `libgfxoutputdrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libgfxoutputdrv.a

# These sources are linked only under certain circumstances.
EXTRA_libgfxoutputdrv_a_SOURCES = \
	ffmpegdrv.c \
	ffmpegdrv.h \
	ffmpeglib.h \
	pngdrv.c \
	pngdrv.h

# These sources are always built.
libgfxoutputdrv_a_SOURCES = \
	bmpdrv.c \
	bmpdrv.h \
	gfxoutput.c

libgfxoutputdrv_a_DEPENDENCIES = @GFXOUTPUT_DRIVERS@
libgfxoutputdrv_a_LIBADD = @GFXOUTPUT_DRIVERS@

EXTRA_DIST = \
    ffmpeg/avcodec.h \
    ffmpeg/avformat.h \
    ffmpeg/avio.h \
    ffmpeg/common.h \
    ffmpeg/rtp.h \
    ffmpeg/rtsp.h \
    ffmpeg/rtspcodes.h
