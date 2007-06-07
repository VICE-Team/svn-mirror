# Makefile.am for the Mac OS X library `libarchmacosxcocoadialog.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	@FFMPEG_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/raster \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarchmacosxcocoadialog.a

noinst_HEADERS = \
	drivesettingswindowcontroller.h \
	joysticksettingswindowcontroller.h \
	viceresourcewindowcontroller.h \
	viceinformationwindowcontroller.h
   
libarchmacosxcocoadialog_a_SOURCES = \
	drivesettingswindowcontroller.m \
	joysticksettingswindowcontroller.m \
	viceresourcewindowcontroller.m \
	viceinformationwindowcontroller.m
