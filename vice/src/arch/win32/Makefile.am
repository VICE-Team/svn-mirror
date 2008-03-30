## Makefile.am for the Win32 version of `libarch.a'.

SUFFIXES = .rc

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	c128kbd.c \
	c128ui.c \
	c128ui.h \
	c610kbd.c \
	c610ui.c \
	c610ui.h \
	c64kbd.c \
	c64ui.c \
	c64ui.h \
	dirent.c \
	dirent.h \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	lose32.h \
	petkbd.c \
	petui.c \
	petui.h \
	print.c \
	res.h \
	res.rc \
	resc128.rc \
	resc64.rc \
	rescbm2.rc \
	respet.rc \
	resvic.rc \
	types.h \
	ui.c \
	ui.h \
	uiattach.c \
	uiattach.h \
	uicbm2set.c \
	uicbm2set.h \
	uidrive.c \
	uidrive.h \
	uijoystick.c \
	uijoystick.h \
	uilib.c \
	uilib.h \
	uipetset.c \
	uipetset.h \
	uisnapshot.h \
	uisnapshot.c \
	uisound.c \
	uisound.h \
	uivicii.h \
	uivicii.c \
	vic20kbd.c \
	vic20ui.c \
	vic20ui.h \
	video.c \
	video.h \
	vsync.c \
	vsync.h \
	winhack.h \
	winmain.c \
	winmain.h

libarch_a_DEPENDENCIES = \
	resc128.o resc64.o rescbm2.o respet.o resvic.o

EXTRA_DIST = \
	msvc/config.h \
	arch.dsp \
	c128.dsp \
	c1541.dsp \
	c64.dsp \
	c64c128.dsp \
	cbm2.dsp \
	crtc.dsp \
	drive.dsp \
	pet.dsp \
	petcat.dsp \
	raster.dsp \
	resid.dsp \
	sid.dsp \
	sounddrv.dsp \
	vic20.dsp \
	vice.dsp \
	vicii.dsp \
	x128.dsp \
	x64.dsp \
	xcbm2.dsp \
	xpet.dsp \
	xvic.dsp \
	vice.dsw \
        c64.ico 

resc128.o : resc128.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resc64.o : resc64.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
rescbm2.o : rescbm2.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
respet.o : respet.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
resvic.o : resvic.rc res.rc res.h
	$(WINDRES) -i $< -o $@ --include-dir $(top_builddir)/src \
		--include-dir $(top_srcdir)/src/arch/win32
