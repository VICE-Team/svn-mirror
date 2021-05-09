# Makefile.am for `libjoystickdrv.a'.

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	-I$(top_srcdir)/src/joyport \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libjoystickdrv.a

# These sources are always linked
libjoystickdrv_a_SOURCES = \
	joystick_linux.c

# These sources are conditionally linked
EXTRA_libjoystickdrv_a_SOURCES = \
	joystick_bsd.c


libjoystickdrv_a_DEPENDENCIES = \
	@JOYSTICK_DRIVERS@

libjoystickdrv_a_LIBADD = \
        @JOYSTICK_DRIVERS@
