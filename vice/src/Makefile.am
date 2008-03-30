ARCH_LIBS = @ARCH_LIBS@

SUBDIRS = sounddrv true1541 c64 c128 vic20 pet arch

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-DPREFIX=\"@prefix@\"

YFLAGS = -dt

noinst_HEADERS = \
	6510core.h \
	acia.h \
	asm.h \
	attach.h \
	autostart.h \
	c1541.h \
	cartridge.h \
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
	rs232.h \
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
	cartridge.c \
	charsets.c \
	cmdline.c \
	drive.c \
	findpath.c \
	fsdevice.c \
	gcr.c \
	info.c \
	kbdbuf.c \
	main.c \
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

maincpu_sources = \
	maincpu.c

EXTRA_DIST = \
	6510core.c \
	ChangeLog \
	ChangeLog.1 \
	acia-tmpl.c \
	cia-tmpl.c \
	convio \
	raster.c \
	via-tmpl.c

BUILT_SOURCES = mon_parse.h mon_parse.c

sounddrv_lib = $(top_builddir)/src/sounddrv/libsounddrv.a
true1541_lib = $(top_builddir)/src/true1541/libtrue1541.a
c64_lib = $(top_builddir)/src/c64/libc64.a
c64c128_lib = $(top_builddir)/src/c64/libc64c128.a
c128_lib = $(top_builddir)/src/c128/libc128.a
vic20_lib = $(top_builddir)/src/vic20/libvic20.a
pet_lib = $(top_builddir)/src/pet/libpet.a

bin_PROGRAMS = x64 x128 xvic xpet c1541 petcat


# x64
x64_SOURCES = $(base_sources) $(maincpu_sources)
x64_LDADD = $(c64_lib) $(sounddrv_lib) $(true1541_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @READLINE@ -lm
x64_DEPENDENCIES = $(c64_lib) $(sounddrv_lib) $(true1541_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@

# x128
x128_SOURCES = $(base_sources)
x128_LDADD = $(c128_lib) $(c64c128_lib) $(sounddrv_lib) $(true1541_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @READLINE@ -lm
x128_DEPENDENCIES = $(c128_lib) $(c64c128_lib) $(sounddrv_lib) \
	$(true1541_lib) @ALLOCA@ @LIBOBJS@ @ARCH_LIBS@

# xvic
xvic_SOURCES = $(base_sources) $(maincpu_sources)
xvic_LDADD = $(vic20_lib) $(sounddrv_lib) $(true1541_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @READLINE@ -lm
xvic_DEPENDENCIES = $(vic20_lib) $(sounddrv_lib) $(true1541_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@

# xpet
xpet_SOURCES = $(base_sources) $(maincpu_sources)
xpet_LDADD = $(pet_lib) $(sounddrv_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@ @SOUND_LIBS@ @X_LIBS@ @READLINE@ -lm
xpet_DEPENDENCIES = $(pet_lib) $(sounddrv_lib) \
	@ALLOCA@ @LIBOBJS@ @ARCH_LIBS@

# c1541
c1541_SOURCES = \
	c1541.c \
	charsets.c \
	drive.c \
	gcr.c \
	mshell.c \
	tape.c \
	utils.c \
	zfile.c \
	zipcode.c

c1541_LDADD = @READLINE@

# petcat
petcat_SOURCES = \
	charsets.c \
	petcat.c \
	tape.c \
	utils.c \
	zfile.c \
	zipcode.c

# Special targets to build libraries in subdirectories.

.PHONY: libc64 libc128 libvic20 libpet libtrue1541 libsounddrv
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
libtrue1541:
	(cd true1541 && $(MAKE))
libsounddrv:
	(cd sounddrv && $(MAKE))
libarch:
	(cd arch && $(MAKE))
