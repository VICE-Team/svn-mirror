
# Makefile.am for `libsounddrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsounddrv.a

if SOUND_USS
SOUNDUSS = sounduss.c
else
SOUNDUSS =
endif

# These sources are linked only under certain circumstances.
EXTRA_libsounddrv_a_SOURCES = \
	soundaix.c \
	soundhpux.c \
	soundmidas.c \
	soundsgi.c \
	soundsun.c \
	sounduss.c

# These sources are always (inconditionally) built.
libsounddrv_a_SOURCES = \
	$(SOUNDUSS) \
	sounddummy.c \
	sounddump.c \
	soundfs.c \
	soundspeed.c
