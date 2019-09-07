AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	-I$(top_srcdir)/src/rs232drv \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_LIBRARIES = librs232drv.a

librs232drv_a_SOURCES = \
	rs232dev.h \
	rs232.c \
	rs232.h \
	rs232drv.c \
	rs232drv.h \
	rs232net.c \
	rs232net.h \
	rsuser.c \
	rsuser.h
