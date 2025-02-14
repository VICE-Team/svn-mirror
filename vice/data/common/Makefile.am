commondir = $(VICE_DATADIR)/common

GRESOURCE_TARGET = $(builddir)/vice.gresource
GRESOURCE_XML = $(srcdir)/vice.gresource.xml

icon_files = vice-x128_16.png \
	     vice-x128_24.png \
	     vice-x128_32.png \
	     vice-x128_48.png \
	     vice-x128_64.png \
	     vice-x128_256.png \
	     vice-x128_1024.svg \
	     vice-x64_16.png \
	     vice-x64_24.png \
	     vice-x64_32.png \
	     vice-x64_48.png \
	     vice-x64_64.png \
	     vice-x64_256.png \
	     vice-x64_1024.svg \
	     vice-xcbm2_16.png \
	     vice-xcbm2_24.png \
	     vice-xcbm2_32.png \
	     vice-xcbm2_48.png \
	     vice-xcbm2_64.png \
	     vice-xcbm2_256.png \
	     vice-xcbm2_1024.svg \
	     vice-x64dtv_16.png \
	     vice-x64dtv_24.png \
	     vice-x64dtv_32.png \
	     vice-x64dtv_48.png \
	     vice-x64dtv_64.png \
	     vice-x64dtv_256.png \
	     vice-x64dtv_1024.svg \
	     vice-xpet_16.png \
	     vice-xpet_24.png \
	     vice-xpet_32.png \
	     vice-xpet_48.png \
	     vice-xpet_64.png \
	     vice-xpet_256.png \
	     vice-xpet_256.svg \
	     vice-xpet_1024.svg \
	     vice-xplus4_16.png \
	     vice-xplus4_24.png \
	     vice-xplus4_32.png \
	     vice-xplus4_48.png \
	     vice-xplus4_64.png \
	     vice-xplus4_256.png \
	     vice-xplus4_1024.svg \
	     vice-xscpu64_16.png \
	     vice-xscpu64_24.png \
	     vice-xscpu64_32.png \
	     vice-xscpu64_48.png \
	     vice-xscpu64_64.png \
	     vice-xscpu64_256.png \
	     vice-xscpu64_1024.svg \
	     vice-vsid_16.png \
	     vice-vsid_24.png \
	     vice-vsid_32.png \
	     vice-vsid_48.png \
	     vice-vsid_64.png \
	     vice-vsid_256.png \
	     vice-vsid_1024.svg \
	     vice-xvic_16.png \
	     vice-xvic_24.png \
	     vice-xvic_32.png \
	     vice-xvic_48.png \
	     vice-xvic_64.png \
	     vice-xvic_256.png \
	     vice-xvic_1024.svg

logo_files = CBM_Logo.svg \
	     vice-logo-black.svg

font_files = C64_Pro_Mono-STYLE.ttf \
             PetMe1282Y.ttf \
             PetMe128.ttf \
             PetMe2X.ttf \
             PetMe2Y.ttf \
             PetMe642Y.ttf \
             PetMe64.ttf \
             PetMe.ttf

desktop_files = x64.desktop \
		x64sc.desktop \
		x64dtv.desktop \
		xscpu64.desktop \
		x128.desktop \
		xvic.desktop \
		xplus4.desktop \
		xpet.desktop \
		xcbm2.desktop \
		xcbm5x0.desktop \
		vsid.desktop

# Don't install icon files into VICE data dir if we have --enable-desktop-files,
# we use install hooks with xdg-icon-resource for those:
if INSTALL_DESKTOP_FILES
common_DATA = $(logo_files) \
	      $(font_files)
else
common_DATA = $(icon_files) \
	      $(logo_files) \
	      $(font_files)
endif

EXTRA_DIST = $(icon_files) \
	     $(logo_files) \
	     $(font_files) \
	     $(desktop_files) \
             PetMe-FreeLicense.txt \
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
	     vsid.rc \
	     vice.gresource.xml


if USE_GTK3UI
# Generate gresource binary file, required by all Gtk3 emus
$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	glib-compile-resources --sourcedir $(top_srcdir) $< --target $@

common_DATA += $(GRESOURCE_TARGET)


if INSTALL_DESKTOP_FILES
.PHONY: install-icons install-desktop-files uinstall-icons 

# Install icon files in their proper place. The default context is 'apps' and
# the default mode is 'system' when running as root and 'user' when not running
# as root, both defaults are what we want.
install-icons:
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-x64_32.png vice-x64
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-x64_48.png vice-x64
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-x64_64.png vice-x64
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-x64_256.png vice-x64
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-x64dtv_32.png vice-x64dtv
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-x64dtv_48.png vice-x64dtv
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-x64dtv_64.png vice-x64dtv
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-x64dtv_256.png vice-x64dtv
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-xscpu64_32.png vice-xscpu64
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-xscpu64_48.png vice-xscpu64
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-xscpu64_64.png vice-xscpu64
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-xscpu64_256.png vice-xscpu64
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-x128_32.png vice-x128
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-x128_48.png vice-x128
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-x128_64.png vice-x128
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-x128_256.png vice-x128
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-xplus4_32.png vice-xplus4
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-xplus4_48.png vice-xplus4
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-xplus4_64.png vice-xplus4
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-xplus4_256.png vice-xplus4
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-xvic_32.png vice-xvic
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-xvic_48.png vice-xvic
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-xvic_64.png vice-xvic
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-xvic_256.png vice-xvic
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-xpet_32.png vice-xpet
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-xpet_48.png vice-xpet
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-xpet_64.png vice-xpet
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-xpet_256.png vice-xpet
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-xcbm2_32.png vice-xcbm2
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-xcbm2_48.png vice-xcbm2
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-xcbm2_64.png vice-xcbm2
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-xcbm2_256.png vice-xcbm2
	xdg-icon-resource install --size 32 $(top_srcdir)/data/common/vice-vsid_32.png vice-vsid
	xdg-icon-resource install --size 48 $(top_srcdir)/data/common/vice-vsid_48.png vice-vsid
	xdg-icon-resource install --size 64 $(top_srcdir)/data/common/vice-vsid_64.png vice-vsid
	xdg-icon-resource install --size 256 $(top_srcdir)/data/common/vice-vsid_256.png vice-vsid

uninstall-icons:
	xdg-icon-resource uninstall --size 32 vice-x64
	xdg-icon-resource uninstall --size 48 vice-x64
	xdg-icon-resource uninstall --size 64 vice-x64
	xdg-icon-resource uninstall --size 256 vice-x64
	xdg-icon-resource uninstall --size 32 vice-x64dtv
	xdg-icon-resource uninstall --size 48 vice-x64dtv
	xdg-icon-resource uninstall --size 64 vice-x64dtv
	xdg-icon-resource uninstall --size 256 vice-x64dtv
	xdg-icon-resource uninstall --size 32 vice-xscpu64
	xdg-icon-resource uninstall --size 48 vice-xscpu64
	xdg-icon-resource uninstall --size 64 vice-xscpu64
	xdg-icon-resource uninstall --size 256 vice-xscpu64
	xdg-icon-resource uninstall --size 32 vice-x128
	xdg-icon-resource uninstall --size 48 vice-x128
	xdg-icon-resource uninstall --size 64 vice-x128
	xdg-icon-resource uninstall --size 256 vice-x128
	xdg-icon-resource uninstall --size 32 vice-xplus4
	xdg-icon-resource uninstall --size 48 vice-xplus4
	xdg-icon-resource uninstall --size 64 vice-xplus4
	xdg-icon-resource uninstall --size 256 vice-xplus4
	xdg-icon-resource uninstall --size 32 vice-xvic
	xdg-icon-resource uninstall --size 48 vice-xvic
	xdg-icon-resource uninstall --size 64 vice-xvic
	xdg-icon-resource uninstall --size 256 vice-xvic
	xdg-icon-resource uninstall --size 32 vice-xpet
	xdg-icon-resource uninstall --size 48 vice-xpet
	xdg-icon-resource uninstall --size 64 vice-xpet
	xdg-icon-resource uninstall --size 256 vice-xpet
	xdg-icon-resource uninstall --size 32 vice-xcbm2
	xdg-icon-resource uninstall --size 48 vice-xcbm2
	xdg-icon-resource uninstall --size 64 vice-xcbm2
	xdg-icon-resource uninstall --size 256 vice-xcbm2
	xdg-icon-resource uninstall --size 32 vice-vsid
	xdg-icon-resource uninstall --size 48 vice-vsid
	xdg-icon-resource uninstall --size 64 vice-vsid
	xdg-icon-resource uninstall --size 256 vice-vsid

# We can use the program prefix/suffix set via configure's --program-prefix= /
# --program-suffix= switches via:
# --set-key=Exec --set-value=@PROGRAM_PREFIX@x64sc@PROGRAM_SUFFIX@, but icon
# lookup appears to depend on the .desktop file's name matching (the part
# after the optional vendor prefix) the executable name.
#
# So for example using --program-suffix=-gtk3 won't work since "x64sc-gtk3.desktop"
# means the vendor is "x64sc" and the executable "gtk3". For now we'll just make
# this work for the canonical emulator names, alternate names will be a TODO.
install-desktop-files:
if SUPPORT_X64
	desktop-file-install $(top_srcdir)/data/common/x64.desktop
endif
	desktop-file-install $(top_srcdir)/data/common/x64sc.desktop
	desktop-file-install $(top_srcdir)/data/common/x64dtv.desktop
	desktop-file-install $(top_srcdir)/data/common/xscpu64.desktop
	desktop-file-install $(top_srcdir)/data/common/x128.desktop
	desktop-file-install $(top_srcdir)/data/common/xplus4.desktop
	desktop-file-install $(top_srcdir)/data/common/xvic.desktop
	desktop-file-install $(top_srcdir)/data/common/xpet.desktop
	desktop-file-install $(top_srcdir)/data/common/xcbm2.desktop
	desktop-file-install $(top_srcdir)/data/common/xcbm5x0.desktop
	desktop-file-install $(top_srcdir)/data/common/vsid.desktop

# No `desktop-file-uninstall` exists, nor is there an uninstall option for
# `desktop-file-install` or `desktop-file-uninstall`, so a simple `rm` will
# have to do.
uninstall-desktop-files:
	rm -f /usr/share/applications/x64{,dtv,sc}.desktop
	rm -f /usr/share/applications/xscpu64.desktop
	rm -f /usr/share/applications/x128.desktop
	rm -f /usr/share/applications/xplus4.desktop
	rm -f /usr/share/applications/xpet.desktop
	rm -f /usr/share/applications/xcbm{2,5x0}.desktop
	rm -f /usr/share/applications/xvic.desktop
	rm -f /usr/share/applications/vsid.desktop

install-data-hook: install-icons install-desktop-files
	update-desktop-database

uninstall-hook: uninstall-icons uninstall-desktop-files
	update-desktop-database

endif

# The *.ico and *.res files are currently generated in
# src/arch/gtk3/data/win32/Makefile.am, but the plan is to move that to here
# to also be able to generate the files for linking with SDL binaries on Win32
CLEANFILES = \
	$(GRESOURCE_TARGET) \
	x64.ico \
	x64sc.ico \
	x64dtv.ico \
	xscpu64.ico \
	x128.ico \
	xvic.ico \
	xplus4.ico \
	xpet.ico \
	xcbm2.ico \
	xcbm5x0.ico \
	vsid.ico \
	x64.res \
	x64sc.res \
	x64dtv.res \
	xscpu64.res \
	x128.res \
	xvic.res \
	xplus4.res \
	xpet.res \
	xcbm2.res \
	xcbm5x0.res \
	vsid.res

endif
