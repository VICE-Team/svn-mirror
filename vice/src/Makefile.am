# We have to override the automake default, because we need to use
# $(CXX) instead of $(CC) when compiling with reSID.
LINK = @LINKCC@ $(CFLAGS) $(INCLUDES) $(CPPFLAGS) -o $@

if HAVE_RS232
rs232_ldadd = rsuser.o
else
rs232_ldadd =
endif

ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = \
	@RESID_DIR@ \
	sounddrv \
	drive \
	vdrive \
	fsdevice \
	diskimage \
	tape \
	imagecontents \
	video \
	raster \
	vicii \
        vdc \
	crtc \
	gfxoutputdrv \
	printerdrv \
	sid \
	monitor \
	c64 \
	c128 \
	vic20 \
	pet \
	plus4 \
	cbm2 \
	arch

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/monitor 

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
	archapi.h \
	attach.h \
	autostart.h \
	cartridge.h \
	charset.h \
	cia.h \
	ciacore.h \
	clkguard.h \
	ciatimer.h \
	cmdline.h \
	color.h \
	config.h.in \
	console.h \
	datasette.h \
	debug.h \
	diskconstants.h \
	diskimage.h \
	emuid.h \
	fixpoint.h \
	findpath.h \
	fsdevice.h \
	fliplist.h \
	fullscreen.h \
	gcr.h \
	gfxoutput.h \
	iecdrive.h \
	imagecontents.h \
	info.h \
	init.h \
	initcmdline.h \
	interrupt.h \
	joystick.h \
	kbdbuf.h \
	keyboard.h \
	log.h \
	machine.h \
	maincpu.h \
	mem.h \
	mos6510.h \
	p00.h \
	palette.h \
	parallel.h \
	piacore.h \
	printer.h \
	resources.h \
	riot.h \
	riotcore.h \
	romset.h \
	rs232.h \
	screenshot.h \
	serial.h \
	snapshot.h \
	sound.h \
	sysfile.h \
	t64.h \
	tap.h \
	tape.h \
	tpi.h \
	tpicore.h \
	traps.h \
	uiapi.h \
	uicolor.h \
	uimon.h \
	usleep.h \
	utils.h \
	version.h.in \
	version.h \
	via.h \
	viacore.h \
	vice.h \
	video.h \
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
	ciatimer.c \
	clkguard.c \
	cmdline.c \
	color.c \
	datasette.c \
	debug.c \
	emuid.c \
	findpath.c \
	fliplist.c \
	gcr.c \
	info.c \
	init.c \
	initcmdline.c \
	interrupt.c \
	joystick.c \
	kbdbuf.c \
	keyboard.c \
	log.c \
	machine.c \
	main.c \
	p00.c \
	palette.c \
	parallel.c \
	resources.c \
	romset.c \
	screenshot.c \
	serial.c \
	snapshot.c \
	sound.c \
	sysfile.c \
	traps.c \
	utils.c \
	vsync.c \
	zfile.c \
	zipcode.c

cpucore_sources = \
	6510core.c

maincpu_sources = \
	maincpu.c

rs232_sources = \
	rsuser.c \
	rsuser.h

EXTRA_DIST = \
	6510core.c \
	ChangeLog \
	ChangeLog.1 \
	ChangeLog.2 \
	ChangeLog.3 \
	aciacore.c \
	ciacore.c \
	fixpoint.c \
	piacore.c \
	riotcore.c \
	tpicore.c \
	viacore.c \
	$(rs232_sources)

c128_lib = $(top_builddir)/src/c128/libc128.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
c64cart_lib = $(top_builddir)/src/c64/cart/libc64cart.a
crtc_lib = $(top_builddir)/src/crtc/libcrtc.a
diskimage_lib = $(top_builddir)/src/diskimage/libdiskimage.a
drive_lib = $(top_builddir)/src/drive/libdrive.a
fsdevice_lib = $(top_builddir)/src/fsdevice/libfsdevice.a
gfxoutputdrv_lib = $(top_builddir)/src/gfxoutputdrv/libgfxoutputdrv.a
imagecontents_lib = $(top_builddir)/src/imagecontents/libimagecontents.a
monitor_lib = $(top_builddir)/src/monitor/libmonitor.a
pet_lib = $(top_builddir)/src/pet/libpet.a
plus4_lib = $(top_builddir)/src/plus4/libplus4.a
printerdrv_lib = $(top_builddir)/src/printerdrv/libprinterdrv.a
raster_lib = $(top_builddir)/src/raster/libraster.a
sid_lib = $(top_builddir)/src/sid/libsid.a
sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
tape_lib = $(top_builddir)/src/tape/libtape.a
vdc_lib = $(top_builddir)/src/vdc/libvdc.a
vdrive_lib = $(top_builddir)/src/vdrive/libvdrive.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
video_lib = $(top_builddir)/src/video/libvideo.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a

common_libs = $(rs232_ldadd) @ARCH_LIBS@ @LIBOBJS@ @READLINE@ @RESID_LIBS@ @SOUND_LIBS@

bin_PROGRAMS = x64 x128 xvic xpet xplus4 xcbm2 c1541 petcat

EXTRA_PROGRAMS =

# x64
x64_libs =  \
	@X64_WINRES@ \
	$(c64_lib) \
	$(c64cart_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(sounddrv_lib) \
	$(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

x64_SOURCES = $(base_sources) $(maincpu_sources)
x64_LDADD = $(x64_libs) @X_LIBS@
x64_DEPENDENCIES = $(x64_libs)
x64_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64-all
x64-all: \
	@RESID_DEP@ \
	libc64 \
	libc64cart \
	libdrive \
	libvdrive \
	libraster \
	libsid \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	libvicii \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
	x64

# x128
x128_libs =  \
	@X128_WINRES@ \
	$(c128_lib) \
	$(c64c128_lib) \
	$(c64cart_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sid_lib) \
	$(sounddrv_lib) \
	$(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(vicii_lib) \
	$(vdc_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

x128_SOURCES = $(base_sources)
x128_LDADD = $(x128_libs) @X_LIBS@
x128_DEPENDENCIES = $(x128_libs)
x128_LDFLAGS = @x128_LDFLAGS@

.PHONY: x128-all
x128-all: \
	libc128 \
	libc64c128 \
	libc64cart \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	libdrive \
	libvdrive \
	libarch \
	libsid \
	libvicii \
	libvdc \
	libraster \
	libvideo \
	libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
	@RESID_DEP@ \
	x128

# xvic
xvic_libs =  \
	@XVIC_WINRES@ \
	$(vic20_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sounddrv_lib) \
	$(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

xvic_SOURCES = $(base_sources) $(maincpu_sources)
xvic_LDADD = $(xvic_libs) @X_LIBS@
xvic_DEPENDENCIES = $(xvic_libs)
xvic_LDFLAGS = @xvic_LDFLAGS@

.PHONY: xvic-all
xvic-all: \
	libdrive \
	libvdrive \
	libraster \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	libvic20 \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
	xvic

# xpet
xpet_libs =  \
	@XPET_WINRES@ \
	$(pet_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sounddrv_lib) \
        $(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(crtc_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

xpet_SOURCES = $(base_sources) $(maincpu_sources)
xpet_LDADD = $(xpet_libs) @X_LIBS@
xpet_DEPENDENCIES = $(xpet_libs) 
xpet_LDFLAGS = @xpet_LDFLAGS@

.PHONY: xpet-all
xpet-all: \
	libdrive \
	libvdrive \
	libraster \
	libcrtc \
	libsounddrv \
	libgfxoutputdrv \
	libprinterdrv \
	libpet \
	libvideo \
	libarch \
	libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
	xpet 

# xplus4
xplus4_libs =  \
	@XPLUS4_WINRES@ \
	$(plus4_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sounddrv_lib) \
	$(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

xplus4_SOURCES = $(base_sources) $(maincpu_sources)
xplus4_LDADD = $(xplus4_libs) @X_LIBS@
xplus4_DEPENDENCIES = $(xplus4_libs)
xplus4_LDFLAGS = @xplus4_LDFLAGS@

.PHONY: xplus4-all
xplus4-all: \
        libdrive \
        libvdrive \
        libraster \
        libsounddrv \
        libgfxoutputdrv \
        libprinterdrv \
        libplus4 \
	libvideo \
        libarch \
        libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
        xplus4

# xcbm2
xcbm2_libs = \
	@XCBM2_WINRES@ \
	$(xcbm2_lib) \
	$(drive_lib) \
	$(vdrive_lib) \
	$(sounddrv_lib) \
	$(gfxoutputdrv_lib) \
	$(printerdrv_lib) \
	$(sid_lib) \
	$(crtc_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(video_lib) \
	$(monitor_lib) \
	$(diskimage_lib) \
	$(fsdevice_lib) \
	$(tape_lib) \
	$(imagecontents_lib) \
	$(common_libs)

xcbm2_SOURCES = $(base_sources)
xcbm2_LDADD = $(xcbm2_libs) @X_LIBS@
xcbm2_DEPENDENCIES = $(xcbm2_libs) 
xcbm2_LDFLAGS = @xcbm2_LDFLAGS@

.PHONY: xcbm2-all
xcbm2-all: \
	libcbm2 \
	libsounddrv \
        libgfxoutputdrv \
	libprinterdrv \
	libarch \
	libsid \
	@RESID_DEP@ \
	libdrive \
	libvdrive \
	libraster \
	libcrtc \
	libvicii \
	libvideo \
	libmonitor \
	libdiskimage \
	libfsdevice \
	libtape \
	libimagecontents \
	xcbm2

# c1541
c1541_SOURCES = \
	c1541.c \
	charset.c \
	cmdline.c \
	findpath.c \
	gcr.c \
	info.c \
	log.c \
	p00.c \
	resources.c \
	utils.c \
	zfile.c \
	zipcode.c

# FIXME: I don't like linking `archdep.o' this way, but we cannot use
# the generic `ARCH_LIBS', because we would have to link against X11
# libs too then.
c1541_LDADD = \
	@ARCH_DIR@/archdep.o \
	@ARCH_DIR@/uicmdline.o \
	$(vdrive_lib) \
	$(diskimage_lib) \
	$(tape_lib) \
	@READLINE@

# petcat
petcat_SOURCES = \
	charset.c \
	cmdline.c \
	findpath.c \
	log.c \
	p00.c \
	petcat.c \
	resources.c \
	utils.c \
	zfile.c \
	zipcode.c

petcat_LDADD = \
	@ARCH_DIR@/archdep.o \
	@ARCH_DIR@/uicmdline.o

# Special targets to build libraries in subdirectories.

.PHONY: libc64 libc128 libvic20 libpet libplus4 libcbm2 libdrive libvdrive
	libprinterdrv libgfxoutputdrv libsounddrv libdiskimage libfsdevice
	libtape libarch libvicii libvdc libsid libcrtc libvideo @RESID_DEP@

libarch:
	(cd arch && $(MAKE))
libc128:
	(cd c128 && $(MAKE))
libc64:
	(cd c64 && $(MAKE) libc64.a)
libc64c128:
	(cd c64 && $(MAKE) libc64c128.a)
libc64cart:
	(cd c64 && $(MAKE) libc64cart)
libcbm2:
	(cd cbm2 && $(MAKE))
libcrtc:
	(cd crtc && $(MAKE))
libdiskimage:
	(cd diskimage && $(MAKE))
libdrive:
	(cd drive && $(MAKE))
libgfxoutputdrv:
	(cd gfxoutputdrv && $(MAKE))
libfsdevice:
	(cd fsdevice && $(MAKE))
libimagecontents:
	(cd imagecontents && $(MAKE))
libmonitor:
	(cd monitor && $(MAKE))
libpet:
	(cd pet && $(MAKE))
libplus4:
	(cd plus4 && $(MAKE))
libprinterdrv:
	(cd printerdrv && $(MAKE))
libraster:
	(cd raster && $(MAKE))
libresid:
	(cd resid && $(MAKE))
libsid:
	(cd sid && $(MAKE))
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
	(cd vicii && $(MAKE))
libvideo:
	(cd video && $(MAKE))

