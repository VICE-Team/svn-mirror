# Makefile.am for `libsounddrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsounddrv.a

# These sources are linked only under certain circumstances.
EXTRA_libsounddrv_a_SOURCES = \
	soundaix.c \
	soundalsa.c \
	soundarts.c \
	soundacorn.c \
	soundallegro.c \
	soundbeos.cc \
	soundcoreaudio.c \
	sounddart.c \
	sounddx.c \
	soundesd.c \
	soundhpux.c \
	soundmidas.c \
	soundmmos2.c \
	soundffmpegaudio.c \
	soundsdl.c \
	soundsgi.c \
	soundsun.c \
	sounduss.c \
	soundwmm.c

# These sources are always built.
libsounddrv_a_SOURCES = \
	sounddummy.c \
	sounddump.c \
	soundfs.c \
	soundspeed.c \
	soundwav.c

libsounddrv_a_DEPENDENCIES = \
	@SOUND_DRIVERS@ \
	sounddummy.o \
	sounddump.o \
	soundfs.o \
	soundspeed.o \
	soundwav.c

libsounddrv_a_LIBADD = \
	@SOUND_DRIVERS@

EXTRA_DIST =
