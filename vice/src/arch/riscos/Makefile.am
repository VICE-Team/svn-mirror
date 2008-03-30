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

noinst_LIBRARIES = \
	libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	c128ui.c \
	c64ui.c \
	c64c128ui.c \
	c64c128ui.h \
	cbm2ui.c \
	config.h \
	console.c \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	mousedrv.c \
	mousedrv.h \
	petui.c \
	plus4ui.c \
	rs232.c \
	riscosmain.c \
	spritedrv.c \
	types.h \
	ui.c \
	ui.h \
	uicmdline.c \
	uiconfig.c \
	uiconfig.h \
	uihelp.c \
	uihelp.h \
	uiimage.c \
	uiimage.h \
	uimon.c \
	uimsgwin.c \
	uimsgwin.h \
	uisharedef.h \
	vic20ui.c \
	video.c \
	videoarch.h \
	vsidarch.h \
	vsidui.c \
	vsync.c \
	vsyncarch.h

EXTRA_DIST = \
	ReadMeRO

