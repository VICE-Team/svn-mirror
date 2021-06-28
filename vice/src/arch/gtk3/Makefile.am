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
	-I$(top_srcdir)/src/datasette \
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

AM_OBJCFLAGS = @VICE_OBJCFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libarch.a libtoolarch.a

libarch_a_SOURCES = \
	archdep.c \
	c128ui.c \
	c64dtvui.c \
	c64scui.c \
	c64ui.c \
	cbm2ui.c \
	cbm5x0ui.c \
	gtk3main.c \
	kbd.c \
	linenoise.c \
	mousedrv.c \
	petui.c \
	plus4ui.c \
	render_queue.c \
	render_thread.c \
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

if NATIVE_GTK3_COMPILE
libarch_a_SOURCES += vice_gtk3_locking.c
endif

if WIN32_COMPILE
libarch_a_SOURCES += directx_renderer.c directx_renderer_impl.cc
else
libarch_a_SOURCES += opengl_renderer.c
if UNIX_MACOSX_COMPILE
libarch_a_SOURCES += opengl_renderer_macos.m
else
libarch_a_SOURCES += opengl_renderer_unix.c
endif
endif

libtoolarch_a_SOURCES = \
	archdep.c

EXTRA_DIST = \
	Resources/VICE.icns \
	archdep.h \
	debug_gtk3.h \
	directx_renderer.h \
	directx_renderer_impl.h \
	kbd.h \
	linenoise.h \
	macOS-ui-runtime.sh \
	make-bindist_osx.sh \
	make-bindist_win32.sh \
	mousedrv.h \
	opengl_renderer.h \
	render_queue.h \
	render_thread.h \
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
	vice_gtk3_locking.h \
	vice-launcher.sh \
	videoarch.h \
	winjoy.h \
	x11-launcher.sh
