# vim: set noet ts=8 sw=8 sts=8:
commondir = $(VICE_DATADIR)/common

common_DATA = CBM_Logo.svg \
	      C64_Pro_Mono-STYLE.ttf \
	      icon1024x1024.png \
	      Icon-128@2x.png \
	      vice-logo-black.svg \
	      C128_16.png \
	      C128_32.png \
	      C128_48.png \
	      C128_256.png \
	      C128_1024.svg \
	      C64_16.png \
	      C64_32.png \
	      C64_48.png \
	      C64_256.png \
	      C64_1024.svg \
	      CBM2_16.png \
	      CBM2_32.png \
	      CBM2_48.png \
	      CBM2_256.png \
	      CBM2_1024.svg \
	      DTV_16.png \
	      DTV_32.png \
	      DTV_48.png \
	      DTV_256.png \
	      DTV_1024.svg \
	      PET_16.png \
	      PET_32.png \
	      PET_48.png \
	      PET_256.png \
	      PET_256.svg \
	      Plus4_16.png \
	      Plus4_32.png \
	      Plus4_48.png \
	      Plus4_256.png \
	      Plus4_1024.svg \
	      SCPU_16.png \
	      SCPU_32.png \
	      SCPU_48.png \
	      SCPU_256.png \
	      SCPU_1024.svg \
	      SID_16.png \
	      SID_32.png \
	      SID_48.png \
	      SID_256.png \
	      SID_1024.svg \
	      VIC20_16.png \
	      VIC20_32.png \
	      VIC20_48.png \
	      VIC20_256.png \
	      VIC20_1024.svg \
	      x64.rc \
	      x64sc.rc \
	      x64dtv.rc \
	      xscpu64.rc \
	      x128.rc \
	      xvic.rc \
	      xplus4.rc \
	      xpet.rc \
	      xcbm2.rc \
	      xcbm5x0.rc \
	      vsid.rc


EXTRA_DIST = $(common_DATA)


#all: x64.res x64sc.res x64dtv.res xscpu64.res x128.res xvic.res xplus4.res xpet.res xcbm2.res xcbm5x0.res vsid.res $(GRESOURCE_TARGET)
#

if NATIVE_GTK3_COMPILE

# Generate gresource binary file, required by all Gtk3 emus
GRESOURCE_TARGET = $(builddir)/vice.gresource
GRESOURCE_XML = $(srcdir)/vice.gresource.xml

$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	glib-compile-resources --sourcedir $(top_srcdir) $< --target $@

common_DATA += $(GRESOURCE_TARGET)


CLEANFILES = \
	$(GRESOURCE_TARGET)

EXTRA_DIST += $(GRESOURCE_XML)

endif




