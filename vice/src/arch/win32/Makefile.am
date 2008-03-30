## Makefile.am for the Win32 version of `libarch.a'.

SUFFIXES = .rc

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	blockdev.c \
	c128ui.c \
	c128ui.h \
	cbm2ui.c \
	cbm2ui.h \
	c64ui.c \
	c64ui.h \
	catweaselmkiii.c \
	console.c \
	dirent.c \
	dirent.h \
	fullscrn.c \
	fullscrn.h \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	lose32.h \
	mouse.c \
	mouse.h \
	petui.c \
	petui.h \
	plus4ui.c \
	plus4ui.h \
	res.h \
	res.rc \
	resc128.rc \
	resc64.rc \
	rescbm2.rc \
	respet.rc \
	resplus4.rc \
	resvic.rc \
	statusbar.c \
	statusbar.h \
	system.c \
	system.h \
	tfearch.c \
	types.h \
	ui.c \
	ui.h \
	uiattach.c \
	uiattach.h \
	uicbm2set.c \
	uicbm2set.h \
	uicmdline.c \
	uicmdline.h \
	uiconsole.c \
	uidatasette.c \
	uidatasette.h \
	uidrivec64c128vic20.c \
	uidrivec64c128vic20.h \
	uidrivepetcbm2.c \
	uidrivepetcbm2.h \
	uidriveplus4.c \
	uidriveplus4.h \
	uijoystick.c \
	uijoystick.h \
	uilib.c \
	uilib.h \
	uimon.c \
	uipetset.c \
	uipetset.h \
	uiplus4mem.c \
	uiplus4mem.h \
	uiram.c \
	uiram.h \
	uireu.c \
	uireu.h \
	uisid.c \
	uisid.h \
	uisnapshot.h \
	uisnapshot.c \
	uisound.c \
	uisound.h \
	uivicii.h \
	uivicii.c \
	uivicset.h \
	uivicset.c \
	uivideo.c \
	uivideo.h \
	vic20ui.c \
	vic20ui.h \
	video.c \
	videoarch.h \
	vsidui.c \
	vsyncarch.c \
	winhack.h \
	winmain.c \
	winmain.h

libarch_a_DEPENDENCIES = \
	resc128.o resc64.o rescbm2.o respet.o resplus4.o resvic.o

EXTRA_DIST = \
	msvc/config.h \
	arch.dsp \
	base.dsp \
	c128.dsp \
	c1541.dsp \
	c64.dsp \
	c64c128.dsp \
	c64cart.dsp \
	cbm2.dsp \
	crtc.dsp \
	diskimage.dsp \
	drive.dsp \
	iec.dsp \
	iecieee.dsp \
	ieee.dsp \
	fsdevice.dsp \
	gfxoutputdrv.dsp \
	imagecontents.dsp \
	monitor.dsp \
	pet.dsp \
	petcat.dsp \
	plus4.dsp \
	printerdrv.dsp \
	raster.dsp \
	resid.dsp \
	rs232drv.dsp \
	serial.dsp \
	sid.dsp \
	sounddrv.dsp \
	tape.dsp \
	tcbm.dsp \
	vdc.dsp \
	vdrive.dsp \
	vic20.dsp \
	vice.dsp \
	vicii.dsp \
	video.dsp \
	x128.dsp \
	x64.dsp \
	xcbm2.dsp \
	xpet.dsp \
	xplus4.dsp \
	xvic.dsp \
	vice.dsw \
        montb.bmp \
        c64.ico 

resc128.o : resc128.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resc64.o : resc64.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
rescbm2.o : rescbm2.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
respet.o : respet.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resplus4.o : resplus4.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resvic.o : resvic.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
