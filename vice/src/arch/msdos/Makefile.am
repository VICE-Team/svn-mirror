## Makefile.am for the MS-DOS version of `libarch.a'.

if USE_MIDAS_SOUND
libarch_a_LIBADD = vmidas.o
else
libarch_a_LIBADD = vicesb.o
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.c \
	c128ui.h \
	c64ui.c \
	c64ui.h \
	cbm2ui.c \
	cbm2ui.h \
	cbmcharsets.c \
	cbmcharsets.h \
	console.c \
	grabkey.c \
	grabkey.h \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	menudefs.c \
	menudefs.h \
	mouse.c \
	mouse.h \
	msdosmain.c \
	petui.c \
	petui.h \
	plus4ui.c \
	plus4ui.h \
	statusbar.c \
	statusbar.h \
	tui.c \
	tui.h \
	tui_backend.c \
	tui_backend.h \
	tuicharset.c \
	tuicharset.h \
	tuiimagebrowser.c \
	tuiimagebrowser.h \
	tuifs.c \
	tuifs.h \
	tuimenu.c \
	tuimenu.h \
	tuiview.c \
	tuiview.h \
	types.h \
	ui.c \
	ui.h \
	uiattach.h \
	uiattach.c \
	uicmdline.c \
	uimon.c \
	uisid.c \
	uisid.h \
	uisnapshot.c \
	uisnapshot.h \
	vic20ui.c \
	vic20ui.h \
	video.c \
	videoarch.h \
	vsidui.c \
	vsync.c

EXTRA_DIST = \
	vicesb.c \
	vicesb.h \
	vmidas.c \
	vmidas.h
