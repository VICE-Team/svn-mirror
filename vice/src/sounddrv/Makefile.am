# Makefile.am for `libsounddrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsounddrv.a

# These sources are linked only under certain circumstances.
EXTRA_libsounddrv_a_SOURCES = \
	soundacorn.c \
	soundahi.c \
	soundaix.c \
	soundallegro.c \
	soundalsa.c \
	soundarts.c \
	soundbeos.cc \
	soundcoreaudio.c \
	sounddart.c \
	sounddx.c \
	soundesd.c \
	soundffmpegaudio.c \
	soundhpux.c \
	soundmidas.c \
	soundmmos2.c \
	soundmp3.c \
	soundsdl.c \
	soundsgi.c \
	soundsun.c \
	sounduss.c \
	soundwmm.c

# These sources are always built.
libsounddrv_a_SOURCES = \
	soundaiff.c \
	sounddummy.c \
	sounddump.c \
	soundfs.c \
	soundiff.c \
	soundspeed.c \
	soundvoc.c \
	soundwav.c

libsounddrv_a_DEPENDENCIES = \
	@SOUND_DRIVERS@ \
	soundaiff.o \
	sounddummy.o \
	sounddump.o \
	soundfs.o \
	soundiff.o \
	soundspeed.o \
	soundvoc.o \
	soundwav.o

libsounddrv_a_LIBADD = \
	@SOUND_DRIVERS@

EXTRA_DIST =
