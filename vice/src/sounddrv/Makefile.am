
# Makefile.am for `libsounddrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsounddrv.a

# These sources are linked only under certain circumstances.
EXTRA_libsounddrv_a_SOURCES = \
	soundaix.c \
	soundallegro.c \
	soundhpux.c \
	soundmidas.c \
	soundsb.c \
	soundsdl.c \
	soundsgi.c \
	soundsun.c \
	sounduss.c

# These sources are always built.
libsounddrv_a_SOURCES = \
	sounddummy.c \
	sounddump.c \
	soundfs.c \
	soundspeed.c

libsounddrv_a_DEPENDENCIES = \
	@SOUND_DRIVERS@ \
	sounddummy.o \
	sounddump.o \
	soundfs.o \
	soundspeed.o

libsounddrv_a_LIBADD = \
	@SOUND_DRIVERS@