AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/core/rtc \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/drive/iec \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/userport

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libdriveiecc64exp.a

libdriveiecc64exp_a_SOURCES = \
	c64exp-cmdline-options.c \
	c64exp-cmdline-options.h \
	c64exp-resources.c \
	c64exp-resources.h \
	iec-c64exp.c \
	dolphindos3.c \
	dolphindos3.h \
	profdos.c \
	profdos.h \
	stardos-exp.c \
	stardos-exp.h \
	supercard.c \
	supercard.h
