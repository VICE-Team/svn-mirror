## Makefile.am for the RISC OS version of `libarch.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/monitor \
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
	cbm2dummy.c \
	config.h \
	console.c \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	main_exit.h \
	petdummy.c \
	petui.c \
	petui.h \
	plus4ui.c \
	plus4ui.h \
	rs232.c \
	spritedrv.c \
	types.h \
	ui.c \
	ui.h \
	uicmdline.c \
	uicmdline.h \
	uiconfig.c \
	uiconfig.h \
	uidummy.c \
	uihelp.c \
	uihelp.h \
	uiimage.c \
	uiimage.h \
	uimon.c \
	uimsgwin.c \
	uimsgwin.h \
	uisharedef.h \
	vic20ui.c \
	vic20ui.h \
	video.c \
	videoarch.h \
	vsidarch.h \
	vsidui.c \
	vsiddummy.c \
	vsync.c \
	vsyncarch.h

EXTRA_DIST = \
	ReadMeRO
