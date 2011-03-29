# We have to override the automake default, because we need to use
# $(CXX) instead of $(CC) when compiling with reSID.
LINK = @LINKCC@ $(CFLAGS) $(LDFLAGS) -o $@
CCLD = @LINKCC@

ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = \
	@RESID_DIR@ \
	@RESID_FP_DIR@ \
	@RESID_DTV_DIR@ \
	sounddrv \
	drive \
	vdrive \
	fsdevice \
	diskimage \
	iecbus \
	serial \
	parallel \
	tape \
	imagecontents \
	fileio \
	video \
	raster \
	vicii \
	viciisc \
	vdc \
	crtc \
	gfxoutputdrv \
	printerdrv \
	rs232drv \
	sid \
	monitor \
	core \
	rtc \
	c64 \
	c64dtv \
	c128 \
	vic20 \
	pet \
	plus4 \
	cbm2 \
	arch

DIST_SUBDIRS = \
	sounddrv \
	drive \
	vdrive \
	fsdevice \
	diskimage \
	iecbus \
	serial \
	parallel \
	tape \
	imagecontents \
	fileio \
	video \
	raster \
	vicii \
	viciisc \
	vdc \
	crtc \
	gfxoutputdrv \
	printerdrv \
	rs232drv \
	sid \
	monitor \
	core \
	rtc \
	c64 \
	c64dtv \
	c128 \
	vic20 \
	pet \
	plus4 \
	cbm2 \
	arch \
	resid \
	resid-fp \
	resid-dtv \
	lib

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/arch \
	-I$(top_srcdir)/src/arch/unix/readline \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/imagecontents

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
	archapi.h \
	attach.h \
	autostart.h \
	autostart-prg.h \
	blockdev.h \
	c128ui.h \
	c64ui.h \
	cartridge.h \
	catweaselmkiii.h \
	cbm2ui.h \
	cbmdos.h \
	cbmimage.h \
	charset.h \
	cia.h \
	clkguard.h \
	clipboard.h \
	cmdline.h \
	color.h \
	config.h.in \
	console.h \
	crc32.h \
	datasette.h \
	debug.h \
	diskconstants.h \
	diskimage.h \
	dma.h \
	dynlib.h \
	embedded.h \
	fileio.h \
	findpath.h \
	fixpoint.h \
	fsdevice.h \
	flash040.h \
	fliplist.h \
	fullscreen.h \
	gcr.h \
	gfxoutput.h \
	hardsid.h \
	iecbus.h \
	iecdrive.h \
	imagecontents.h \
	info.h \
	init.h \
	initcmdline.h \
	interrupt.h \
	ioutil.h \
	joystick.h \
	kbdbuf.h \
	keyboard.h \
	lib.h \
	libm_math.h \
	lightpen.h \
	log.h \
	machine-bus.h \
	machine-drive.h \
	machine-printer.h \
	machine-video.h \
	machine.h \
	main.h \
	mainc64cpu.c \
	mainc64cpu.h \
	maincpu.h \
	mainviccpu.c \
	mc6821.h \
	mem.h \
	midi.h \
	mididrv.h \
	monitor.h \
	mos6510.h \
	mos6510dtv.h \
	mouse.h \
	network.h \
	opencbm.h \
	opencbmlib.h \
	palette.h \
	parallel.h \
	parsid.h \
	petui.h \
	piacore.h \
	plus4ui.h \
	printer.h \
	ps2mouse.h \
	ram.h \
	rawfile.h \
	rawnet.h \
	rawnetarch.h \
	resources.h \
	riot.h \
	romset.h \
	rs232drv.h \
	rsuser.h \
	screenshot.h \
	serial.h \
	signals.h \
	snapshot.h \
	sound.h \
	sysfile.h \
	tap.h \
	tape.h \
	tpi.h \
	translate_funcs.h \
	translate_languages.h \
	translate_text.c \
	traps.h \
	uiapi.h \
	uicmdline.h \
	uicolor.h \
	uimon.h \
	usleep.h \
	util.h \
	version.h.in \
	version.h \
	via.h \
	vic20ui.h \
	vice.h \
	vice_sdl.h \
	vice-event.h \
	vicemaxpath.h \
	vicesocket.h \
	vicii.h \
	video.h \
	viewport.h \
	vsidui.h \
	vsync.h \
	vsyncapi.h \
	z80regs.h \
	zfile.h \
	zipcode.h

base_sources = \
	alarm.c \
	attach.c \
	autostart.c \
	autostart-prg.c \
	charset.c \
	clkguard.c \
	clipboard.c \
	cmdline.c \
	cbmdos.c \
	cbmimage.c \
	color.c \
	crc32.c \
	datasette.c \
	debug.c \
	dma.c \
	embedded.c \
	event.c \
	findpath.c \
	fliplist.c \
	gcr.c \
	info.c \
	init.c \
	initcmdline.c \
	interrupt.c \
	ioutil.c \
	joystick.c \
	kbdbuf.c \
	keyboard.c \
	lib.c \
	libm_math.c \
	lightpen.c \
	log.c \
	machine-bus.c \
	machine.c \
	main.c \
	network.c \
	opencbmlib.c \
	palette.c \
	ram.c \
	rawfile.c \
	rawnet.c \
	resources.c \
	romset.c \
	screenshot.c \
	snapshot.c \
	socket.c \
	sound.c \
	sysfile.c \
	translate.c \
	traps.c \
	util.c \
	vsync.c \
	zfile.c \
	zipcode.c

mouse_sources = \
	mouse.c

ps2mouse_sources = \
	ps2mouse.c

midi_sources = \
	midi.c

cpucore_sources = \
	6510core.c

cpudtvcore_sources = \
	6510dtvcore.c

maincpu_sources = \
	maincpu.c

EXTRA_DIST = \
	6510core.c \
	6510dtvcore.c \
	ChangeLog \
	ChangeLog.1 \
	ChangeLog.2 \
	ChangeLog.3 \
	ChangeLog.4 \
	ChangeLog.5 \
	ChangeLog.6 \
	aciacore.c \
	bin2c.c \
	bin2c.sh \
	convert_owcc_libs.sh \
	debug.h.in \
	fixpoint.c \
	geninfocontrib_h.sh \
	gentranslate_h.sh \
	gentranslate_h.sh.in \
	gentranslatetable.sh \
	infocontrib.sed \
	od2c.sh \
	palette2c.sh \
	piacore.c \
	translate.txt

if EMBEDDED
EMBEDDED_H_FILES = \
	c128basic64.h \
	c128basichi.h \
	c128basiclo.h \
	c128chargde.h \
	c128chargen.h \
	c128chargfr.h \
	c128chargse.h \
	c128kernal.h \
	c128kernal64.h \
	c128kernalde.h \
	c128kernalfi.h \
	c128kernalfr.h \
	c128kernalit.h \
	c128kernalno.h \
	c128kernalse.h \
	c64basic.h \
	c64kernal.h \
	c64chargen.h \
	cbm2basic128.h \
	cbm2basic256.h \
	cbm2basic500.h \
	cbm2chargen500.h \
	cbm2chargen600.h \
	cbm2chargen700.h \
	cbm2kernal.h \
	cbm2kernal500.h \
	drived1541II.h \
	drived1571cr.h \
	drivedos1001.h \
	drivedos1541.h \
	drivedos1551.h \
	drivedos1570.h \
	drivedos1571.h \
	drivedos1581.h \
	drivedos2031.h \
	drivedos2040.h \
	drivedos3040.h \
	drivedos4040.h \
	petbasic1.h \
	petbasic2.h \
	petbasic4.h \
	petchargen.h \
	petedit1g.h \
	petedit2b.h \
	petedit2g.h \
	petedit4b40.h \
	petedit4b80.h \
	petedit4g40.h \
	petkernal1.h \
	petkernal2.h \
	petkernal4.h \
	plus43plus1hi.h \
	plus43plus1lo.h \
	plus4basic.h \
	plus4kernal.h \
	printermps803.h \
	printernl10cbm.h \
	vic20basic.h \
	vic20chargen.h \
	vic20kernal.h \
	c128_vdc_comp_vpl.h \
	c128_vdc_deft_vpl.h \
	c64_c64hq_vpl.h \
	c64_c64s_vpl.h \
	c64_ccs64_vpl.h \
	c64_default_vpl.h \
	c64_frodo_vpl.h \
	c64_godot_vpl.h \
	c64_pc64_vpl.h \
	c64_vice_vpl.h \
	pet_amber_vpl.h \
	pet_green_vpl.h \
	pet_white_vpl.h \
	plus4_default_vpl.h \
	vic20_default_vpl.h

if !CROSS
EMBEDDED_DEPENDS = bin2c bin2c.sh
else
EMBEDDED_DEPENDS = bin2c.sh
endif

else
EMBEDDED_H_FILES =
endif

if OWCC
OWCC_LIBS = owcc_libs_converted.h
else
OWCC_LIBS =
endif

if HAVE_TRANSLATION
BUILT_SOURCES = translate.h translate_table.h $(EMBEDDED_H_FILES) $(OWCC_LIBS) infocontrib.h

translate_table.h:	$(srcdir)/translate.txt
	echo "generating translate_table.h..."
	$(SHELL) $(srcdir)/gentranslatetable.sh <$(srcdir)/translate.txt >translate_table.h
else
BUILT_SOURCES = translate.h $(EMBEDDED_H_FILES) $(OWCC_LIBS) infocontrib.h
endif

infocontrib.h:	$(top_srcdir)/doc/vice.texi
	echo "generating infocontrib.h..."
	$(SHELL) $(srcdir)/geninfocontrib_h.sh <$(top_srcdir)/doc/vice.texi | sed -f $(srcdir)/infocontrib.sed >infocontrib.h

translate.h:	$(srcdir)/translate.txt
	echo "generating translate.h..."
if MINIX_SUPPORT
	cc $(srcdir)/arch/win32/utils/gentranslate.c -o gentranslate
	./gentranslate translate.txt translate.h translate_table.h
else
	$(SHELL) $(srcdir)/gentranslate_h.sh <$(srcdir)/translate.txt >translate.h
endif

if OWCC
owcc_libs_converted.h:
	$(SHELL) $(srcdir)/convert_owcc_libs.sh $(LIBS)
endif

bin2c: bin2c.c
	$(CC) $(INCLUDES) $(CFLAGS) $(srcdir)/bin2c.c -o bin2c

c128basic64.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/basic64 c128basic64.h "static BYTE c128basic64_embedded[C128_BASIC64_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128basichi.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/basichi c128basichi.h "static BYTE c128basichi_embedded[C128_BASIC_ROM_IMAGEHI_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128basiclo.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/basiclo c128basiclo.h "static BYTE c128basiclo_embedded[C128_BASIC_ROM_IMAGELO_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128chargde.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/chargde c128chargde.h "static BYTE chargen_de[C128_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128chargen.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/chargen c128chargen.h "static BYTE chargen_int[C128_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128chargfr.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/chargfr c128chargfr.h "static BYTE chargen_fr[C128_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128chargse.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/chargse c128chargse.h "static BYTE chargen_se[C128_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernal.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernal c128kernal.h "static BYTE kernal_int[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernal64.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernal64 c128kernal64.h "static BYTE c128kernal64_embedded[C128_KERNAL64_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalde.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalde c128kernalde.h "static BYTE kernal_de[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalfi.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalfi c128kernalfi.h "static BYTE kernal_fi[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalfr.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalfr c128kernalfr.h "static BYTE kernal_fr[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalit.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalit c128kernalit.h "static BYTE kernal_it[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalno.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalno c128kernalno.h "static BYTE kernal_no[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128kernalse.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C128/kernalse c128kernalse.h "static BYTE kernal_se[C128_KERNAL_ROM_IMAGE_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c64basic.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C64/basic c64basic.h "BYTE c64memrom_basic64_rom[C64_BASIC_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c64kernal.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C64/kernal c64kernal.h "BYTE c64memrom_kernal64_rom[C64_KERNAL_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c64chargen.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/C64/chargen c64chargen.h "BYTE mem_chargen_rom[C64_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2basic128.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/basic.128 cbm2basic128.h "static BYTE cbm2basic128_embedded[0x4000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2basic256.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/basic.256 cbm2basic256.h "static BYTE cbm2basic256_embedded[0x4000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2basic500.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/basic.500 cbm2basic500.h "static BYTE cbm2basic500_embedded[0x4000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2chargen500.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/chargen.500 cbm2chargen500.h "static BYTE cbm2chargen500_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2chargen600.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/chargen.600 cbm2chargen600.h "static BYTE cbm2chargen600_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2chargen700.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/chargen.700 cbm2chargen700.h "static BYTE cbm2chargen700_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2kernal.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/kernal cbm2kernal.h "static BYTE cbm2kernal_embedded[0x2000]" $(CROSS) $(SHELL) $(top_srcdir)

cbm2kernal500.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/CBM-II/kernal.500 cbm2kernal500.h "static BYTE cbm2kernal500_embedded[0x2000]" $(CROSS) $(SHELL) $(top_srcdir)

drived1541II.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/d1541II drived1541II.h "static BYTE drive_rom1541ii_embedded[DRIVE_ROM1541II_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drived1571cr.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/d1571cr drived1571cr.h "static BYTE drive_rom1571cr[DRIVE_ROM1571_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos1001.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1001 drivedos1001.h "static BYTE drive_rom1001[DRIVE_ROM1001_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos1541.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1541 drivedos1541.h "static BYTE drive_rom1541_embedded[DRIVE_ROM1541_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos1551.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1551 drivedos1551.h "static BYTE drive_rom1551[DRIVE_ROM1551_SIZE]" $(CROSS) $(SHELL)  $(top_srcdir)

drivedos1570.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1570 drivedos1570.h "static BYTE drive_rom1570[DRIVE_ROM1571_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos1571.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1571 drivedos1571.h "static BYTE drive_rom1571[DRIVE_ROM1571_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos1581.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos1581 drivedos1581.h "static BYTE drive_rom1581[DRIVE_ROM1581_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos2031.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos2031 drivedos2031.h "static BYTE drive_rom2031[DRIVE_ROM2031_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos2040.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos2040 drivedos2040.h "static BYTE drive_rom2040[DRIVE_ROM2040_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos3040.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos3040 drivedos3040.h "static BYTE drive_rom3040[DRIVE_ROM3040_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

drivedos4040.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/DRIVES/dos4040 drivedos4040.h "static BYTE drive_rom4040[DRIVE_ROM4040_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

petbasic1.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/basic1 petbasic1.h "BYTE petbasic1_embedded[0x2000]" $(CROSS) $(SHELL) $(top_srcdir)

petbasic2.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/basic2 petbasic2.h "BYTE petbasic2_embedded[0x2000]" $(CROSS) $(SHELL) $(top_srcdir)

petbasic4.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/basic4 petbasic4.h "BYTE petbasic4_embedded[0x3000]" $(CROSS) $(SHELL) $(top_srcdir)

petchargen.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/chargen petchargen.h "BYTE petchargen_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit1g.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit1g petedit1g.h "BYTE petedit1g_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit2b.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit2b petedit2b.h "BYTE petedit2b_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit2g.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit2g petedit2g.h "BYTE petedit2g_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit4b40.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit4b40 petedit4b40.h "BYTE petedit4b40_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit4b80.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit4b80 petedit4b80.h "BYTE petedit4b80_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petedit4g40.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/edit4g40 petedit4g40.h "BYTE petedit4g40_embedded[0x800]" $(CROSS) $(SHELL) $(top_srcdir)

petkernal1.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/kernal1 petkernal1.h "BYTE petkernal1_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

petkernal2.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/kernal2 petkernal2.h "BYTE petkernal2_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

petkernal4.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PET/kernal4 petkernal4.h "BYTE petkernal4_embedded[0x1000]" $(CROSS) $(SHELL) $(top_srcdir)

plus43plus1hi.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PLUS4/3plus1hi plus43plus1hi.h "BYTE extromhi1[PLUS4_KERNAL_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

plus43plus1lo.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PLUS4/3plus1lo plus43plus1lo.h "BYTE extromlo1[PLUS4_BASIC_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

plus4basic.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PLUS4/basic plus4basic.h "BYTE plus4memrom_basic_rom[PLUS4_BASIC_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

plus4kernal.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PLUS4/kernal plus4kernal.h "BYTE plus4memrom_kernal_rom[PLUS4_KERNAL_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

printermps803.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PRINTER/mps803 printermps803.h "static BYTE charset[512][7]" $(CROSS) $(SHELL) $(top_srcdir)

printernl10cbm.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/PRINTER/nl10-cbm printernl10cbm.h "static BYTE  drv_nl10_rom[NL10_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

vic20basic.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/VIC20/basic vic20basic.h "BYTE vic20memrom_basic_rom[VIC20_BASIC_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

vic20chargen.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/VIC20/chargen vic20chargen.h "BYTE vic20chargen_embedded[VIC20_CHARGEN_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

vic20kernal.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/bin2c.sh $(top_srcdir)/data/VIC20/kernal vic20kernal.h "BYTE vic20memrom_kernal_rom[VIC20_KERNAL_ROM_SIZE]" $(CROSS) $(SHELL) $(top_srcdir)

c128_vdc_comp_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C128/vdc_comp.vpl c128_vdc_comp_vpl.h "unsigned char c128_vdc_comp_vpl[]"

c128_vdc_deft_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C128/vdc_deft.vpl c128_vdc_deft_vpl.h "unsigned char c128_vdc_deft_vpl[]"

c64_c64hq_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/c64hq.vpl c64_c64hq_vpl.h "unsigned char c64_c64hq_vpl[]"

c64_c64s_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/c64s.vpl c64_c64s_vpl.h "unsigned char c64_c64s_vpl[]"

c64_ccs64_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/ccs64.vpl c64_ccs64_vpl.h "unsigned char c64_ccs64_vpl[]"

c64_default_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/default.vpl c64_default_vpl.h "unsigned char c64_default_vpl[]"

c64_frodo_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/frodo.vpl c64_frodo_vpl.h "unsigned char c64_frodo_vpl[]"

c64_godot_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/godot.vpl c64_godot_vpl.h "unsigned char c64_godot_vpl[]"

c64_pc64_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/pc64.vpl c64_pc64_vpl.h "unsigned char c64_pc64_vpl[]"

c64_vice_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/C64/vice.vpl c64_vice_vpl.h "unsigned char c64_vice_vpl[]"

pet_amber_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/PET/amber.vpl pet_amber_vpl.h "unsigned char pet_amber_vpl[]"

pet_green_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/PET/green.vpl pet_green_vpl.h "unsigned char pet_green_vpl[]"

pet_white_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/PET/white.vpl pet_white_vpl.h "unsigned char pet_white_vpl[]"

plus4_default_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/PLUS4/default.vpl plus4_default_vpl.h "unsigned char plus4_default_vpl[]"

vic20_default_vpl.h: $(EMBEDDED_DEPENDS)
	$(SHELL) $(srcdir)/palette2c.sh <$(top_srcdir)/data/VIC20/default.vpl vic20_default_vpl.h "unsigned char vic20_default_vpl[]"

c128_lib = $(top_builddir)/src/c128/libc128.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64sc_lib = $(top_builddir)/src/c64/libc64sc.a
c64dtv_lib = $(top_builddir)/src/c64dtv/libc64dtv.a
c64dtvstubs_lib = $(top_builddir)/src/c64dtv/libc64dtvstubs.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
c64c64dtv_lib = $(top_builddir)/src/c64/libc64c64dtv.a
c64cart_lib = $(top_builddir)/src/c64/cart/libc64cart.a
core_lib = $(top_builddir)/src/core/libcore.a
crtc_lib = $(top_builddir)/src/crtc/libcrtc.a
diskimage_lib = $(top_builddir)/src/diskimage/libdiskimage.a
drive_lib = $(top_builddir)/src/drive/libdrive.a
driveiec_lib = $(top_builddir)/src/drive/iec/libdriveiec.a
driveiec128dcr_lib = $(top_builddir)/src/drive/iec128dcr/libdriveiec128dcr.a
driveiecieee_lib = $(top_builddir)/src/drive/iecieee/libdriveiecieee.a
driveiecc64exp_lib = $(top_builddir)/src/drive/iec/c64exp/libdriveiecc64exp.a
driveiecplus4exp_lib = $(top_builddir)/src/drive/iec/plus4exp/libdriveiecplus4exp.a
driveieee_lib = $(top_builddir)/src/drive/ieee/libdriveieee.a
drivetcbm_lib = $(top_builddir)/src/drive/tcbm/libdrivetcbm.a
fsdevice_lib = $(top_builddir)/src/fsdevice/libfsdevice.a
fileio_lib = $(top_builddir)/src/fileio/libfileio.a
gfxoutputdrv_lib = $(top_builddir)/src/gfxoutputdrv/libgfxoutputdrv.a
iecbus_lib = $(top_builddir)/src/iecbus/libiecbus.a
imagecontents_lib = $(top_builddir)/src/imagecontents/libimagecontents.a
monitor_lib = $(top_builddir)/src/monitor/libmonitor.a
parallel_lib = $(top_builddir)/src/parallel/libparallel.a
pet_lib = $(top_builddir)/src/pet/libpet.a
plus4_lib = $(top_builddir)/src/plus4/libplus4.a
printerdrv_lib = $(top_builddir)/src/printerdrv/libprinterdrv.a
rs232drv_lib = $(top_builddir)/src/rs232drv/librs232drv.a
raster_lib = $(top_builddir)/src/raster/libraster.a
rtc_lib = $(top_builddir)/src/rtc/librtc.a
serial_lib = $(top_builddir)/src/serial/libserial.a
sid_lib = $(top_builddir)/src/sid/libsid.a
sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
tape_lib = $(top_builddir)/src/tape/libtape.a
vdc_lib = $(top_builddir)/src/vdc/libvdc.a
vdrive_lib = $(top_builddir)/src/vdrive/libvdrive.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
vic20cart_lib = $(top_builddir)/src/vic20/cart/libvic20cart.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
viciidtv_lib = $(top_builddir)/src/vicii/libviciidtv.a
viciisc_lib = $(top_builddir)/src/viciisc/libviciisc.a
video_lib = $(top_builddir)/src/video/libvideo.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a

common_libs = @ARCH_LIBS@ @LIBOBJS@ @READLINE@
resid_libs = @RESID_LIBS@ @RESID_FP_LIBS@
resid_dtv_libs = @RESID_DTV_LIBS@ @RESID_FP_LIBS@

# external libraries required for all emulators
emu_extlibs = @X_LIBS@ @UI_LIBS@ @SDL_EXTRA_LIBS@ @INTLLIBS@ @SOUND_LIBS@ @FFMPEG_LIBS@ @QUICKTIME_LIBS@ @JOY_LIBS@ @READLINE_LIBS@ @GFXOUTPUT_LIBS@ @ZLIB_LIBS@ @DYNLIB_LIBS@

driver_libs = $(sounddrv_lib) $(gfxoutputdrv_lib) $(printerdrv_lib) $(rs232drv_lib) $(diskimage_lib) $(fsdevice_lib) $(tape_lib) $(imagecontents_lib) $(fileio_lib) $(serial_lib) $(core_lib)

if HAVE_RAWDRIVE
blockdev_lib = @ARCH_DIR@/blockdev.o
else
blockdev_lib =
endif

if HAVE_REALDEVICE
realdevice_lib = opencbmlib.o @ARCH_DIR@/dynlib.o
else
realdevice_lib =
endif

if SUPPORT_X64DTV
x64dtv_owbin = x64dtvow
x64dtv_bin = x64dtv
else
x64dtv_owbin =
x64dtv_bin =
endif

if SUPPORT_X64SC
x64sc_owbin = x64scow
x64sc_bin = x64sc
else
x64sc_owbin =
x64sc_bin =
endif

if SUPPORT_CMDLINE_TOOLS
c1541 = c1541
petcat = petcat
cartconv = cartconv
else
c1541 =
petcat =
cartconv =
endif

# workaround for extra exe creation
x128ow_SOURCES = maincpu.c
x64ow_SOURCES = maincpu.c
x64dtvow_SOURCES = maincpu.c
x64scow_SOURCES = maincpu.c
xcbm2ow_SOURCES = maincpu.c
xpetow_SOURCES = maincpu.c
xplus4ow_SOURCES = maincpu.c
xvicow_SOURCES = maincpu.c

if OWCC
OW_progs = x64ow $(x64sc_owbin) x128ow $(x64dtv_owbin) xvicow xpetow xplus4ow xcbm2ow
else
OW_progs =
endif

bin_PROGRAMS = x64 $(x64sc_bin) x128 $(x64dtv_bin) xvic xpet xplus4 xcbm2 $(c1541) $(petcat) $(cartconv) $(OW_progs)

EXTRA_PROGRAMS =

# x64
x64_libs =  \
	@X64_WINRES@ \
	$(c64_lib) \
	$(c64cart_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(vicii_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

x64_SOURCES = $(base_sources) $(maincpu_sources) $(mouse_sources) $(midi_sources)
x64_LDADD = $(x64_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
x64_DEPENDENCIES = $(x64_libs)
x64_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64-all
x64-all: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	@RESID_FP_DEP@ \
	libc64 \
	libc64cart \
	libdriveiec \
	libdriveiecieee \
	libdriveiecc64exp \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libsid \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libvicii \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	librtc \
	x64$(EXEEXT)

# x64sc
x64sc_libs =  \
	@X64_WINRES@ \
	$(c64sc_lib) \
	$(c64cart_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(viciisc_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

x64sc_SOURCES = $(base_sources) $(mouse_sources) $(midi_sources)
x64sc_LDADD = $(x64sc_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
x64sc_DEPENDENCIES = $(x64sc_libs)
x64sc_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64sc-all
x64sc-all: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	@RESID_FP_DEP@ \
	libc64sc \
	libc64cart \
	libdriveiec \
	libdriveiecieee \
	libdriveiecc64exp \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libsid \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libviciisc \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	x64sc$(EXEEXT)

# x64dtv
x64dtv_libs =  \
	@X64DTV_WINRES@ \
	$(c64dtv_lib) \
	$(c64c64dtv_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(viciidtv_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_dtv_libs) \
	$(c64dtvstubs_lib)

x64dtv_SOURCES = $(base_sources) $(ps2mouse_sources)
x64dtv_LDADD = $(x64dtv_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
x64dtv_DEPENDENCIES = $(x64dtv_libs)
x64dtv_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64dtv-all
x64dtv-all: \
	$(BUILT_SOURCES) \
	@RESID_DTV_DEP@ \
	@RESID_FP_DEP@ \
	libc64dtv \
	libc64dtvstubs \
	libc64c64dtv \
	libdriveiec \
	libdriveiecieee \
	libdriveiecc64exp \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libsid \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libviciidtv \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	x64dtv$(EXEEXT)

# x128
x128_libs =  \
	@X128_WINRES@ \
	$(c128_lib) \
	$(c64cart_lib) \
	$(c64c128_lib) \
	$(driveiec_lib) \
	$(driveiec128dcr_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(vicii_lib) \
	$(vdc_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

x128_SOURCES = $(base_sources) $(mouse_sources) $(midi_sources)
x128_LDADD = $(x128_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
x128_DEPENDENCIES = $(x128_libs)
x128_LDFLAGS = @x128_LDFLAGS@

.PHONY: x128-all
x128-all: \
	$(BUILT_SOURCES) \
	libc128 \
	libc64c128 \
	libc64cart \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libdriveiec \
	libdriveiec128dcr \
	libdriveiecieee \
	libdriveiecc64exp \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libarch \
	libsid \
	libvicii \
	libvdc \
	libraster \
	libvideo \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	librtc \
	@RESID_DEP@ \
	@RESID_FP_DEP@ \
	x128$(EXEEXT)

# xvic
xvic_libs =  \
	@XVIC_WINRES@ \
	$(vic20_lib) \
	$(vic20cart_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(raster_lib) \
	$(rtc_lib) \
	$(sid_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

xvic_SOURCES = $(base_sources) $(mouse_sources) $(midi_sources)
xvic_LDADD = $(xvic_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
xvic_DEPENDENCIES = $(xvic_libs)
xvic_LDFLAGS = @xvic_LDFLAGS@

.PHONY: xvic-all
xvic-all: \
	$(BUILT_SOURCES) \
	libdriveiec \
	libdriveiecieee \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libvic20 \
	libvic20cart \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libsid \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	librtc \
	xvic$(EXEEXT)

# xpet
xpet_libs =  \
	@XPET_WINRES@ \
	$(pet_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(crtc_lib) \
	$(rtc_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(sid_lib) \
	$(common_libs) \
	$(resid_libs)

xpet_SOURCES = $(base_sources) $(maincpu_sources) $(mouse_sources)
xpet_LDADD = $(xpet_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
xpet_DEPENDENCIES = $(xpet_libs) 
xpet_LDFLAGS = @xpet_LDFLAGS@

.PHONY: xpet-all
xpet-all: \
	$(BUILT_SOURCES) \
	libdriveiecieee \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libcrtc \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libpet \
	libvideo \
	libsid \
	libarch \
	libmonitor \
	libdiskimage \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	librtc \
	xpet$(EXEEXT)

# xplus4
xplus4_libs =  \
	@XPLUS4_WINRES@ \
	$(plus4_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecplus4exp_lib) \
	$(drivetcbm_lib) \
	$(drive_lib) \
	$(iecbus_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(sid_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

xplus4_SOURCES = $(base_sources) $(mouse_sources)
xplus4_LDADD = $(xplus4_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
xplus4_DEPENDENCIES = $(xplus4_libs)
xplus4_LDFLAGS = @xplus4_LDFLAGS@

.PHONY: xplus4-all
xplus4-all: \
	$(BUILT_SOURCES) \
	libdriveiec \
	libdriveiecieee \
	libdriveiecplus4exp \
	libdrivetcbm \
	libdrive \
	libvdrive \
	libraster \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libplus4 \
	libvideo \
	libsid \
	libarch \
	libmonitor \
	libdiskimage \
	libiecbus \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	librtc \
	xplus4$(EXEEXT)

# xcbm2
xcbm2_libs = \
	@XCBM2_WINRES@ \
	$(xcbm2_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(sid_lib) \
	$(crtc_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

xcbm2_SOURCES = $(base_sources) $(mouse_sources)
xcbm2_LDADD = $(xcbm2_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@
xcbm2_DEPENDENCIES = $(xcbm2_libs) 
xcbm2_LDFLAGS = @xcbm2_LDFLAGS@

.PHONY: xcbm2-all
xcbm2-all: \
	$(BUILT_SOURCES) \
	libcbm2 \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	librs232drv \
	libarch \
	libsid \
	@RESID_DEP@ \
	@RESID_FP_DEP@ \
	libdriveiecieee \
	libdriveieee \
	libdrive \
	libparallel \
	libvdrive \
	libraster \
	libcrtc \
	libvicii \
	libvideo \
	libmonitor \
	libdiskimage \
	libserial \
	libfsdevice \
	libtape \
	libimagecontents \
	libfileio \
	libcore \
	xcbm2$(EXEEXT)

# c1541
c1541_SOURCES = \
	c1541.c \
	cbmdos.c \
	charset.c \
	findpath.c \
	gcr.c \
	cbmimage.c \
	info.c \
	ioutil.c \
	lib.c \
	log.c \
	rawfile.c \
	resources.c \
	util.c \
	zfile.c \
	zipcode.c

if OWCC
OWCC_SYSTEM = @ARCH_DIR@/system.o
else
OWCC_SYSTEM =
endif

# FIXME: I don't like linking `archdep.o' this way, but we cannot use
# the generic `ARCH_LIBS', because we would have to link against X11
# libs too then.
c1541_LDADD = \
	@ARCH_DIR@/archdep.o \
	$(OWCC_SYSTEM) \
	$(blockdev_lib) \
	$(realdevice_lib) \
	$(vdrive_lib) \
	$(imagecontents_lib) \
	$(fileio_lib) \
	$(diskimage_lib) \
	$(tape_lib) \
	$(serial_lib) \
	@INTLLIBS@ @SDL_EXTRA_LIBS@ \
	@READLINE@ @READLINE_LIBS@ \
	@ZLIB_LIBS@ @DYNLIB_LIBS@

# petcat
petcat_SOURCES = \
	charset.c \
	findpath.c \
	ioutil.c \
	lib.c \
	log.c \
	petcat.c \
	rawfile.c \
	resources.c \
	util.c \
	zfile.c \
	zipcode.c

petcat_LDADD = \
	@ARCH_DIR@/archdep.o \
	$(OWCC_SYSTEM) \
	$(fileio_lib) \
	@INTLLIBS@ @SDL_EXTRA_LIBS@ \
	@ZLIB_LIBS@

# cartconv
cartconv_SOURCES = cartconv.c
cartconv_LDADD = @INTLLIBS@

# Special targets to build libraries in subdirectories.

.PHONY: libc64 libc64sc libc128 libc64dtv libc64dtvstubs libvic20 libpet libplus4 libcbm2 libdrive libvdrive
	libprinterdrv librs232drv libgfxoutputdrv libsounddrv libdiskimage
	libfsdevice libtape libarch libvicii libviciisc libviciidtv libvdc libsid libcrtc libvideo
	libserial libparallel libcore libiecbus libfileio @RESID_DEP@ @RESID_DTV_DEP@ @RESID_FP_DEP@

libarch:
	(cd arch && $(MAKE))
libc128:
	(cd c128 && $(MAKE))
libc64:
	(cd c64 && $(MAKE) libc64.a)
libc64sc:
	(cd c64 && $(MAKE) libc64sc.a)
libc64c128:
	(cd c64 && $(MAKE) libc64c128.a)
libc64c64dtv:
	(cd c64 && $(MAKE) libc64c64dtv.a)
libc64cart:
	(cd c64 && $(MAKE) libc64cart)
libc64dtv:
	(cd c64dtv && $(MAKE) libc64dtv.a)
libc64dtvstubs:
	(cd c64dtv && $(MAKE) libc64dtvstubs.a)
libcbm2:
	(cd cbm2 && $(MAKE))
libcore:
	(cd core && $(MAKE))
libcrtc:
	(cd crtc && $(MAKE))
libdiskimage:
	(cd diskimage && $(MAKE))
libdrive:
	(cd drive && $(MAKE))
libdriveiec:
	(cd drive && $(MAKE) libdriveiec)
libdriveiec128dcr:
	(cd drive && $(MAKE) libdriveiec128dcr)
libdriveiecc64exp:
	(cd drive/iec && $(MAKE) libdriveiecc64exp)
libdriveiecieee:
	(cd drive && $(MAKE) libdriveiecieee)
libdriveiecplus4exp:
	(cd drive/iec && $(MAKE) libdriveiecplus4exp)
libdriveieee:
	(cd drive && $(MAKE) libdriveieee)
libdrivetcbm:
	(cd drive && $(MAKE) libdrivetcbm)
libgfxoutputdrv:
	(cd gfxoutputdrv && $(MAKE))
libfileio:
	(cd fileio && $(MAKE))
libfsdevice:
	(cd fsdevice && $(MAKE))
libiecbus:
	(cd iecbus && $(MAKE))
libimagecontents:
	(cd imagecontents && $(MAKE))
libmonitor:
	(cd monitor && $(MAKE) libmonitor.a)
libparallel:
	(cd parallel && $(MAKE))
libpet:
	(cd pet && $(MAKE))
libplus4:
	(cd plus4 && $(MAKE))
libprinterdrv:
	(cd printerdrv && $(MAKE))
librs232drv:
	(cd rs232drv && $(MAKE))
libraster:
	(cd raster && $(MAKE) libraster.a)
librtc:
	(cd rtc && $(MAKE) librtc.a)
libresid:
	(cd resid && $(MAKE) libresid.a)
libresiddtv:
	(cd resid-dtv && $(MAKE) libresiddtv.a)
libresidfp:
	(cd resid-fp && $(MAKE) libresidfp.a)
libserial:
	(cd serial && $(MAKE))
libsid:
	(cd sid && $(MAKE) libsid.a)
libsounddrv:
	(cd sounddrv && $(MAKE))
libtape:
	(cd tape && $(MAKE))
libvdc:
	(cd vdc && $(MAKE))
libvdrive:
	(cd vdrive && $(MAKE))
libvic20:
	(cd vic20 && $(MAKE))
libvic20cart:
	(cd vic20 && $(MAKE) libvic20cart)
libvicii:
	(cd vicii && $(MAKE) libvicii.a)
libviciidtv:
	(cd vicii && $(MAKE) libviciidtv.a)
libviciisc:
	(cd viciisc && $(MAKE) libviciisc.a)
libvideo:
	(cd video && $(MAKE))

x64ow$(EXEEXT): x64$(EXEEXT)
	wrc -fe=x64ow$(EXEEXT) arch/win32/resc64.res x64$(EXEEXT)

x64dtvow$(EXEEXT): x64dtv$(EXEEXT)
	wrc -fe=x64dtvow$(EXEEXT) arch/win32/resc64dtv.res x64dtv$(EXEEXT)

x64scow$(EXEEXT): x64sc$(EXEEXT)
	wrc -fe=x64scow$(EXEEXT) arch/win32/resc64.res x64sc$(EXEEXT)

x128ow$(EXEEXT): x128$(EXEEXT)
	wrc -fe=x128ow$(EXEEXT) arch/win32/resc128.res x128$(EXEEXT)

xcbm2ow$(EXEEXT): xcbm2$(EXEEXT)
	wrc -fe=xcbm2ow$(EXEEXT) arch/win32/rescbm2.res xcbm2$(EXEEXT)

xpetow$(EXEEXT): xpet$(EXEEXT)
	wrc -fe=xpetow$(EXEEXT) arch/win32/respet.res xpet$(EXEEXT)

xplus4ow$(EXEEXT): xplus4$(EXEEXT)
	wrc -fe=xplus4ow$(EXEEXT) arch/win32/resplus4.res xplus4$(EXEEXT)

xvicow$(EXEEXT): xvic$(EXEEXT)
	wrc -fe=xvicow$(EXEEXT) arch/win32/resvic20.res xvic$(EXEEXT)
