# We have to override the automake default, because we need to use
# $(CXX) instead of $(CC) when compiling with reSID.
LINK = @LINKCC@ $(CFLAGS) $(LDFLAGS) -o $@
CCLD = @LINKCC@

ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = \
	@RESID_DIR@ \
	@RESID_FP_DIR@ \
	@RESID_DTV_DIR@ \
	@VICE_LIBPNG_DIR@ \
	@VICE_ZLIB_DIR@ \
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
	vdc \
	crtc \
	gfxoutputdrv \
	printerdrv \
	rs232drv \
	sid \
	monitor \
	core \
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
	vdc \
	crtc \
	gfxoutputdrv \
	printerdrv \
	rs232drv \
	sid \
	monitor \
	core \
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
	@VICE_ZLIB_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/imagecontents

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
	archapi.h \
	attach.h \
	autostart.h \
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
	emuid.h \
	event.h \
	fileio.h \
	findpath.h \
	fixpoint.h \
	fsdevice.h \
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
	log.h \
	machine-bus.h \
	machine-drive.h \
	machine-printer.h \
	machine-video.h \
	machine.h \
	main.h \
	maincpu.h \
	mc6821.h \
	mem.h \
	midi.h \
	mididrv.h \
	monitor.h \
	mos6510.h \
	mos6510dtv.h \
	mouse.h \
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
	ram.h \
	rawfile.h \
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
	emuid.c \
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
	log.c \
	machine-bus.c \
	machine.c \
	main.c \
	network.c \
	palette.c \
	ram.c \
	rawfile.c \
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

maincpu_sources = \
	maincpu.c

EXTRA_DIST = \
	6510core.c \
	ChangeLog \
	ChangeLog.1 \
	ChangeLog.2 \
	ChangeLog.3 \
	ChangeLog.4 \
	ChangeLog.5 \
	ChangeLog.6 \
	aciacore.c \
	fixpoint.c \
	gentranslate_h.sh \
	gentranslatetable.sh \
	piacore.c \
	translate.txt

if HAVE_TRANSLATION
BUILT_SOURCES = translate.h translate_table.h

translate_table.h:	$(srcdir)/translate.txt
	echo "generating translate_table.h..."
	$(SHELL) $(srcdir)/gentranslatetable.sh <$(srcdir)/translate.txt >translate_table.h
else
BUILT_SOURCES = translate.h
endif

translate.h:	$(srcdir)/translate.txt
	echo "generating translate.h..."
if MINIX_SUPPORT
	cc $(srcdir)/arch/win32/utils/gentranslate.c -o gentranslate
	./gentranslate translate.txt translate.h translate_table.h
else
	$(SHELL) $(srcdir)/gentranslate_h.sh <$(srcdir)/translate.txt >translate.h
endif

c128_lib = $(top_builddir)/src/c128/libc128.a
c64_lib = $(top_builddir)/src/c64/libc64.a
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
serial_lib = $(top_builddir)/src/serial/libserial.a
sid_lib = $(top_builddir)/src/sid/libsid.a
sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
tape_lib = $(top_builddir)/src/tape/libtape.a
vdc_lib = $(top_builddir)/src/vdc/libvdc.a
vdrive_lib = $(top_builddir)/src/vdrive/libvdrive.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
viciidtv_lib = $(top_builddir)/src/vicii/libviciidtv.a
video_lib = $(top_builddir)/src/video/libvideo.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a

common_libs = @ARCH_LIBS@ @LIBOBJS@ @READLINE@ @VICE_LIBPNG@ @VICE_ZLIB@
resid_libs = @RESID_LIBS@ @RESID_FP_LIBS@
resid_dtv_libs = @RESID_DTV_LIBS@ @RESID_FP_LIBS@

# external libraries required for all emulators
emu_extlibs = @X_LIBS@ @UI_LIBS@ @INTLLIBS@ @SOUND_LIBS@ @FFMPEG_LIBS@ @QUICKTIME_LIBS@ @JOY_LIBS@ @READLINE_LIBS@ @GFXOUTPUT_LIBS@ @ZLIB_LIBS@ @DYNLIB_LIBS@

driver_libs = $(sounddrv_lib) $(gfxoutputdrv_lib) $(printerdrv_lib) $(rs232drv_lib) $(diskimage_lib) $(fsdevice_lib) $(tape_lib) $(imagecontents_lib) $(fileio_lib) $(serial_lib) $(core_lib)

if HAVE_RAWDRIVE
blockdev_lib = @ARCH_DIR@/blockdev.o
else
blockdev_lib =
endif

if HAVE_REALDEVICE
realdevice_lib = @ARCH_DIR@/opencbmlib.o
else
realdevice_lib =
endif

if SUPPORT_X64DTV
bin_PROGRAMS = x64 x128 x64dtv xvic xpet xplus4 xcbm2 c1541 petcat cartconv
else
bin_PROGRAMS = x64 x128 xvic xpet xplus4 xcbm2 c1541 petcat cartconv
endif

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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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
	x64$(EXEEXT)

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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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
	$(c64c128_lib) \
	$(c64cart_lib) \
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
	@RESID_DEP@ \
	@RESID_FP_DEP@ \
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
	x128$(EXEEXT)

# xvic
xvic_libs =  \
	@XVIC_WINRES@ \
	$(vic20_lib) \
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
	$(sid_lib) \
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

xvic_SOURCES = $(base_sources) $(maincpu_sources) $(mouse_sources) $(midi_sources)
xvic_LDADD = $(xvic_libs) $(emu_extlibs) @NETPLAY_LIBS@
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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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
	$(raster_lib) \
	$(video_lib) \
	$(sid_lib) \
	$(common_libs) \
	$(resid_libs)

xpet_SOURCES = $(base_sources) $(maincpu_sources) $(mouse_sources)
xpet_LDADD = $(xpet_libs) $(emu_extlibs) @NETPLAY_LIBS@
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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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
	$(video_lib) \
	$(common_libs) \
	$(resid_libs)

xplus4_SOURCES = $(base_sources) $(mouse_sources)
xplus4_LDADD = $(xplus4_libs) $(emu_extlibs) @NETPLAY_LIBS@
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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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
xcbm2_LDADD = $(xcbm2_libs) $(emu_extlibs) @NETPLAY_LIBS@
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
	@VICE_LIBPNG_DEP@ \
	@VICE_ZLIB_DEP@ \
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

# FIXME: I don't like linking `archdep.o' this way, but we cannot use
# the generic `ARCH_LIBS', because we would have to link against X11
# libs too then.
c1541_LDADD = \
	@ARCH_DIR@/archdep.o \
	$(blockdev_lib) \
	$(realdevice_lib) \
	$(vdrive_lib) \
	$(imagecontents_lib) \
	$(fileio_lib) \
	$(diskimage_lib) \
	$(tape_lib) \
	$(serial_lib) \
	@INTLLIBS@ \
	@READLINE@ @READLINE_LIBS@ \
	@VICE_ZLIB@ @ZLIB_LIBS@ @DYNLIB_LIBS@

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
	$(fileio_lib) \
	@INTLLIBS@ \
	@VICE_ZLIB@ @ZLIB_LIBS@

# cartconv
cartconv_SOURCES = cartconv.c
cartconv_LDADD = @INTLLIBS@

# Special targets to build libraries in subdirectories.

.PHONY: libc64 libc128 libc64dtv libc64dtvstubs libvic20 libpet libplus4 libcbm2 libdrive libvdrive
	libprinterdrv librs232drv libgfxoutputdrv libsounddrv libdiskimage
	libfsdevice libtape libarch libvicii libviciidtv libvdc libsid libcrtc libvideo
	libserial libparallel libcore libiecbus libfileio @RESID_DEP@ @RESID_DTV_DEP@ @RESID_FP_DEP@
	@VICE_LIBPNG_DEP@ @VICE_ZLIB_DEP@

libarch:
	(cd arch && $(MAKE))
libc128:
	(cd c128 && $(MAKE))
libc64:
	(cd c64 && $(MAKE) libc64.a)
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
libpng:
	(cd lib/lpng && $(MAKE))
libprinterdrv:
	(cd printerdrv && $(MAKE))
librs232drv:
	(cd rs232drv && $(MAKE))
libraster:
	(cd raster && $(MAKE) libraster.a)
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
libvicii:
	(cd vicii && $(MAKE) libvicii.a)
libviciidtv:
	(cd vicii && $(MAKE) libviciidtv.a)
libvideo:
	(cd video && $(MAKE))
libz:
	(cd lib/zlib && $(MAKE))
