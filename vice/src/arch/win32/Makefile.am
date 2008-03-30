## Makefile.am for the Win32 version of `libarch.a'.

if !CROSS
SUBDIRS = utils
endif

SUFFIXES = .rc

INCLUDES = \
	@ARCH_INCLUDES@ \
	@ARCH_INCLUDES@/msvc/wpcap \
	@ARCH_INCLUDES@/mingw32 \
	@FFMPEG_INCLUDES@ \
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
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/gfxoutputdrv

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	mingw32/system.h \
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
	gifdrv.c \
	hardsid.c \
	intl.c \
	intl.h \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
	mousedrv.c \
	mousedrv.h \
	opencbm.h \
	opencbmlib.c \
	parsid.c \
	petui.c \
	plus4ui.c \
	res.rc \
	resacia.rc \
	resc128.rc \
	resc64.rc \
	resc64_256k.rc \
	rescbm2.rc \
	rescommon.h \
	resdigimax.rc \
	resdrivec128.rc \
	resdrivec64vic20.rc \
	resdrivepetcbm2.rc \
	resdriveplus4.rc \
	resgeoram.rc \
	reside64.rc \
	resmmc64.rc \
	respet.rc \
	respetreu.rc \
 	resplus256k.rc \
	resplus4.rc \
	resplus60k.rc \
	resramcart.rc \
	resreu.rc \
	resrs232user.rc \
	ressid.rc \
	ressidcart.rc \
	restfe.rc \
	resvic20.rc \
	resvicii.rc \
	rs232.c \
	signals.c \
	statusbar.c \
	statusbar.h \
	system.c \
	tfearch.c \
	types.h \
	ui-resources.c \
	ui.c \
	ui.h \
	uiacia.c \
	uiacia.h \
	uiattach.c \
	uiattach.h \
	uic128.c \
	uic128.h \
	uic64_256k.c \
	uic64_256k.h \
	uic64cart.c \
	uic64cart.h \
	uicart.c \
	uicart.h \
	uicbm2set.c \
	uicbm2set.h \
	uicmdline.c \
	uiconsole.c \
	uidatasette.c \
	uidatasette.h \
	uidigimax.c \
	uidigimax.h \
	uidrivec64vic20.c \
	uidrivec64vic20.h \
	uidrivec128.c \
	uidrivec128.h \
	uidrivepetcbm2.c \
	uidrivepetcbm2.h \
	uidriveplus4.c \
	uidriveplus4.h \
	uievent.c \
	uievent.h \
	uifliplist.c \
	uifliplist.h \
	uigeoram.c \
	uigeoram.h \
	uihelp.c \
	uihelp.h \
	uiide64.c \
	uiide64.h \
	uijoystick.c \
	uijoystick.h \
	uikeyboard.c \
	uikeyboard.h \
	uilib.c \
	uilib.h \
	uimediafile.c \
	uimediafile.h \
	uimmc64.c \
	uimmc64.h \
	uimon.c \
	uimonmsg.h \
	uinetwork.c \
	uinetwork.h \
	uiperipheral.c \
	uiperipheral.h \
	uipetreu.c \
	uipetreu.h \
	uipetset.c \
	uipetset.h \
	uiplus256k.c \
	uiplus256k.h \
	uiplus4cart.c \
	uiplus4cart.h \
	uiplus4mem.c \
	uiplus4mem.h \
	uiplus60k.c \
	uiplus60k.h \
	uiquicksnapshot.c \
	uiquicksnapshot.h \
	uiram.c \
	uiram.h \
	uiramcart.c \
	uiramcart.h \
	uireu.c \
	uireu.h \
	uirom.c \
	uirom.h \
	uirs232.c \
	uirs232.h \
	uirs232user.c \
	uirs232user.h \
	uisid.c \
	uisid.h \
	uisidcart.c \
	uisidcart.h \
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
	winmain.c \
	winmain.h

libarch_a_DEPENDENCIES = \
	resc128.o resc64.o rescbm2.o respet.o resplus4.o resvic20.o

EXTRA_DIST = \
	evc/config.h \
	evc/system.c \
	evc/system.h \
	msvc/config.h \
	msvc/system.h \
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
	c64exp.dsp \
	cartconv.dsp \
	cbm2.dsp \
	core.dsp \
	crtc.dsp \
	diskimage.dsp \
	drive.dsp \
	fileio.dsp \
	fsdevice.dsp \
        genmsvcver.dsp \
	gentranslate.dsp \
	genwinres.dsp \
	genwinres.sh \
	gfxoutputdrv.dsp \
	iec.dsp \
	iec128dcr.dsp \
	iecbus.dsp \
	iecieee.dsp \
	ieee.dsp \
	imagecontents.dsp \
	monitor.dsp \
	parallel.dsp \
	pet.dsp \
	petcat.dsp \
	plus4.dsp \
	plus4exp.dsp \
	png.dsp \
	printerdrv.dsp \
	raster.dsp \
	resid.dsp \
	res.txt \
	resdialogs.txt \
	resmenu.txt \
	resstrings.txt \
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
	zlib.dsp \
	vice.dsw \
	vice.manifest \
	montb.bmp \
	make-bindist.sh \
	c64.ico 

BUILT_SOURCES = res.h

res.h:	$(srcdir)/res.txt $(srcdir)/resdialogs.txt $(srcdir)/resmenu.txt $(srcdir)/resstrings.txt
	echo "generating res.h..."
@CROSS_TRUE@	cat >resall.txt $(srcdir)/res.txt $(srcdir)/resdialogs.txt $(srcdir)/resmenu.txt $(srcdir)/resstrings.txt
@CROSS_TRUE@	. $(srcdir)/genwinres.sh <resall.txt >res.h
@CROSS_TRUE@	rm -f resall.txt
@CROSS_FALSE@	(cd utils; $(MAKE))
@CROSS_FALSE@	utils/genwinres res.h $(srcdir)/res.txt $(srcdir)/resdialogs.txt $(srcdir)/resmenu.txt $(srcdir)/resstrings.txt

resc128.o : $(srcdir)/resc128.rc $(srcdir)/res.rc res.h $(srcdir)/resacia.rc \
	$(srcdir)/resrs232user.rc $(srcdir)/ressid.rc $(srcdir)/resvicii.rc \
	$(srcdir)/resreu.rc $(srcdir)/resgeoram.rc $(srcdir)/resramcart.rc \
	$(srcdir)/resmmc64.rc $(srcdir)/reside64.rc $(srcdir)/resdrivec128.rc \
	$(srcdir)/resdigimax.rc $(srcdir)/restfe.rc
	cat >resc128cat.rc $(srcdir)/resc128.rc $(srcdir)/res.rc \
	$(srcdir)/resacia.rc $(srcdir)/reside64.rc $(srcdir)/resreu.rc \
	$(srcdir)/resgeoram.rc $(srcdir)/resramcart.rc $(srcdir)/resmmc64.rc \
	$(srcdir)/resrs232user.rc $(srcdir)/ressid.rc $(srcdir)/resvicii.rc \
	$(srcdir)/resdrivec128.rc $(srcdir)/resdigimax.rc $(srcdir)/restfe.rc
	$(WRC) -i resc128cat.rc -o resc128.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32 \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i resc128.res -o $@
	rm -f resc128.res resc128cat.rc

resc64.o : $(srcdir)/resc64.rc $(srcdir)/res.rc res.h $(srcdir)/resacia.rc \
	$(srcdir)/resrs232user.rc $(srcdir)/ressid.rc $(srcdir)/resvicii.rc \
	$(srcdir)/resreu.rc $(srcdir)/resgeoram.rc $(srcdir)/resramcart.rc \
	$(srcdir)/resmmc64.rc $(srcdir)/resplus256k.rc $(srcdir)/resplus60k.rc \
	$(srcdir)/resc64_256k.rc $(srcdir)/reside64.rc $(srcdir)/resdrivec64vic20.rc \
	$(srcdir)/resdigimax.rc $(srcdir)/restfe.rc
	cat >resc64cat.rc $(srcdir)/resc64.rc $(srcdir)/res.rc \
	$(srcdir)/resacia.rc $(srcdir)/resdrivec64vic20.rc \
	$(srcdir)/reside64.rc $(srcdir)/resreu.rc $(srcdir)/resgeoram.rc \
	$(srcdir)/resmmc64.rc $(srcdir)/resramcart.rc $(srcdir)/resplus256k.rc \
	$(srcdir)/resplus60k.rc $(srcdir)/resc64_256k.rc $(srcdir)/resrs232user.rc \
	$(srcdir)/ressid.rc $(srcdir)/resvicii.rc $(srcdir)/resdigimax.rc \
	$(srcdir)/restfe.rc
	$(WRC) -i resc64cat.rc -o resc64.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32  \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i resc64.res -o $@
	rm -f resc64.res resc64cat.rc

rescbm2.o : $(srcdir)/rescbm2.rc $(srcdir)/res.rc res.h $(srcdir)/resacia.rc \
	$(srcdir)/ressid.rc $(srcdir)/resdrivepetcbm2.rc $(srcdir)/resvicii.rc
	cat >rescbm2cat.rc $(srcdir)/rescbm2.rc $(srcdir)/res.rc \
	$(srcdir)/resacia.rc $(srcdir)/resdrivepetcbm2.rc $(srcdir)/ressid.rc \
	$(srcdir)/resvicii.rc
	$(WRC) -i rescbm2cat.rc -o rescbm2.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32 \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i rescbm2.res -o $@
	rm -f rescbm2.res rescbm2cat.rc

respet.o : $(srcdir)/respet.rc $(srcdir)/res.rc res.h $(srcdir)/resacia.rc \
	$(srcdir)/resdrivepetcbm2.rc $(srcdir)/respetreu.rc $(srcdir)/ressidcart.rc
	cat >respetcat.rc $(srcdir)/respet.rc $(srcdir)/res.rc \
	$(srcdir)/resacia.rc $(srcdir)/resdrivepetcbm2.rc $(srcdir)/respetreu.rc \
	$(srcdir)/ressidcart.rc
	$(WRC) -i respetcat.rc -o respet.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32 \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i respet.res -o $@
	rm -f respet.res respetcat.rc

resplus4.o : $(srcdir)/resplus4.rc $(srcdir)/res.rc res.h \
	$(srcdir)/resacia.rc $(srcdir)/resdriveplus4.rc \
	$(srcdir)/ressidcart.rc
	cat >resplus4cat.rc $(srcdir)/resplus4.rc $(srcdir)/res.rc \
	$(srcdir)/resacia.rc $(srcdir)/resdriveplus4.rc \
	$(srcdir)/ressidcart.rc
	$(WRC) -i resplus4cat.rc -o resplus4.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32 \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i resplus4.res -o $@
	rm -f resplus4.res resplus4cat.rc

resvic20.o : $(srcdir)/resvic20.rc $(srcdir)/res.rc res.h \
	$(srcdir)/resrs232user.rc $(srcdir)/resdrivec64vic20.rc \
	$(srcdir)/ressidcart.rc
	cat >resvic20cat.rc $(srcdir)/resvic20.rc $(srcdir)/res.rc \
	$(srcdir)/resdrivec64vic20.rc $(srcdir)/resrs232user.rc \
	$(srcdir)/ressidcart.rc
	$(WRC) -i resvic20cat.rc -o resvic20.res \
	--include-dir $(top_builddir)/src \
	--include-dir $(top_srcdir)/src/arch/win32 \
	--include-dir $(top_srcdir)/src
	$(WINDRES) -i resvic20.res -o $@
	rm -f resvic20.res resvic20cat.rc

.PHONY: utils
utils:
	(cd utils; $(MAKE))
