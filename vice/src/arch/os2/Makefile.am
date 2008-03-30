# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = snippets vac++ kbd dialogs ui 

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
        blockdev.c \
	config.h \
	console.c \
        fullscr.c \
        fullscr.h \
        fullscros2.h \
        gradd.h \
	joy.c \
	joy.h \
	kbd.c \
	kbd.h \
        makedist.cmd \
        mouse.c \
        mouse.h \
	os2main.c \
	rs232.c \
	signals.h \
	sounddrv.c \
	sounddrv.h \
	types.h \
	ui.c \
	ui.h \
	ui_status.h \
	uicmdline.c \
	uicmdline.h \
	uimon.c \
	video.c \
	videoarch.h \
	vsidui.c \
	vsyncarch.c

EXTRA_DIST = \
	vice.def \
	install.cmd \
	makedist.cmd \
	makeclean.cmd \
	unused/pm/winaddon.c \
	unused/pm/winaddon.h \
	unused/pm/scrollbars.c \
	unused/pm/scrollbars.h \
	unused/video_gpi.h \
	unused/video_gpi.c \
	unused/pm_cmdline.c \
	unused/pm_cmdline.h \
	unused/vsync.c \
	unused/vsync.h \
	unused/ui_status.c \
	icons/c1541.ico \
	icons/check.ico \
        icons/dirup.bmp \
        icons/exit.bmp \
        icons/ffwd.bmp \
	icons/folder.pal \
	icons/folder1.ico \
	icons/folder2.ico \
	icons/info.ico \
 	icons/note.ico \
        icons/pause.bmp \
        icons/playback.bmp \
        icons/record.bmp \
        icons/return.bmp \
        icons/skull.ico \
        icons/stepinto.bmp \
        icons/stepover.bmp \
        icons/stop.bmp \
        icons/vsid.ico \
	icons/x128.ico \
	icons/x64.ico \
	icons/xcbm2.ico \
	icons/xpet.ico \
        icons/xplus4.ico \
        icons/xvic.ico \
	texi2ipf/readme \
	texi2ipf/texi2ipf.exe \
	texi2ipf/texi2ipf.INF \
        doc/vice2.ipf \
	doc/Vice_for_OS2.txt \
        fonts/header.fnt \
        fonts/header1.fnt \
        fonts/header2.fnt \
	fonts/bdf2fnt.cc \
	fonts/bdf2fnt.icc \
	fonts/chargen2fnt.cc \
	fonts/chargen2fnt.icc \
        fonts/mps2chargen.cc \
        fonts/mps2chargen.icc \
        fonts/mps803.fnt \
	fonts/vice-cbm.cmd \
	fonts/vice2.asm \
	fonts/vice2.def \
	fonts/vice2.rc

