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
	libarch.a \
	libarch64.a \
	libarch128.a \
	libarchcbm2.a \
	libarchpet.a \
	libarchplus4.a \
	libarchvic20.a \
	libarchdummy.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	config.h \
	console.c \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	mousedrv.c \
	mousedrv.h \
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
	video.c \
	videoarch.h \
	vsidarch.h \
	vsync.c \
	vsyncarch.h

libarch64_a_SOURCES = \
	c64ui.c \
	c64c128ui.c \
	c64c128ui.h \
	vsidui.c

libarch128_a_SOURCES = \
	c128ui.c \
	c64c128ui.c \
	c64c128ui.h \
	vsiddummy.c

libarchcbm2_a_SOURCES = \
	cbm2ui.c \
	cbm2dummy.c \
	vsiddummy.c

libarchpet_a_SOURCES = \
	petui.c \
	petdummy.c \
	vsiddummy.c

libarchplus4_a_SOURCES = \
	plus4ui.c \
	vsiddummy.c \
	cbm2dummy.c

libarchvic20_a_SOURCES = \
	vic20ui.c \
	vsiddummy.c

libarchdummy_a_SOURCES = \
	uidummy.c

EXTRA_DIST = \
	ReadMeRO

