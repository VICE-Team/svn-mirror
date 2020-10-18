# vim: set noet ts=8:
if WIN32_COMPILE

# Create Windows .ico files
#
# Currently the generation of the Windows .res files happens here, I tried to
# do this work in data/common/Makefile.am, so we can use the same files for
# SDL builds, but that didn't quite work out. Probably due to recursive make
# not handling dependencies outside its tree very well (/src vs /data).
#


# TODO: check if we can use patsubst here, to simplify the rules a bit.

X64SC_ICONS = $(top_srcdir)/data/common/C64_16.png \
	      $(top_srcdir)/data/common/C64_24.png \
	      $(top_srcdir)/data/common/C64_32.png \
	      $(top_srcdir)/data/common/C64_48.png \
	      $(top_srcdir)/data/common/C64_256.png

if SUPPORT_X64
X64_ICONS = $(X64SC_ICONS)
endif

X64DTV_ICONS = $(top_srcdir)/data/common/DTV_16.png \
	       $(top_srcdir)/data/common/DTV_24.png \
	       $(top_srcdir)/data/common/DTV_32.png \
	       $(top_srcdir)/data/common/DTV_48.png \
	       $(top_srcdir)/data/common/DTV_256.png

XSCPU64_ICONS = $(top_srcdir)/data/common/SCPU_16.png \
		$(top_srcdir)/data/common/SCPU_24.png \
		$(top_srcdir)/data/common/SCPU_32.png \
		$(top_srcdir)/data/common/SCPU_48.png \
		$(top_srcdir)/data/common/SCPU_256.png

# FIXME: 24px icon missing
X128_ICONS = $(top_srcdir)/data/common/C128_16.png \
	     $(top_srcdir)/data/common/C128_32.png \
	     $(top_srcdir)/data/common/C128_48.png \
	     $(top_srcdir)/data/common/C128_256.png

XVIC_ICONS = $(top_srcdir)/data/common/VIC20_16.png \
	     $(top_srcdir)/data/common/VIC20_24.png \
	     $(top_srcdir)/data/common/VIC20_32.png \
	     $(top_srcdir)/data/common/VIC20_48.png \
	     $(top_srcdir)/data/common/VIC20_256.png

XPLUS4_ICONS = $(top_srcdir)/data/common/Plus4_16.png \
	       $(top_srcdir)/data/common/Plus4_24.png \
	       $(top_srcdir)/data/common/Plus4_32.png \
	       $(top_srcdir)/data/common/Plus4_48.png \
	       $(top_srcdir)/data/common/Plus4_256.png

XPET_ICONS = $(top_srcdir)/data/common/PET_16.png \
	     $(top_srcdir)/data/common/PET_24.png \
	     $(top_srcdir)/data/common/PET_32.png \
	     $(top_srcdir)/data/common/PET_48.png \
	     $(top_srcdir)/data/common/PET_256.png

XCBM2_ICONS = $(top_srcdir)/data/common/CBM2_16.png \
	      $(top_srcdir)/data/common/CBM2_24.png \
	      $(top_srcdir)/data/common/CBM2_32.png \
	      $(top_srcdir)/data/common/CBM2_48.png \
	      $(top_srcdir)/data/common/CBM2_256.png

XCBM5x0_ICONS = $(XCBM2_ICONS)

VSID_ICONS = $(top_srcdir)/data/common/SID_16.png \
	     $(top_srcdir)/data/common/SID_24.png \
	     $(top_srcdir)/data/common/SID_32.png \
	     $(top_srcdir)/data/common/SID_48.png \
	     $(top_srcdir)/data/common/SID_256.png


# Do we use the old x64 emu?
if SUPPORT_X64
X64_RES_FILE = x64.res
else
X64_RES_FILE =
endif


all: $(X64_RES_FILE) x64sc.res x64dtv.res xscpu64.res x128.res xvic.res xplus4.res xpet.res xcbm2.res xcbm5x0.res vsid.res


# Generic rule to create Windows .ico files
%.ico:
	$(ICOTOOL) -c -o $(top_builddir)/data/common/$@ $^


# Generic rule to create Windows .res files from .ico and .rc files
#
# TODO: try to simplify
#
%.res: $(top_srcdir)/data/common/%.rc %.ico
	$(WINDRES) $< -O coff -o $(top_builddir)/data/common/$@ -I $(top_builddir)/data/common


# Icon file dependencies for each emu
if SUPPORT_X64
x64.ico: $(X64_ICONS)
endif
x64sc.ico: $(X64SC_ICONS)
x64dtv.ico: $(X64DTV_ICONS)
xscpu64.ico: $(XSCPU64_ICONS)
x128.ico: $(X128_ICONS)
xvic.ico: $(XVIC_ICONS)
xplus4.ico: $(XPLUS4_ICONS)
xpet.ico: $(XPET_ICONS)
xcbm2.ico: $(XCBM2_ICONS)
xcbm5x0.ico: $(XCBM5x0_ICONS)
vsid.ico: $(VSID_ICONS)

endif
