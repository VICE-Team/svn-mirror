# Makefile.am for `libjoyport.a'.

AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/samplerdrv \
	-I$(top_srcdir)/src/rtc

noinst_LIBRARIES = libjoyport.a

# These sources are always built.
libjoyport_a_SOURCES = \
	joyport.c \
	joyport.h \
	lightpen.c \
	lightpen.h \
	mouse.c \
	mouse.h \
	sampler2bit.c \
	sampler2bit.h
