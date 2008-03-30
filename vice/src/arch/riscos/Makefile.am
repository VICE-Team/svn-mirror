## Makefile.am for the RISC OS version of `libarch.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.c \
	c128ui.h \
	c610ui.c \
	c610ui.h \
	c64c128ui.c \
	c64c128ui.h \
	c64ui.c \
	c64ui.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	petdummy.c \
	petui.c \
	petui.h \
	print.c \
	rs232.c \
	types.h \
	ui.c \
	ui.h \
	uidummy.c \
	uiimage.c \
	uiimage.h \
	vic20ui.c \
	vic20ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h

EXTRA_DIST = \
	ReadMeRO

