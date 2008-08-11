# Makefile.am for the AMIGAOS version of `libarch.a'.

bin_PROGRAMS = x64ns x64dtvns x128ns xvicns xplus4ns xcbm2ns xpetns

x64ns_SOURCES = x64ns.c

x64dtvns_SOURCES = x64dtvns.c

x128ns_SOURCES = x128ns.c

xvicns_SOURCES = xvicns.c

xplus4ns_SOURCES = xplus4ns.c

xcbm2ns_SOURCES = xcbm2ns.c

xpetns_SOURCES = xpetns.c

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c64dtv \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	ahi.c \
	ahi.h \
	amigamain.c \
	archdep.c \
	archdep.h \
	blockdev.c \
	c128ui.c \
	c128uires.h \
	c64ui.c \
	c64uires.h \
	c64dtvui.c \
	c64dtvuires.h \
	catweaselmkiii.c \
	cbm2ui.c \
	cbm2uires.h \
	console.c \
	fullscreen.c \
	fullscreenarch.h \
	hardsid.c \
	intl.c \
	intl_funcs.h \
	joy.c \
	joy.h \
	joyai.c \
	joyai.h \
	joyll.c \
	joyll.h \
	kbd.c \
	kbd.h \
	memory.h \
	mousedrv.c \
	mousedrv.h \
	noinlineargs.c \
	petui.c \
	petuires.h \
	plus4ui.c \
	plus4uires.h \
	pointer.c \
	pointer.h \
	private.h \
	rs232.c \
	screen-shot.c \
	screen-shot.h \
	signals.c \
	statusbar.c \
	statusbar.h \
	tfearch.c \
	timer.c \
	timer.h \
	types.h \
	ui.c \
	ui.h \
	uiapi.c \
	uiattach.c \
	uiattach.h \
	uic64cart.c \
	uic64cart.h \
	uicart.c \
	uicart.h \
	uicmdline.c \
	uicolor.c \
	uilib.c \
	uilib.h \
	uimon.c \
	uires.h \
	vic20ui.c \
	vic20uires.h \
	video.c \
	videoarch.h \
	vsidui.c \
	vsyncapi.c \
	mui/filereq.c \
	mui/filereq.h \
	mui/mui.c \
	mui/mui.h \
	mui/uiacia.c \
	mui/uiacia.h \
	mui/uic64_256k.c \
	mui/uic64_256k.h \
	mui/uic64dtv.c \
	mui/uic64dtv.h \
	mui/uic128settings.c \
	mui/uic128settings.h \
	mui/uicbm2settings.c \
	mui/uicbm2settings.h \
	mui/uidatasette.c \
	mui/uidatasette.h \
	mui/uidigimax.c \
	mui/uidigimax.h \
	mui/uidrivec128.c \
	mui/uidrivec128.h \
	mui/uidrivec64vic20.c \
	mui/uidrivec64vic20.h \
	mui/uidrivepetcbm2.c \
	mui/uidrivepetcbm2.h \
	mui/uidriveplus4.c \
	mui/uidriveplus4.h \
	mui/uifliplist.c \
	mui/uifliplist.h \
	mui/uigeoram.c \
	mui/uigeoram.h \
	mui/uiide64.c \
	mui/uiide64.h \
	mui/uijoystick.c \
	mui/uijoystick.h \
	mui/uijoystickll.c \
	mui/uijoystickll.h \
	mui/uimmc64.c \
	mui/uimmc64.h \
	mui/uimouse.c \
	mui/uimouse.h \
	mui/uinetwork.c \
	mui/uinetwork.h \
	mui/uipetreu.c \
	mui/uipetreu.h \
	mui/uipetsettings.c \
	mui/uipetsettings.h \
	mui/uiplus256k.c \
	mui/uiplus256k.h \
	mui/uiplus4settings.c \
	mui/uiplus4settings.h \
	mui/uiplus60k.c \
	mui/uiplus60k.h \
	mui/uiram.c \
	mui/uiram.h \
	mui/uiramcart.c \
	mui/uiramcart.h \
	mui/uireu.c \
	mui/uireu.h \
	mui/uiromc128settings.c \
	mui/uiromc128settings.h \
	mui/uiromc64vic20settings.c \
	mui/uiromc64vic20settings.h \
	mui/uiromcbm2settings.c \
	mui/uiromcbm2settings.h \
	mui/uirompetsettings.c \
	mui/uirompetsettings.h \
	mui/uiromplus4settings.c \
	mui/uiromplus4settings.h \
	mui/uirs232user.c \
	mui/uirs232user.h \
	mui/uisid.c \
	mui/uisid.h \
	mui/uisidcart.c \
	mui/uisidcart.h \
	mui/uisnapshot.c \
	mui/uisnapshot.h \
	mui/uisound.c \
	mui/uisound.h \
	mui/uivic20mem.c \
	mui/uivic20mem.h \
	mui/uivicii.c \
	mui/uivicii.h \
	mui/uivideoc128.c \
	mui/uivideoc128.h \
	mui/uivideoc64plus4vic20.c \
	mui/uivideoc64plus4vic20.h \
	mui/uivideocbm2pet.c \
	mui/uivideocbm2pet.h

EXTRA_DIST = \
	info-files/VICE.info \
	info-files/x128.exe.info \
	info-files/x64.exe.info \
	info-files/xcbm2.exe.info \
	info-files/xpet.exe.info \
	info-files/xplus4.exe.info \
	info-files/xvic.exe.info \
	info-files/morphos/VICE.info \
	info-files/morphos/x128.info \
	info-files/morphos/x64.info \
	info-files/morphos/xcbm2.info \
	info-files/morphos/xpet.info \
	info-files/morphos/xplus4.info \
	info-files/morphos/xvic.info \
	config.h.vbcc \
	fixdeps.c \
	genintl_h.sh \
	genintltable.sh \
	intl.txt \
	README.AMIGA \
	make-bindist.sh

BUILT_SOURCES = intl.h intl_table.h

intl.h: $(srcdir)/intl.txt
	echo "generating intl.h..."
	$(SHELL) $(srcdir)/genintl_h.sh <$(srcdir)/intl.txt >intl.h

intl_table.h: $(srcdir)/intl.txt
	echo "generating intl_table.h..."
	$(SHELL) $(srcdir)/genintltable.sh <$(srcdir)/intl.txt >intl_table.h
