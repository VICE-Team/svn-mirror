# Makefile.am for the Mac OS X library `libarchmacosxcocoamenu.a'.

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

noinst_LIBRARIES = libarchmacosxcocoamenu.a

noinst_HEADERS = \
	c64controller.h \
	c128controller.h \
	c64machinecontroller.h \
	c128machinecontroller.h \
	viceappcontroller.h \
	vicemachinecontroller.h
   
libarchmacosxcocoamenu_a_SOURCES = \
	c128ui.m \
	c64ui.m \
	cbm2ui.m \
	petui.m \
	plus4ui.m \
	vic20ui.m \
	c64controller.m \
	c64machinecontroller.m \
	c128controller.m \
	c128machinecontroller.m \
	viceappcontroller.m \
	vicemachinecontroller.m
