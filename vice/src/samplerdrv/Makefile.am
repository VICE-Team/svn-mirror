# Makefile.am for `libsamplerdrv.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsamplerdrv.a

# These sources are always built.
libsamplerdrv_a_SOURCES = \
	portaudio_drv.c \
	portaudio_drv.h \
	sampler.c \
	sampler.h
