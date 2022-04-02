# vim: set noet ts=8 sw=8 sts=8:

# We have to override the automake default, because we need to use
# $(CXX) instead of $(CC) when compiling with reSID.
LINK = @LINKCC@ @VICE_CFLAGS@ @VICE_LDFLAGS@ $(LDFLAGS) -o $@
# We shouldnt have to add the flags here, but without it doesn't add them
CCLD = @LINKCC@ @VICE_LDFLAGS@ $(LDFLAGS)

AM_CFLAGS = @VICE_CFLAGS@
AM_CXXFLAGS = @VICE_CXXFLAGS@
AM_LDFLAGS = @VICE_LDFLAGS@

ARCH_LIBS = @ARCH_LIBS@


# Windows .res files to be linked to each binary 
if WINDOWS_COMPILE
# .. but NOT in headless mode
if !USE_HEADLESSUI
X64_RES = $(top_builddir)/data/common/x64.res
X64DTV_RES = $(top_builddir)/data/common/x64dtv.res
X64SC_RES = $(top_builddir)/data/common/x64sc.res
XSCPU64_RES = $(top_builddir)/data/common/xscpu64.res
X128_RES = $(top_builddir)/data/common/x128.res
XVIC_RES = $(top_builddir)/data/common/xvic.res
XPLUS4_RES = $(top_builddir)/data/common/xplus4.res
XPET_RES = $(top_builddir)/data/common/xpet.res
XCBM2_RES = $(top_builddir)/data/common/xcbm2.res
XCBM5x0_RES = $(top_builddir)/data/common/xcbm5x0.res
VSID_RES = $(top_builddir)/data/common/vsid.res
endif
endif


if DUMMY_COMPILE
SUBDIRS = buildtools

else

SUBDIRS = \
	@RESID_DIR@ \
	@RESID_DTV_DIR@ \
	joyport \
	socketdrv \
	hwsiddrv \
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
	userport \
	tapeport \
	diag \
	c64 \
	scpu64 \
	c64dtv \
	c128 \
	vic20 \
	pet \
	plus4 \
	cbm2 \
	arch \
	lib \
	hvsc \
	datasette \
	tools

endif

DIST_SUBDIRS = \
	socketdrv \
	hwsiddrv \
	joyport \
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
	diag \
	userport \
	tapeport \
	c64 \
	scpu64 \
	c64dtv \
	c128 \
	vic20 \
	pet \
	plus4 \
	cbm2 \
	arch \
	@RESIDSUB@ \
	@RESIDDTVSUB@ \
	lib \
	buildtools \
	hvsc \
	datasette \
	tools

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/datasette \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/fsdevice \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/diskimage \
	-I$(top_srcdir)/src/imagecontents \
	-I$(top_srcdir)/src/diag \
	-I$(top_srcdir)/src/userport \
	-I$(top_srcdir)/src/lib/ \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/joyport \
	-I$(top_srcdir)/src/core/rtc \
	-I$(top_srcdir)/src/tapeport \
	-I$(top_srcdir)/src/tape \
	-I$(top_srcdir)/src/socketdrv \
	-I$(top_srcdir)/src/hvsc \
	-I$(top_srcdir)/src/lib/linenoise-ng \
	-I$(top_srcdir)/src/cart

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
	attach.h \
	autostart.h \
	autostart-prg.h \
	c128ui.h \
	c64ui.h \
	cartio.h \
	cartridge.h \
	catweaselmkiii.h \
	cbm2ui.h \
	cbmdos.h \
	cbmimage.h \
	charset.h \
	cia.h \
	clipboard.h \
	cmdline.h \
	crt.h \
	color.h \
	config.h.in \
	console.h \
	crc32.h \
	debug.h \
	digimaxcore.c \
	diskconstants.h \
	diskimage.h \
	dma.h \
	dynlib.h \
	export.h \
	fileio.h \
	findpath.h \
	fixpoint.h \
	fsdevice.h \
	flash040.h \
	fliplist.h \
	fullscreen.h \
	gcr.h \
	gfxoutput.h \
	h6809regs.h \
	hardsid.h \
	iecbus.h \
	iecdrive.h \
	imagecontents.h \
	info.h \
	init.h \
	initcmdline.h \
	interrupt.h \
	kbdbuf.h \
	keyboard.h \
	lib.h \
	log.h \
	machine-bus.h \
	machine-drive.h \
	machine-printer.h \
	machine-video.h \
	machine.h \
	main.h \
	mainc64cpu.c \
	mainc64cpu.h \
	main65816cpu.c \
	main65816cpu.h \
	maincpu.c \
	maincpu.h \
	mainlock.h \
	mainviccpu.c \
	mem.h \
	midi.h \
	mididrv.h \
	monitor.h \
	mos6510.h \
	mos6510dtv.h \
	network.h \
	opencbmlib.h \
	palette.h \
	parallel.h \
	parsid.h \
	petui.h \
	piacore.h \
	plus4ui.h \
	printer.h \
	ps2mouse.h \
	r65c02.h \
	ram.h \
	rawfile.h \
	rawnet.h \
	rawnetarch.h \
	resources.h \
	riot.h \
	romset.h \
	scpu64ui.h \
	screenshot.h \
	snapshot.h \
	serial.h \
	sidcart.h \
	signals.h \
	snespad.h \
	sound.h \
	ssi2001.h \
	sysfile.h \
	tap.h \
	tape.h \
	tpi.h \
	traps.h \
	types.h \
	uiapi.h \
	uicmdline.h \
	uicolor.h \
	uimon.h \
	util.h \
	version.h.in \
	version.h \
	via.h \
	vic20ui.h \
	vice.h \
	vice_sdl.h \
	vicedate.h \
	vice-event.h \
	vicesocket.h \
	vicefeatures.h \
	vicii.h \
	video.h \
	viewport.h \
	vsidui.h \
	vsync.h \
	vsyncapi.h \
	wdc65816.h \
	z80regs.h \
	zfile.h \
	zipcode.h

base_sources = \
	alarm.c \
	attach.c \
	autostart.c \
	autostart-prg.c \
	cbmdos.c \
	cbmimage.c \
	charset.c \
	clipboard.c \
	cmdline.c \
	color.c \
	crc32.c \
	crt.c \
	debug.c \
	dma.c \
	event.c \
	findpath.c \
	fliplist.c \
	gcr.c \
	info.c \
	init.c \
	initcmdline.c \
	interrupt.c \
	kbdbuf.c \
	keyboard.c \
	lib.c \
	log.c \
	machine-bus.c \
	machine.c \
	main.c \
	mainlock.c \
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
	traps.c \
	util.c \
	vicefeatures.c \
	vsync.c \
	zfile.c \
	zipcode.c

ps2mouse_sources = \
	ps2mouse.c

midi_sources = \
	midi.c

cpucore_sources = \
	6510core.c \
	65c02core.c \
	65816core.c

cpudtvcore_sources = \
	6510dtvcore.c

COMMON_EXTRA_DIST = \
	systemheaderoverride/gtk/gtk.h \
	6510core.c \
	65816core.c \
	65c02core.c \
	6510dtvcore.c \
	aciacore.c \
	debug.h.in \
	fixpoint.c \
	piacore.c \
	vice-version.sh \
	vice-version.sh.in \
	wrap-u-ar.sh

# RESID_EXTRA_DIST is a list of files in the resid directory that need to be
# included in the source archive, this list was added to fix the fact that
# 'make dist' only produced a proper/workable source archive when done on a
# platform with a c++ compiler and resid enabled.

RESID_EXTRA_DIST = \
	resid/aclocal.m4 \
	resid/AUTHORS \
	resid/ChangeLog \
	resid/configure \
	resid/configure.in \
	resid/COPYING \
	resid/dac.cc \
	resid/dac.h \
	resid/envelope.cc \
	resid/envelope.h \
	resid/extfilt.cc \
	resid/extfilt.h \
	resid/filter.cc \
	resid/filter.h \
	resid/INSTALL \
	resid/Makefile.am \
	resid/Makefile.in \
	resid/NEWS \
	resid/pot.cc \
	resid/pot.h \
	resid/README \
	resid/README.VICE \
	resid/resid-config.h \
	resid/samp2src.pl \
	resid/sid.cc \
	resid/sid.h \
	resid/siddefs.h.in \
	resid/spline.h \
	resid/THANKS \
	resid/TODO \
	resid/version.cc \
	resid/voice.cc \
	resid/voice.h \
	resid/wave.cc \
	resid/wave.h \
	resid/wave6581__ST.dat \
	resid/wave6581__ST.h \
	resid/wave6581_P_T.dat \
	resid/wave6581_P_T.h \
	resid/wave6581_PS_.dat \
	resid/wave6581_PS_.h \
	resid/wave6581_PST.dat \
	resid/wave6581_PST.h \
	resid/wave8580__ST.dat \
	resid/wave8580__ST.h \
	resid/wave8580_P_T.dat \
	resid/wave8580_P_T.h \
	resid/wave8580_PS_.dat \
	resid/wave8580_PS_.h \
	resid/wave8580_PST.dat \
	resid/wave8580_PST.h

# RESIDDTV_EXTRA_DIST is a list of files in the resid directory that need to be
# included in the source archive, this list was added to fix the fact that
# 'make dist' only produced a proper/workable source archive when done on a
# platform with a c++ compiler and resid enabled.

RESIDDTV_EXTRA_DIST = \
	resid-dtv/aclocal.m4 \
	resid-dtv/AUTHORS \
	resid-dtv/bittrain.h \
	resid-dtv/ChangeLog \
	resid-dtv/configure \
	resid-dtv/configure.in \
	resid-dtv/COPYING \
	resid-dtv/envelope.cc \
	resid-dtv/envelope.h \
	resid-dtv/extfilt.cc \
	resid-dtv/extfilt.h \
	resid-dtv/filter.cc \
	resid-dtv/filter.h \
	resid-dtv/INSTALL \
	resid-dtv/Makefile.am \
	resid-dtv/Makefile.in \
	resid-dtv/NEWS \
	resid-dtv/README \
	resid-dtv/residdtv-config.h \
	resid-dtv/sid.cc \
	resid-dtv/sid.h \
	resid-dtv/siddtvdefs.h.in \
	resid-dtv/version.cc \
	resid-dtv/voice.cc \
	resid-dtv/voice.h \
	resid-dtv/wave.cc \
	resid-dtv/wave.h

if RESID_DIR_USED

EXTRA_DIST = $(COMMON_EXTRA_DIST)

else

EXTRA_DIST = $(COMMON_EXTRA_DIST) $(RESID_EXTRA_DIST) $(RESIDDTV_EXTRA_DIST)

endif


if USE_SVN_REVISION
SVN_VERSION_HEADER = svnversion.h
if USE_SVN_REVISION_OVERRIDE
SVN_REVISION = "@SVN_REVISION_OVERRIDE@"
else
SVN_REVISION = `$(SVNVERSION) $(top_srcdir)`
endif
else
SVN_VERSION_HEADER =
endif

BUILT_SOURCES = $(SVN_VERSION_HEADER) infocontrib.h

if USE_SVN_REVISION
svnversion.h.indirect:
	@$(SHELL) $(srcdir)/buildtools/gensvnversion.sh $(SVN_REVISION) svnversion.h

svnversion.h: svnversion.h.indirect
endif

infocontrib.h: $(srcdir)/buildtools/geninfocontrib_h.sh $(top_srcdir)/doc/vice.texi $(srcdir)/buildtools/infocontrib.sed
	@echo "generating infocontrib.h"
	@if [ "`file --mime-encoding $(srcdir)/buildtools/geninfocontrib_h.sh`" != "$(srcdir)/buildtools/geninfocontrib_h.sh: iso-8859-1" ]; then \
		echo "ERROR: geninfocontrib_h.sh contains content that is not valid iso-8859-x" >&2; \
		false; \
	fi
	@$(SHELL) $(srcdir)/buildtools/geninfocontrib_h.sh infocontrib.h <$(top_srcdir)/doc/vice.texi `grep "VICEDATE_YEAR " $(top_srcdir)/src/vicedate.h | cut -d " " -f 3` | LC_ALL=C sed -f $(srcdir)/buildtools/infocontrib.sed >infocontrib.h
	@if [ "`file --mime-encoding infocontrib.h`" != "infocontrib.h: iso-8859-1" ]; then \
		echo "ERROR: generated infocontrib.h contains content that is not valid iso-8859-x" >&2; \
		false; \
	fi

# we need to replace \ and space by "regular" characters, else things break on some shells
	@LC_ALL=C sed -e 's/\\/{/g' -e 's/ /}/g' <$(top_srcdir)/README >./README.tmp
	@$(SHELL) $(srcdir)/buildtools/geninfocontrib_h.sh README `grep "VICEDATE_YEAR " $(top_srcdir)/src/vicedate.h | cut -d " " -f 3` <README.tmp >README2.tmp
	@LC_ALL=C sed -e 's/{/\\/g' -e 's/}/ /g' <README2.tmp >../README
	@rm -f README*.tmp
	@cp $(top_srcdir)/doc/html/index.html ./index.tmp
	@$(SHELL) $(srcdir)/buildtools/geninfocontrib_h.sh indexhtml `grep "VICEDATE_YEAR " $(top_srcdir)/src/vicedate.h | cut -d " " -f 3` <index.tmp >../doc/html/index.html
	@rm -f index.tmp
	@rm -f team.tmp
	@iconv -f ISO-8859-15 -t UTF-8 infocontrib.h > infocontrib.h.utf8 && mv infocontrib.h.utf8 infocontrib.h



arch_gtk3_lib = $(top_builddir)/src/arch/gtk3/libarch.a
arch_gtk3_widgets_lib = $(top_builddir)/src/arch/gtk3/widgets/libwidgets.a
arch_gtk3_base_widgets_lib = $(top_builddir)/src/arch/gtk3/widgets/base/libbasewidgets.a
arch_gtk3_novte_lib = $(top_builddir)/src/arch/gtk3/novte/libnovte.a
arch_gtk3_joystickdrv_lib = $(top_builddir)/src/arch/gtk3/joystickdrv/libjoystickdrv.a
arch_sdl_lib = $(top_builddir)/src/arch/sdl/libarch.a
arch_headless_lib = $(top_builddir)/src/arch/headless/libarch.a
archdep_lib = $(top_builddir)/src/arch/shared/libarchdep.a
c128_lib = $(top_builddir)/src/c128/libc128.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64sc_lib = $(top_builddir)/src/c64/libc64sc.a
scpu64_lib = $(top_builddir)/src/scpu64/libscpu64.a
c64dtv_lib = $(top_builddir)/src/c64dtv/libc64dtv.a
c64dtvstubs_lib = $(top_builddir)/src/c64dtv/libc64dtvstubs.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
c64scpu64_lib = $(top_builddir)/src/c64/libc64scpu64.a
c64c64dtv_lib = $(top_builddir)/src/c64/libc64c64dtv.a
c64cart_lib = $(top_builddir)/src/c64/cart/libc64cart.a
c64cartsystem_lib = $(top_builddir)/src/c64/cart/libc64cartsystem.a
c64commoncart_lib = $(top_builddir)/src/c64/cart/libc64commoncart.a
core_lib = $(top_builddir)/src/core/libcore.a
crtc_lib = $(top_builddir)/src/crtc/libcrtc.a
datasette_lib = $(top_builddir)/src/datasette/libdatasette.a
diag_lib = $(top_builddir)/src/diag/libdiag.a
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
hwsiddrv_lib = $(top_builddir)/src/hwsiddrv/libhwsiddrv.a
hvsc_lib = $(top_builddir)/src/hvsc/libhvsc.a
iecbus_lib = $(top_builddir)/src/iecbus/libiecbus.a
imagecontents_lib = $(top_builddir)/src/imagecontents/libimagecontents.a
iodrv_lib = $(top_builddir)/src/arch/shared/iodrv/libiodrv.a
joyport_lib = $(top_builddir)/src/joyport/libjoyport.a
linenoise_ng_lib = $(top_builddir)/src/lib/linenoise-ng/liblinenoiseng.a
mascuerade_lib = $(top_builddir)/src/vic20/cart/libmascuerade.a
mididrv_lib = $(top_builddir)/src/arch/shared/mididrv/libmididrv.a
monitor_lib = $(top_builddir)/src/monitor/libmonitor.a
p64_lib = $(top_builddir)/src/lib/p64/libp64.a
parallel_lib = $(top_builddir)/src/parallel/libparallel.a
pet_lib = $(top_builddir)/src/pet/libpet.a
plus4_lib = $(top_builddir)/src/plus4/libplus4.a
plus4cart_lib = $(top_builddir)/src/plus4/cart/libplus4cart.a
printerdrv_lib = $(top_builddir)/src/printerdrv/libprinterdrv.a
resid_lib = $(top_builddir)/src/resid/libresid.a
resid_dtv_lib = $(top_builddir)/src/resid-dtv/libresiddtv.a
rs232drv_lib = $(top_builddir)/src/rs232drv/librs232drv.a
raster_lib = $(top_builddir)/src/raster/libraster.a
rtc_lib = $(top_builddir)/src/core/rtc/librtc.a
samplerdrv_lib = $(top_builddir)/src/arch/shared/samplerdrv/libsamplerdrv.a
serial_lib = $(top_builddir)/src/serial/libserial.a
sid_lib = $(top_builddir)/src/sid/libsid.a
sid_dtv_lib = $(top_builddir)/src/sid/libsid_dtv.a
socketdrv_lib = $(top_builddir)/src/socketdrv/libsocketdrv.a
sounddrv_lib = $(top_builddir)/src/arch/shared/sounddrv/libsounddrv.a
tape_lib = $(top_builddir)/src/tape/libtape.a
tapeport_lib = $(top_builddir)/src/tapeport/libtapeport.a
toolarch_lib = $(ARCH_DIR)/libtoolarch.a
userport_lib = $(top_builddir)/src/userport/libuserport.a
vdc_lib = $(top_builddir)/src/vdc/libvdc.a
vdrive_lib = $(top_builddir)/src/vdrive/libvdrive.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
vic20cart_lib = $(top_builddir)/src/vic20/cart/libvic20cart.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
viciidtv_lib = $(top_builddir)/src/vicii/libviciidtv.a
viciisc_lib = $(top_builddir)/src/viciisc/libviciisc.a
video_lib = $(top_builddir)/src/video/libvideo.a
vsid_lib = $(top_builddir)/src/c64/libvsid.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a
cbm2cart_lib = $(top_builddir)/src/cbm2/cart/libcbm2cart.a
xcbm5x0_lib = $(top_builddir)/src/cbm2/libcbm5x0.a

common_libs = @ARCH_LIBS@ @LIBOBJS@ @READLINE@
resid_libs = @RESID_LIBS@
resid_dtv_libs = @RESID_DTV_LIBS@

# external libraries required for all emulators
emu_extlibs = @UI_LIBS@ @SDL_EXTRA_LIBS@ @SOUND_LIBS@ @JOY_LIBS@ @GFXOUTPUT_LIBS@ @ZLIB_LIBS@ @DYNLIB_LIBS@ @ARCH_LIBS@ $(archdep_lib)

driver_libs = $(joyport_lib) $(samplerdrv_lib) $(sounddrv_lib) $(mididrv_lib) $(socketdrv_lib) $(hwsiddrv_lib) $(iodrv_lib) $(gfxoutputdrv_lib) $(printerdrv_lib) $(rs232drv_lib) $(diskimage_lib) $(fsdevice_lib) $(tape_lib) $(fileio_lib) $(serial_lib) $(core_lib)

if SUPPORT_X64
x64_bin = x64
else
x64_bin =
endif


# workaround for extra exe creation

bin_PROGRAMS = vsid $(x64_bin) x64sc x64dtv xscpu64 x128 xvic xpet xplus4 \
	       xcbm2 xcbm5x0 c1541

EXTRA_PROGRAMS =

# vsid
vsid_libs =  \
	$(archdep_lib) \
	$(vsid_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(sounddrv_lib) \
	$(mididrv_lib) \
	$(socketdrv_lib) \
	$(hwsiddrv_lib) \
	$(iodrv_lib) \
	$(serial_lib) \
	$(core_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(hvsc_lib) \
	$(datasette_lib)

vsid_SOURCES = $(base_sources) $(midi_sources)
vsid_LDADD = $(vsid_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(VSID_RES)

vsid_DEPENDENCIES = $(vsid_libs)
vsid_LDFLAGS = @vsid_LDFLAGS@

.PHONY: vsid-all vsid-program
vsid-all:
	@for i in $(vsid_libs); do rm -fr "$$i"; done
	$(MAKE) vsid-program

vsid-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	vsid$(EXEEXT)

# x64
x64_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(c64_lib) \
	$(c64cartsystem_lib) \
	$(c64cart_lib) \
	$(c64commoncart_lib) \
	$(datasette_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(iecbus_lib) \
	$(p64_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(vicii_lib) \
	$(raster_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_libs) \
	$(hvsc_lib)

x64_SOURCES = $(base_sources) $(midi_sources)
x64_LDADD = $(x64_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(X64_RES)
x64_DEPENDENCIES = $(x64_libs)
x64_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64-all x64-program
x64-all:
	@for i in $(x64_libs); do rm -fr "$$i"; done
	$(MAKE) x64-program

x64-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	x64$(EXEEXT)

# x64sc
x64sc_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(c64sc_lib) \
	$(c64cartsystem_lib) \
	$(c64cart_lib) \
	$(c64commoncart_lib) \
	$(datasette_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(viciisc_lib) \
	$(raster_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_libs) \
	$(hvsc_lib)

x64sc_SOURCES = $(base_sources) $(midi_sources)
x64sc_LDADD = $(x64sc_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(X64SC_RES)
x64sc_DEPENDENCIES = $(x64sc_libs)
x64sc_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64sc-all x64sc-program
x64sc-all:
	@for i in $(x64sc_libs); do rm -fr "$$i"; done
	$(MAKE) x64sc-program

x64sc-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	x64sc$(EXEEXT)

# x64dtv
x64dtv_libs =  \
	$(archdep_lib) \
	$(c64dtv_lib) \
	$(c64c64dtv_lib) \
	$(datasette_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(vdrive_lib) \
	$(sid_dtv_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(viciidtv_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_dtv_libs) \
	$(c64dtvstubs_lib) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(rtc_lib) \
	$(hvsc_lib)

x64dtv_SOURCES = $(base_sources) $(ps2mouse_sources)
x64dtv_LDADD = $(x64dtv_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(X64DTV_RES)
x64dtv_DEPENDENCIES = $(x64dtv_libs)
x64dtv_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64dtv-all x64dtv-program
x64dtv-all:
	@for i in $(x64dtv_libs); do rm -fr "$$i"; done
	$(MAKE) x64dtv-program

x64dtv-program: \
	$(BUILT_SOURCES) \
	@RESID_DTV_DEP@ \
	x64dtv$(EXEEXT)

# xscpu64
xscpu64_libs =  \
	$(archdep_lib) \
	$(scpu64_lib) \
	$(c64cartsystem_lib) \
	$(c64cart_lib) \
	$(c64commoncart_lib) \
	$(c64scpu64_lib) \
	$(datasette_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(viciisc_lib) \
	$(raster_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_libs) \
	$(hvsc_lib)

xscpu64_SOURCES = $(base_sources) $(midi_sources)
xscpu64_LDADD = $(xscpu64_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XSCPU64_RES)
xscpu64_DEPENDENCIES = $(xscpu64_libs)
xscpu64_LDFLAGS = @xscpu64_LDFLAGS@

.PHONY: xscpu64-all xscpu64-program
xscpu64-all:
	@for i in $(xscpu64_libs); do rm -fr "$$i"; done
	$(MAKE) xscpu64-program

xscpu64-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xscpu64$(EXEEXT)

# x128
x128_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(c128_lib) \
	$(c64cartsystem_lib) \
	$(c64cart_lib) \
	$(c64commoncart_lib) \
	$(c64c128_lib) \
	$(datasette_lib) \
	$(driveiec_lib) \
	$(driveiec128dcr_lib) \
	$(driveiecieee_lib) \
	$(driveiecc64exp_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(monitor_lib) \
	$(driver_libs) \
	$(vicii_lib) \
	$(vdc_lib) \
	$(raster_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_libs) \
	$(hvsc_lib)

x128_SOURCES = $(base_sources) $(midi_sources)
x128_LDADD = $(x128_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(X128_RES)
x128_DEPENDENCIES = $(x128_libs)
x128_LDFLAGS = @x128_LDFLAGS@

.PHONY: x128-all x128-program
x128-all:
	@for i in $(x128_libs); do rm -fr "$$i"; done
	$(MAKE) x128-program

x128-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	x128$(EXEEXT)

# xvic
xvic_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(datasette_lib) \
	$(vic20cart_lib) \
	$(vic20_lib) \
	$(c64commoncart_lib) \
	$(mascuerade_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(sid_lib) \
	$(driver_libs) \
	$(raster_lib) \
	$(userport_lib) \
	$(diag_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(resid_libs) \
	$(hvsc_lib)

xvic_SOURCES = $(base_sources) $(midi_sources)
xvic_LDADD = $(xvic_libs) $(emu_extlibs) $(mascuerade_lib) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XVIC_RES)
xvic_DEPENDENCIES = $(xvic_libs)
xvic_LDFLAGS = @xvic_LDFLAGS@

.PHONY: xvic-all xvic-program
xvic-all:
	@for i in $(xvic_libs); do rm -fr "$$i"; done
	$(MAKE) xvic-program

xvic-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xvic$(EXEEXT)

# xpet
xpet_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(datasette_lib) \
	$(pet_lib) \
	$(diag_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(driveiec_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(sid_lib) \
	$(driver_libs) \
	$(crtc_lib) \
	$(raster_lib) \
	$(userport_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(resid_libs) \
	$(rtc_lib) \
	$(hvsc_lib)

xpet_SOURCES = $(base_sources)
xpet_LDADD = $(xpet_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XPET_RES)
xpet_DEPENDENCIES = $(xpet_libs) 
xpet_LDFLAGS = @xpet_LDFLAGS@

.PHONY: xpet-all xpet-program
xpet-all:
	@for i in $(xpet_libs); do rm -fr "$$i"; done
	$(MAKE) xpet-program

xpet-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xpet$(EXEEXT)

# xplus4
xplus4_libs =  \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(datasette_lib) \
	$(plus4_lib) \
	$(plus4cart_lib) \
	$(diag_lib) \
	$(driveiec_lib) \
	$(driveiecieee_lib) \
	$(driveiecplus4exp_lib) \
	$(drivetcbm_lib) \
	$(drive_lib) \
	$(driveieee_lib) \
	$(parallel_lib) \
	$(driveieee_lib) \
	$(p64_lib) \
	$(iecbus_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(sid_lib) \
	$(driver_libs) \
	$(raster_lib) \
	$(rtc_lib) \
	$(userport_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(resid_libs) \
	$(hvsc_lib)

xplus4_SOURCES = $(base_sources)
xplus4_LDADD = $(xplus4_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XPLUS4_RES)
xplus4_DEPENDENCIES = $(xplus4_libs)
xplus4_LDFLAGS = @xplus4_LDFLAGS@

.PHONY: xplus4-all xplus4-program
xplus4-all:
	@for i in $(xplus4_libs); do rm -fr "$$i"; done
	$(MAKE) xplus4-program

xplus4-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xplus4$(EXEEXT)

# xcbm2
xcbm2_libs = \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(datasette_lib) \
	$(xcbm2_lib) \
	$(cbm2cart_lib) \
	$(diag_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(driveiec_lib) \
	$(drivetcbm_lib) \
	$(p64_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(sid_lib) \
	$(driver_libs) \
	$(crtc_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(userport_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(resid_libs) \
	$(rtc_lib) \
	$(hvsc_lib)

xcbm2_SOURCES = $(base_sources)
xcbm2_LDADD = $(xcbm2_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XCBM2_RES)
xcbm2_DEPENDENCIES = $(xcbm2_libs)
xcbm2_LDFLAGS = @xcbm2_LDFLAGS@

.PHONY: xcbm2-all xcbm2-program
xcbm2-all:
	@for i in $(xcbm2_libs); do rm -fr "$$i"; done
	$(MAKE) xcbm2-program

xcbm2-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xcbm2$(EXEEXT)

# xcbm5x0
xcbm5x0_libs = \
	$(archdep_lib) \
	$(tapeport_lib) \
	$(xcbm5x0_lib) \
	$(cbm2cart_lib) \
	$(datasette_lib) \
	$(diag_lib) \
	$(driveiecieee_lib) \
	$(driveieee_lib) \
	$(drive_lib) \
	$(drivetcbm_lib) \
	$(driveiec_lib) \
	$(p64_lib) \
	$(parallel_lib) \
	$(vdrive_lib) \
	$(monitor_lib) \
	$(xcbm5x0_lib) \
	$(sid_lib) \
	$(driver_libs) \
	$(vicii_lib) \
	$(raster_lib) \
	$(rtc_lib) \
	$(video_lib) \
	$(common_libs) \
	$(imagecontents_lib) \
	$(joyport_lib) \
	$(joystickdrv_lib) \
	$(resid_libs) \
	$(hvsc_lib)

xcbm5x0_SOURCES = $(base_sources)
xcbm5x0_LDADD = $(xcbm5x0_libs) $(emu_extlibs) @TFE_LIBS@ @NETPLAY_LIBS@ @READLINE_LIBS@ $(XCBM5x0_RES)
xcbm5x0_DEPENDENCIES = $(xcbm5x0_libs) 
xcbm5x0_LDFLAGS = @xcbm2_LDFLAGS@

.PHONY: xcbm5x0-all xcbm5x0-program
xcbm5x0-all:
	@for i in $(xcbm5x0_libs); do rm -fr "$$i"; done
	$(MAKE) xcbm5x0-program

xcbm5x0-program: \
	$(BUILT_SOURCES) \
	@RESID_DEP@ \
	xcbm5x0$(EXEEXT)

# c1541
c1541_SOURCES = \
	c1541.c \
	c1541-stubs.c \
	cbmdos.c \
	charset.c \
	findpath.c \
	gcr.c \
	cbmimage.c \
	info.c \
	lib.c \
	log.c \
	opencbmlib.c \
	rawfile.c \
	resources.c \
	util.c \
	zfile.c \
	zipcode.c

c1541_libs = \
	$(toolarch_lib) \
	$(p64_lib) \
	$(blockdev_lib) \
	$(vdrive_lib) \
	$(imagecontents_lib) \
	$(fileio_lib) \
	$(diskimage_lib) \
	$(tape_lib) \
	$(serial_lib) \
	$(socketdrv_lib) \
	$(linenoise_ng_lib) \
	$(archdep_lib)

c1541_LDADD = \
	$(c1541_libs) \
	@SDL_EXTRA_LIBS@ \
	@READLINE@ @READLINE_LIBS@ \
	@ZLIB_LIBS@ @DYNLIB_LIBS@

if WINDOWS_COMPILE
c1541_LDFLAGS = -mconsole
endif


.PHONY: c1541-all c1541-program
c1541-all:
	@for i in $(c1541_libs); do rm -fr "$$i"; done
	$(MAKE) c1541-program

c1541-program: \
	$(BUILT_SOURCES) \
	c1541$(EXEEXT)

# distclean
DISTCLEANFILES = $(BUILT_SOURCES) $(GENFILES)

# Special targets to build libraries in subdirectories.
.PHONY: @RESID_DEP@ @RESID_DTV_DEP@

info.c: infocontrib.h


clean: clean-recursive
