# We have to override the automake default, because we need to use
# $(CXX) instead of $(CC) when compiling with reSID.
LINK = @LINKCC@ $(CFLAGS) $(INCLUDES) $(CPPFLAGS) -o $@

if HAVE_RS232
if HAVE_PRINTER
rs232_printer_ldadd = pruser.o prdevice.o rsuser.o
else
rs232_printer_ldadd = rsuser.o
endif
else
if HAVE_PRINTER
rs232_printer_ldadd = pruser.o prdevice.o
else
rs232_printer_ldadd =
endif
endif

ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = \
	@RESID_DIR@ \
	sounddrv \
	drive \
	raster \
	vicii \
	crtc \
	sid \
	c64 \
	c128 \
	vic20 \
	pet \
	cbm2 \
	arch

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/raster

YFLAGS = -dt

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
	archapi.h \
	asm.h \
	attach.h \
	autostart.h \
	cartridge.h \
	charsets.h \
	cia.h \
	ciacore.h \
	clkguard.h \
	ciatimer.h \
	cmdline.h \
	config.h.in \
	datasette.h \
	emuid.h \
	fixpoint.h \
	findpath.h \
	fs_cbm.h \
	fsdevice.h \
	fliplist.h \
	gcr.h \
	iecdrive.h \
	imagecontents.h \
	info.h \
	interrupt.h \
	kbdbuf.h \
	keyboard.h \
	log.h \
	machine.h \
	maincpu.h \
	mem.h \
	mon.h \
	mon_parse.h \
	mos6510.h \
	p00.h \
	palette.h \
	parallel.h \
	piacore.h \
	print.h \
	resources.h \
	riot.h \
	riotcore.h \
	romset.h \
	rs232.h \
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
	usleep.h \
	utils.h \
	vdrive.h \
	vdrive-iec.h\
	via.h \
	viacore.h \
	vice.h \
	warn.h \
	zfile.h \
	zipcode.h

base_sources = \
	alarm.c \
	asm.c \
	attach.c \
	autostart.c \
	charsets.c \
	ciatimer.c \
	clkguard.c \
	cmdline.c \
	datasette.c \
	emuid.c \
	findpath.c \
	fsdevice.c \
	fliplist.c \
	gcr.c \
	info.c \
	interrupt.c \
	imagecontents.c \
	kbdbuf.c \
	keyboard.c \
	log.c \
	main.c \
	mon.c \
	mon_lex.l \
	mon_parse.y \
	p00.c \
	palette.c \
	parallel.c \
	resources.c \
	romset.c \
	serial.c \
	snapshot.c \
	sound.c \
	sysfile.c \
	t64.c \
	tap.c \
	tape.c \
	traps.c \
	utils.c \
	vdrive.c \
	vdrive-iec.c \
	warn.c \
	zfile.c \
	zipcode.c

cpucore_sources = \
	6510core.c

maincpu_sources = \
	maincpu.c

printer_sources = \
	prdevice.c \
	prdevice.h \
	pruser.c \
	pruser.h

rs232_sources = \
	rsuser.c \
	rsuser.h

EXTRA_DIST = \
	6510core.c \
	ChangeLog \
	ChangeLog.1 \
	ChangeLog.2 \
	aciacore.c \
	ciacore.c \
	convio \
	fixpoint.c \
	piacore.c \
	riotcore.c \
	tpicore.c \
	viacore.c \
	$(printer_sources) \
	$(rs232_sources)

BUILT_SOURCES = mon_parse.h mon_parse.c 

c128_lib = $(top_builddir)/src/c128/libc128.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
drive_lib = $(top_builddir)/src/drive/libdrive.a
pet_lib = $(top_builddir)/src/pet/libpet.a
raster_lib = $(top_builddir)/src/raster/libraster.a
sid_lib = $(top_builddir)/src/sid/libsid.a
sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
crtc_lib = $(top_builddir)/src/crtc/libcrtc.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a

common_libs = $(rs232_printer_ldadd) @ALLOCA@ @ARCH_LIBS@ @LIBOBJS@ @READLINE@ @RESID_LIBS@ @SOUND_LIBS@ 

bin_PROGRAMS = x64 x128 xvic xpet xcbm2 c1541 petcat

EXTRA_PROGRAMS =

# x64
x64_libs =  \
	@X64_WINRES@ \
	$(c64_lib) \
	$(drive_lib) \
	$(sid_lib) \
	$(sounddrv_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(common_libs)

x64_SOURCES = $(base_sources) $(maincpu_sources)
x64_LDADD = $(x64_libs) @X_LIBS@
x64_DEPENDENCIES = $(x64_libs)
x64_LDFLAGS = @x64_LDFLAGS@

.PHONY: x64-all
x64-all: \
	@RESID_DEP@ \
	libc64 \
	libdrive \
	libraster \
	libsid \
	libsounddrv \
	libvicii \
	libarch \
	x64

# x128
x128_libs =  \
	@X128_WINRES@ \
	$(c128_lib) \
	$(c64c128_lib) \
	$(drive_lib) \
	$(sid_lib) \
	$(sounddrv_lib) \
	$(vicii_lib) \
	$(raster_lib) \
	$(common_libs)

x128_SOURCES = $(base_sources)
x128_LDADD = $(x128_libs) @X_LIBS@
x128_DEPENDENCIES = $(x128_libs)
x128_LDFLAGS = @x128_LDFLAGS@

.PHONY: x128-all
x128-all: libc128 libc64c128 libsounddrv libdrive libarch libsid \
	libvicii @RESID_DEP@ x128

# xvic
xvic_libs =  \
	@XVIC_WINRES@ \
	$(vic20_lib) \
	$(drive_lib) \
	$(sounddrv_lib) \
	$(raster_lib) \
	$(common_libs)

xvic_SOURCES = $(base_sources) $(maincpu_sources)
xvic_LDADD = $(xvic_libs) @X_LIBS@
xvic_DEPENDENCIES = $(xvic_libs)
xvic_LDFLAGS = @xvic_LDFLAGS@

.PHONY: xvic-all
xvic-all: \
	libdrive \
	libraster \
	libsounddrv \
	libvic20 \
	libarch \
    	xvic

# xpet
xpet_libs =  \
	@XPET_WINRES@ \
	$(drive_lib) \
	$(pet_lib) \
	$(sounddrv_lib) \
	$(crtc_lib) \
	$(raster_lib) \
	$(common_libs)
	
xpet_SOURCES = $(base_sources) $(maincpu_sources)
xpet_LDADD = $(xpet_libs) @X_LIBS@
xpet_DEPENDENCIES = $(xpet_libs) 
xpet_LDFLAGS = @xpet_LDFLAGS@

.PHONY: xpet-all
xpet-all: \
	libdrive \
	libraster \
	libcrtc \
	libsounddrv \
	libpet \
	libarch \
	xpet 

# xcbm2
xcbm2_libs = \
	@XCBM2_WINRES@ \
	$(drive_lib) \
	$(xcbm2_lib) \
	$(sounddrv_lib) \
	$(sid_lib) \
	$(crtc_lib) \
	$(raster_lib) \
	$(common_libs)

xcbm2_SOURCES = $(base_sources)
xcbm2_LDADD = $(xcbm2_libs) @X_LIBS@
xcbm2_DEPENDENCIES = $(xcbm2_libs) 
xcbm2_LDFLAGS = @xcbm2_LDFLAGS@

.PHONY: xcbm2-all
xcbm2-all: \
	libcbm2 \
	libsounddrv \
	libarch \
	libsid \
	@RESID_DEP@ \
	libdrive \
	libraster \
	libcrtc \
	xcbm2

# c1541
c1541_SOURCES = \
	c1541.c \
	charsets.c \
	cmdline.c \
	findpath.c \
	gcr.c \
	info.c \
	log.c \
	p00.c \
	resources.c \
	t64.c \
	utils.c \
	vdrive-iec.c \
	vdrive.c \
	zfile.c \
	zipcode.c

# FIXME: I don't like linking `archdep.o' this way, but we cannot use
# the generic `ARCH_LIBS', because we would have to link against X11
# libs too then.
c1541_LDADD = @ARCH_DIR@/archdep.o @READLINE@

# petcat
petcat_SOURCES = \
	charsets.c \
	cmdline.c \
	findpath.c \
	log.c \
	p00.c \
	petcat.c \
	resources.c \
	utils.c \
	zfile.c \
	zipcode.c

petcat_LDADD = @ARCH_DIR@/archdep.o

# Special targets to build libraries in subdirectories.

.PHONY: libc64 libc128 libvic20 libpet libcbm2 libdrive libsounddrv libarch \
	libvicii libsid libcrtc @RESID_DEP@

libarch:
	(cd arch && $(MAKE))
libc128:
	(cd c128 && $(MAKE))
libc64:
	(cd c64 && $(MAKE) libc64.a)
libc64c128:
	(cd c64 && $(MAKE) libc64c128.a)
libcbm2:
	(cd cbm2 && $(MAKE))
libdrive:
	(cd drive && $(MAKE))
libpet:
	(cd pet && $(MAKE))
libraster:
	(cd raster && $(MAKE))
libresid:
	(cd resid && $(MAKE))
libsid:
	(cd sid && $(MAKE))
libsounddrv:
	(cd sounddrv && $(MAKE))
libvic20:
	(cd vic20 && $(MAKE))
libvicii:
	(cd vicii && $(MAKE))
libcrtc:
	(cd crtc && $(MAKE))
