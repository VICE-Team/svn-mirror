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

SUBDIRS = @RESID_DIR@ sounddrv drive c64 c128 vic20 pet cbm2 sid vicii arch

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-DPREFIX=\"@prefix@\"

YFLAGS = -dt

noinst_HEADERS = \
	6510core.h \
	acia.h \
	alarm.h \
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
	crtc.h \
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
	log.h \
	machine.h \
	maincpu.h \
	mem.h \
	memutils.h \
	mon.h \
	mon_parse.h \
	mos6510.h \
	p00.h \
	palette.h \
	parallel.h \
	print.h \
	raster.h \
	resources.h \
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
	traps.h \
	types.h \
	usleep.h \
	utils.h \
	vdrive.h \
	vdrive-iec.h\
	via.h \
	viacore.h \
	vice.h \
	vmachine.h \
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
	log.c \
	main.c \
	memutils.c \
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
	crtccore.c \
	convio \
	fixpoint.c \
	pia-tmpl.c \
	raster.c \
	tpi-tmpl.c \
	viacore.c \
	c1541.dsp \
	petcat.dsp \
	x128.dsp \
	x64.dsp \
	xcbm2.dsp \
	xpet.dsp \
	xvic.dsp \
	$(printer_sources) \
	$(rs232_sources)

BUILT_SOURCES = mon_parse.h mon_parse.c 

sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
drive_lib = $(top_builddir)/src/drive/libdrive.a
sid_lib = $(top_builddir)/src/sid/libsid.a
vicii_lib = $(top_builddir)/src/vicii/libvicii.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
c128_lib = $(top_builddir)/src/c128/libc128.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
pet_lib = $(top_builddir)/src/pet/libpet.a
xcbm2_lib = $(top_builddir)/src/cbm2/libcbm2.a

bin_PROGRAMS = x64 x128 xvic xpet xcbm2 c1541 petcat


# x64
x64_SOURCES = $(base_sources) $(maincpu_sources)
x64_LDADD = $(c64_lib) $(sounddrv_lib) $(drive_lib) $(rs232_printer_ldadd) \
	$(sid_lib) @RESID_LIBS@ $(vicii_lib) @ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ \
	@SOUND_LIBS@ @X_LIBS@ @READLINE@ @LEXLIB@ @X64_WINRES@
x64_DEPENDENCIES = $(c64_lib) $(sounddrv_lib) $(drive_lib) $(sid_lib) \
	@RESID_LIBS@ $(rs232_printer_ldadd) @ALLOCA@ @LIBOBJS@ @ARCH_LIBS@
x64_LDFLAGS = @x64_LDFLAGS@
.PHONY: x64-all
x64-all: libc64 libsounddrv libdrive libarch libsid libvicii @RESID_DEP@ x64

# x128
x128_SOURCES = $(base_sources)
x128_LDADD = $(c128_lib) $(c64c128_lib) $(sounddrv_lib) $(drive_lib) \
	$(rs232_printer_ldadd) $(sid_lib) @RESID_LIBS@ $(vicii_lib) @ALLOCA@ \
	@LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @LEXLIB@ @READLINE@ \
	@X128_WINRES@
x128_DEPENDENCIES = $(c128_lib) $(c64c128_lib) $(sounddrv_lib) \
	$(rs232_printer_ldadd) $(drive_lib) $(sid_lib) @RESID_LIBS@ @ALLOCA@ \
	@LIBOBJS@ @ARCH_LIBS@
x128_LDFLAGS = @x128_LDFLAGS@ 
.PHONY: x128-all
x128-all: libc128 libc64c128 libsounddrv libdrive libarch libsid \
	libvicii @RESID_DEP@ x128

# xvic
xvic_SOURCES = $(base_sources) $(maincpu_sources)
xvic_LDADD = $(vic20_lib) $(sounddrv_lib) $(drive_lib) $(rs232_printer_ldadd) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @LEXLIB@ \
	@READLINE@ @XVIC_WINRES@
xvic_DEPENDENCIES = $(vic20_lib) $(sounddrv_lib) $(drive_lib) \
	$(rs232_printer_ldadd) @ALLOCA@ @LIBOBJS@ @ARCH_LIBS@
xvic_LDFLAGS = @xvic_LDFLAGS@
.PHONY: xvic-all
xvic-all: libvic20 libsounddrv libdrive libarch xvic

# xpet
xpet_SOURCES = $(base_sources) $(maincpu_sources)
xpet_LDADD = $(drive_lib) $(pet_lib) $(sounddrv_lib) $(rs232_printer_ldadd) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @LEXLIB@ \
	@READLINE@ @XPET_WINRES@
xpet_DEPENDENCIES = $(drive_lib) $(pet_lib) $(sounddrv_lib) \
	$(rs232_printer_ldadd) @ALLOCA@ @LIBOBJS@ @ARCH_LIBS@
xpet_LDFLAGS = @xpet_LDFLAGS@
.PHONY: xpet-all
xpet-all: libpet libsounddrv libarch libdrive xpet 

# xcbm2
xcbm2_SOURCES = $(base_sources)
xcbm2_LDADD = $(drive_lib) $(xcbm2_lib) $(sounddrv_lib) \
	$(rs232_printer_ldadd) $(sid_lib) @RESID_LIBS@ @ALLOCA@ @LIBOBJS@ \
	@ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @LEXLIB@ @READLINE@ @XCBM2_WINRES@
xcbm2_DEPENDENCIES = $(drive_lib) $(xcbm2_lib) $(sounddrv_lib) \
	$(rs232_printer_ldadd) $(sid_lib) @RESID_LIBS@ @ALLOCA@ @LIBOBJS@ \
	@ARCH_LIBS@
xcbm2_LDFLAGS = @xcbm2_LDFLAGS@
.PHONY: xcbm2-all
xcbm2-all: libcbm2 libsounddrv libarch libsid @RESID_DEP@ libdrive xcbm2

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
	libsid @RESID_DEP@

libc64:
	(cd c64 && $(MAKE) libc64.a)
libc64c128:
	(cd c64 && $(MAKE) libc64c128.a)
libc128:
	(cd c128 && $(MAKE))
libvic20:
	(cd vic20 && $(MAKE))
libpet:
	(cd pet && $(MAKE))
libcbm2:
	(cd cbm2 && $(MAKE))
libdrive:
	(cd drive && $(MAKE))
libsounddrv:
	(cd sounddrv && $(MAKE))
libarch:
	(cd arch && $(MAKE))
libsid:
	(cd sid && $(MAKE))
libresid:
	(cd resid && $(MAKE))
libvicii:
	(cd vicii && $(MAKE))

