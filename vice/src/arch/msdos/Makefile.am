## Makefile.am for the MS-DOS version of `libarch.a'.

if USE_MIDAS_SOUND
libarch_a_LIBADD = vmidas.o
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

libarch_a_DEPENDENCIES = @DOS_TFE_PCAP_LIB@

noinst_LIBRARIES = libarch.a

libpcap_nodma.a:
	(echo fixing dos_pcap; cp `gcc -print-file-name=libpcap.a` libpcap_nodma.a; ar -d libpcap_nodma.a dma.o)

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128ui.c \
	c64ui.c \
	cbm2ui.c \
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
	mousedrv.c \
	mousedrv.h \
	msdosmain.c \
	parsid.c \
	petui.c \
	plus4ui.c \
	signals.c \
	statusbar.c \
	statusbar.h \
	tfearch.c \
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
	uic64_256k.c \
	uic64_256k.h \
	uic64cart.c \
	uic64cart.h \
	uicmdline.c \
	uidrive.c \
	uidrive.h \
	uigeoram.c \
	uigeoram.h \
	uijoystick.c \
	uijoystick.h \
	uimon.c \
	uinetplay.c \
	uinetplay.h \
	uiperipherial.c \
	uiperipherial.h \
	uipetreu.c \
	uipetreu.h \
 	uiplus256k.c \
 	uiplus256k.h \
	uiplus60k.c \
	uiplus60k.h \
	uiprinter.c \
	uiprinter.h \
	uiramcart.c \
	uiramcart.h \
	uireu.c \
	uireu.h \
	uiscreenshot.c \
	uiscreenshot.h \
	uisid.c \
	uisid.h \
	uisnapshot.c \
	uisnapshot.h \
	uisound.c \
	uisound.h \
	uitfe.c \
	uitfe.h \
	vic20ui.c \
	video.c \
	videoarch.h \
	vsidui.c \
	vsync.c

EXTRA_DIST = \
	vmidas.c \
	vmidas.h
