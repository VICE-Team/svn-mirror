# Makefile.am for `libscreendrv.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
        -I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libscreendrv.a

# These sources are linked only under certain circumstances.
EXTRA_libscreendrv_a_SOURCES = \
	png.c

# These sources are always built.
libscreendrv_a_SOURCES = \
	bmp.c

libscreendrv_a_DEPENDENCIES = @SCREENSHOT_DRIVERS@

libscreendrv_a_LIBADD = @SCREENSHOT_DRIVERS@

EXTRA_DIST =
