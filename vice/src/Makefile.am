ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = sounddrv true1541 c64 c128 vic20 pet arch

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-DPREFIX=\"@prefix@\"

noinst_HEADERS = \
	6510core.h \
	asm.h \
	attach.h \
	autostart.h \
	c1541.h \
	charsets.h \
	cia.h \
	cmdline.h \
	config.h.in \
	drive.h \
	file.h \
	findpath.h \
	fs_cbm.h \
	fsdevice.h \
	gcr.h \
	icon.h \
	iecdrive.h \
	info.h \
	interrupt.h \
	kbdbuf.h \
	machine.h \
	maincpu.h \
	mem.h \
	memutils.h \
	misc.h \
	mon.h \
	mon_parse.h \
	mos6510.h \
	mshell.h \
	op.h \
	palette.h \
	parallel.h \
	printer.h \
	raster.h \
	resources.h \
	serial.h \
	sound.h \
	sysfile.h \
	tape.h \
	tapeunit.h \
	traps.h \
	types.h \
	utils.h \
	via.h \
	vice.h \
	vmachine.h \
	warn.h \
	zfile.h \
	zipcode.h

base_sources = \
	asm.c \
	attach.c \
	autostart.c \
	charsets.c \
	cmdline.c \
	drive.c \
	findpath.c \
	fsdevice.c \
	gcr.c \
	info.c \
	kbdbuf.c \
	main.c \
	maincpu.c \
	memutils.c \
	misc.c \
	mon.c \
	mon_lex.l \
	mon_parse.y \
	mshell.c \
	palette.c \
	parallel.c \
	resources.c \
	serial.c \
	sound.c \
	sysfile.c \
	tape.c \
	tapeunit.c \
	traps.c \
	utils.c \
	warn.c \
	zfile.c \
	zipcode.c

EXTRA_DIST = \
	6510core.c \
	ChangeLog \
	ChangeLog.1 \
	cia-tmpl.c \
	convio \
	raster.c \
	via-tmpl.c

sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
true1541_lib = $(top_builddir)/src/true1541/libtrue1541.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c128_lib = $(top_builddir)/src/c128/libc128.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
pet_lib = $(top_builddir)/src/pet/libpet.a

bin_PROGRAMS = x64 x128 xvic xpet

# x64
x64_SOURCES = $(base_sources)
x64_LDADD = $(c64_lib) $(sounddrv_lib) $(true1541_lib) \
	@ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ -lm
x64_DEPENDENCIES = $(c64_lib) $(sounddrv_lib) $(true1541_lib) @ARCH_LIBS@

# x128
# Notice that this works by overriding some symbols from the C64 library.
x128_SOURCES = $(base_sources)
x128_LDADD = $(c128_lib) $(c64_lib) $(sounddrv_lib) $(true1541_lib) \
	@ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ -lm
x128_DEPENDENCIES = $(c128_lib) $(c64_lib) $(sounddrv_lib) $(true1541_lib) \
	@ARCH_LIBS@

# xvic
xvic_SOURCES = $(base_sources)
xvic_LDADD = $(vic20_lib) $(sounddrv_lib) $(true1541_lib) \
	@ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ -lm
xvic_DEPENDENCIES = $(vic20_lib) $(sounddrv_lib) $(true1541_lib) @ARCH_LIBS@

# xpet
xpet_SOURCES = $(base_sources)
xpet_LDADD = $(pet_lib) $(sounddrv_lib) \
	@ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ -lm
xpet_DEPENDENCIES = $(pet_lib) $(sounddrv_lib) @ARCH_LIBS@


# Special targets for building the subdirectories.  This only works
# with GNU make, though.  FIXME?

.PHONY: c64 c128 vic20 pet true1541 sounddrv
c64:
	(cd c64 && $(MAKE))
c128:
	(cd c128 && $(MAKE))
vic20:
	(cd vic20 && $(MAKE))
pet:
	(cd pet && $(MAKE))
true1541:
	(cd true1541 && $(MAKE))
sounddrv:
	(cd sounddrv && $(MAKE))
