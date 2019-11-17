## Makefile.am for the native GTK3 version of `libarch.a'.

if WIN32_COMPILE
extra_includes = -I$(top_srcdir)/src/arch/mingw32-pcap/wpcap
else
extra_includes =
endif

SUBDIRS = data novte widgets

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	$(extra_includes) \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/crtc \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/joyport \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src/rs232drv \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vdc \
	-I$(top_srcdir)/src/arch/gtk3/widgets \
	-I$(top_srcdir)/src/arch/gtk3/widgets/base \
	-I$(top_srcdir)/src/arch/gtk3/novte \
	-I$(top_builddir)/src/arch/gtk3/novte \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c64dtv \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/plus4 \
	-I$(top_srcdir)/src/scpu64 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/samplerdrv \
	-I$(top_srcdir)/src/c64/cart \
	-I$(top_srcdir)/src/imagecontents \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/tapeport \
	-I$(top_srcdir)/src/hvsc

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@


noinst_LIBRARIES = libarch.a

libarch_a_SOURCES = \
	archdep.c \
	c128ui.c \
	c64dtvui.c \
	c64scui.c \
	c64ui.c \
	cairo_renderer.c \
	cbm2ui.c \
	cbm5x0ui.c \
	gtk3main.c \
	joy-osx-hid.c \
	joy-osx-hidmgr.c \
	joy-osx-hidutil.c \
	joy-osx.c \
	joy-unix-usb.c \
	joy-unix.c \
	joy-win32-dinput-handle.c \
	joy-win32.c \
	kbd.c \
	linenoise.c \
	mousedrv.c \
	opengl_renderer.c \
	petui.c \
	plus4ui.c \
	quartz_renderer.c \
	scpu64ui.c \
	ui.c \
	uiabout.c \
	uicart.c \
	uicmdline.c \
	uicommands.c \
	uicompiletimefeatures.c \
	uidata.c \
	uidatasette.c \
	uidebug.c \
	uidiskattach.c \
	uidiskcreate.c \
	uiedit.c \
	uifliplist.c \
	uihotkeys.c \
	uimachinemenu.c \
	uimachinewindow.c \
	uimedia.c \
	uimenu.c \
	uimon-fallback.c \
	uimon.c \
	uinetplay.c \
	uinetplay_new.c \
	uisettings.c \
	uisidattach.c \
	uismartattach.c \
	uisnapshot.c \
	uistatusbar.c \
	uitapeattach.c \
	uitapecreate.c \
	uivideo.c \
	uivsidmenu.c \
	uivsidwindow.c \
	vic20ui.c \
	video.c \
	vsidui.c \
	vsyncarch.c

EXTRA_DIST = \
	Info.plist \
	Resources/VICE.icns \
	archdep.h \
	cairo_renderer.h \
	debug_gtk3.h \
	joy-osx-hid.h \
	joy-osx-hidlib.h \
	joy-osx.h \
	joy-unix.h \
	joy-win32-dinput-handle.h \
	joy-win32.h \
	joy.h \
	kbd.h \
	linenoise.h \
	macOS-launcher.sh \
	make-bindist_osx.sh \
	make-bindist_win32.sh \
	mousedrv.h \
	opengl_renderer.h \
	quartz_renderer.h \
	ui.h \
	uiabout.h \
	uicart.h \
	uicmdline.h \
	uicommands.h \
	uicompiletimefeatures.h \
	uidata.h \
	uidatasette.h \
	uidebug.h \
	uidiskattach.h \
	uidiskcreate.h \
	uiedit.h \
	uifliplist.h \
	uihotkeys.h \
	uimachinemenu.h \
	uimachinewindow.h \
	uimedia.h \
	uimenu.h \
	uimon-fallback.h \
	uimonarch.h \
	uinetplay.h \
	uinetplay_new.h \
	uisettings.h \
	uisidattach.h \
	uismartattach.h \
	uisnapshot.h \
	uistatusbar.h \
	uitapeattach.h \
	uitapecreate.h \
	uivideo.h \
	uivsidmenu.h \
	uivsidwindow.h \
	vice-launcher.sh \
	videoarch.h \
	winjoy.h \
	x11-launcher.sh
