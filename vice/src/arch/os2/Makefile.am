# Makefile.am for the OS2 version of `libarch.a'.

SUBDIRS = snippets vac++ kbd dialogs ui 

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/monitor

noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	archdep.h \
	config.h \
	console.c \
	joystick.c \
	joystick.h \
	kbd.c \
	kbd.h \
	main_exit.c \
	main_exit.h \
        makedist.cmd \
        mouse.c \
        mouse.h \
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
	video.c \
	videoarch.h \
	vsidui.c \
	vsync.c 

EXTRA_DIST = \
	vice.def \
        install.cmd \
        makedist.cmd \
        makeclean.cmd \
	unused/pm/winaddon.c \
	unused/pm/winaddon.h \
	unused/pm/scrollbars.c \
	unused/pm/scrollbars.h \
        unused/contentsdlg.c \
        unused/contentsdlg.h \
        unused/video_gpi.h \
        unused/video_gpi.c \
        unused/pm_cmdline.c \
        unused/pm_cmdline.h \
        unused/vsync.c \
        unused/vsync.h \
        unused/ui_status.c \
        icons/folder.pal \
        icons/folder1.ico \
        icons/folder2.ico \
        icons/x64.ico \
        icons/xpet.ico \
        icons/x128.ico \
        icons/xcbm2.ico \
        icons/c1541.ico \
        texi2ipf/readme \
        texi2ipf/texi2ipf.exe \
        texi2ipf/texi2ipf.INF \
        doc/Vice_for_OS2.txt

