## Makefile.am for the Win32 version of `libarch.a'.

SUFFIXES = .rc

INCLUDES = \
	@ARCH_INCLUDES@ \
	@ARCH_INCLUDES@/msvc/wpcap \
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
	c64ui.c \
	catweaselmkiii.c \
	cbm2ui.c \
	console.c \
	dirent.c \
	dirent.h \
	ffmpeglib.c \
	fullscrn.c \
	fullscrn.h \
	hardsid.c \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	lose32.h \
	mousedrv.c \
	mousedrv.h \
	petui.c \
	plus4ui.c \
	res.h \
	res.rc \
	resc128.rc \
	resc64.rc \
	rescbm2.rc \
	respet.rc \
	resplus4.rc \
	resreu.rc \
	ressid.rc \
	restfe.rc \
	resvic.rc \
	resvicii.rc \
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
	uic128.c \
	uic128.h \
	uicbm2set.c \
	uicbm2set.h \
	uicmdline.c \
	uiconsole.c \
	uidatasette.c \
	uidatasette.h \
	uidrivec64vic20.c \
	uidrivec64vic20.h \
	uidrivec128.c \
	uidrivec128.h \
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
	uiprinter.c \
	uiprinter.h \
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
	uispeed.c \
	uispeed.h \
	uitfe.c \
	uitfe.h \
	uivicii.h \
	uivicii.c \
	uivicset.h \
	uivicset.c \
	uivideo.c \
	uivideo.h \
	vic20ui.c \
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
	msvc/wpcap/bittypes.h \
	msvc/wpcap/ip6_misc.h \
	msvc/wpcap/pcap-stdinc.h \
	msvc/wpcap/pcap.h \
	msvc/wpcap/net/bpf.h \
	arch.dsp \
	base.dsp \
	c128.dsp \
	c1541.dsp \
	c64.dsp \
	c64c128.dsp \
	c64cart.dsp \
	cbm2.dsp \
	core.dsp \
	crtc.dsp \
	diskimage.dsp \
	drive.dsp \
	iec.dsp \
	iecbus.dsp \
	iecieee.dsp \
	ieee.dsp \
	fsdevice.dsp \
	gfxoutputdrv.dsp \
	imagecontents.dsp \
	monitor.dsp \
	parallel.dsp \
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

resc128.o : resc128.rc res.rc res.h ressid.rc resvicii.rc resreu.rc
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resc64.o : resc64.rc res.rc res.h ressid.rc resvicii.rc resreu.rc
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
rescbm2.o : rescbm2.rc res.rc res.h ressid.rc
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
