gtk3datadir = $(VICE_DATADIR)/common


GRESOURCE_TARGET = $(builddir)/vice.gresource
# TODO: check the double backslash
GRESOURCE_PROTO = $(srcdir)//vice.gresource.xml.proto
GRESOURCE_XML = $(builddir)/vice.gresource.xml

# FIXME: final target of gresource is called _TARGET, but here the final target
#        is _COFF
WINDRES_RC_PROTO = $(top_srcdir)/src/arch/gtk3/data/icon.rc.proto
if WIN32_COMPILE
WINDRES_RC_TARGET = $(top_builddir)/src/arch/gtk3/data/icon.rc
WINDRES_COFF = $(top_builddir)/src/arch/gtk3/data/icon.res
endif

if WIN32_COMPILE
all: $(GRESOURCE_TARGET) $(WINDRES_COFF)
else
all: $(GRESOURCE_TARGET)
endif

$(GRESOURCE_XML): $(GRESOURCE_PROTO)
	cat $(GRESOURCE_PROTO) | sed -e "s@VICE_SRC_DIR@$(srcdir)@g" > $(GRESOURCE_XML)

$(GRESOURCE_TARGET): $(GRESOURCE_XML)
	glib-compile-resources $< --target $@

if WIN32_COMPILE
$(WINDRES_RC_TARGET): $(WINDRES_RC_PROTO)
	cat $< | sed -e "s@__PREFIX__@$(top_srcdir)@g" > $@

$(WINDRES_COFF): $(WINDRES_RC_TARGET)
	$(WINDRES) $< -O coff -o $@
endif

EXTRA_DIST = $(GRESOURCE_PROTO) $(WINDRES_RC_PROTO)


if UNIX_COMPILE
# Make .desktop files


VERSION_NUM="$(VICE_VERSION_MAJOR).$(VICE_VERSION_MINOR)"

# x64
x64.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64$(PROGRAM_SUFFIX)" \
		"C64 emulator (deprecated)" \
		"icon.png"

# x64sc
x64sc.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64sc$(PROGRAM_SUFFIX)" \
		"Cycle-exact C64 emulator" \
		"icon.png"

# x64dtv
x64dtv.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64dtv$(PROGRAM_SUFFIX)" \
		"DTV64 emulator" \
		"icon.png"

# xscpu64
xscpu64.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xscpu64$(PROGRAM_SUFFIX)" \
		"SuperCPU C64 emulator" \
		"icon.png"

# x128
x128.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x128$(PROGRAM_SUFFIX)" \
		"C128 emulator" \
		"icon.png"


# xvic
xvic.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xvic$(PROGRAM_SUFFIX)" \
		"VIC-20 emulator" \
		"icon.png"

# xpet
xpet.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xpet$(PROGRAM_SUFFIX)" \
		"PET emulator" \
		"icon.png"


# xcbm5x0
xcbm5x0.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xcbm5x0$(PROGRAM_SUFFIX)" \
		"CBM-II model 5x0 emulator" \
		"icon.png"


# xcbm2
xcbm2.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xcbm2$(PROGRAM_SUFFIX)" \
		"CBM-II model 6x0/7x0 emulator" \
		"icon.png"


# vsid
vsid.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)vsid$(PROGRAM_SUFFIX)" \
		"SID Player (better than JCH's garbage)" \
		"icon.png"


# c1541
c1541.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)c1541$(PROGRAM_SUFFIX)" \
		"CBM media editor or so?" \
		"icon.png"

endif


# Install data cannot be conditional according to automake, so we install into
# $PREFIX/common, including x64, which probably breaks shit later.
gtk3data_DATA = \
	$(GRESOURCE_TARGET) $(WINDRES_COFF) \
	x64.desktop \
	x64sc.desktop \
	x64dtv.desktop \
	xscpu64.desktop \
	x128.desktop \
	xvic.desktop \
	xpet.desktop \
	xcbm5x0.desktop \
	xcbm2.desktop \
	c1541.desktop



.PHONY: clean
clean:
	rm -f $(GRESOURCE_TARGET)
	rm -f $(GRESOURCE_XML)
	rm -f $(WINDRES_COFF)
	rm -f $(WINDRES_RC_TARGET)

.PHONY: distclean
distclean: clean

# vim: set noet ts=8 sts=8 sw=8:
