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
if !UNIX_MACOSX_COMPILE
# Make .desktop files

if INSTALL_DESKTOP_FILES


VERSION_NUM="$(VICE_VERSION_MAJOR).$(VICE_VERSION_MINOR)"

X64_DESKTOP = $(PROGRAM_PREFIX)x64$(PROGRAM_SUFFIX).desktop
X64SC_DESKTOP = $(PROGRAM_PREFIX)x64sc$(PROGRAM_SUFFIX).desktop
X64DTV_DESKTOP = $(PROGRAM_PREFIX)x64dtv$(PROGRAM_SUFFIX).desktop
XSCPU64_DESKTOP = $(PROGRAM_PREFIX)xscpu64$(PROGRAM_SUFFIX).desktop
X128_DESKTOP = $(PROGRAM_PREFIX)x128$(PROGRAM_SUFFIX).desktop
XVIC_DESKTOP = $(PROGRAM_PREFIX)xvic$(PROGRAM_SUFFIX).desktop
XPET_DESKTOP = $(PROGRAM_PREFIX)xpet$(PROGRAM_SUFFIX).desktop
XPLUS4_DESKTOP = $(PROGRAM_PREFIX)xplus4$(PROGRAM_SUFFIX).desktop
XCBM5x0_DESKTOP = $(PROGRAM_PREFIX)xcbm5x0$(PROGRAM_SUFFIX).desktop
XCBM2_DESKTOP = $(PROGRAM_PREFIX)xcbm2$(PROGRAM_SUFFIX).desktop
VSID_DESKTOP = $(PROGRAM_PREFIX)vsid$(PROGRAM_SUFFIX).desktop

# x64
$(X64_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64$(PROGRAM_SUFFIX)" \
		"Crappy C64 emulator" \
		"C64_1024.svg"

# x64sc
$(X64SC_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64sc$(PROGRAM_SUFFIX)" \
		"Cycle-exact C64 emulator" \
		"C64_1024.svg"

# x64dtv
$(X64DTV_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x64dtv$(PROGRAM_SUFFIX)" \
		"DTV64 emulator" \
		"DTV_1024.svg"

# xscpu64
$(XSCPU64_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xscpu64$(PROGRAM_SUFFIX)" \
		"SuperCPU C64 emulator" \
		"SCPU_1024.svg"

# x128
$(X128_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)x128$(PROGRAM_SUFFIX)" \
		"C128 emulator" \
		"C128_1024.svg"


# xvic
$(XVIC_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xvic$(PROGRAM_SUFFIX)" \
		"VIC-20 emulator" \
		"VIC20_1024.svg"

# xpet
$(XPET_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xpet$(PROGRAM_SUFFIX)" \
		"PET emulator" \
		"PET_1024.svg"


# xcbm5x0
$(XCBM5x0_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xcbm5x0$(PROGRAM_SUFFIX)" \
		"CBM-II model 5x0 emulator" \
		"CBM2_1024.svg"


# xcbm2
$(XCBM2_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xcbm2$(PROGRAM_SUFFIX)" \
		"CBM-II model 6x0/7x0 emulator" \
		"CBM2_1024.svg"


# xplus4
$(XPLUS4_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)xplus4$(PROGRAM_SUFFIX)" \
		"Plus-4 / C16 emulator" \
		"Plus_1024.svg"


# vsid
$(VSID_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)vsid$(PROGRAM_SUFFIX)" \
		"SID Player (way better than JCH's garbage)" \
		"SID_1024.svg"


# c1541
# Shell tool, icon doesn't make much sense
c1541.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VERSION_NUM)" \
		"$(PROGRAM_PREFIX)c1541$(PROGRAM_SUFFIX)" \
		"Don't let Groepaz take this away from you!" \
		"C64_1024.svg"


# if INSTALL_DESKTOP_FILES
endif
# if !UNIX_MACOSX_COMPILE
endif
# if UNIX_COMPILE
endif


# Install data cannot be conditional according to automake, so we install into
# $PREFIX/common, including x64, which probably breaks shit later.
gtk3data_DATA = \
	$(GRESOURCE_TARGET) $(WINDRES_COFF) \
	$(X64_DESKTOP) \
	$(X64SC_DESKTOP) \
	$(X64DTV_DESKTOP) \
	$(XSCPU64_DESKTOP) \
	$(X128_DESKTOP) \
	$(XVIC_DESKTOP) \
	$(XPET_DESKTOP) \
	$(XCBM5x0_DESKTOP) \
	$(XCBM2_DESKTOP) \
	$(XPLUS4_DESKTOP) \
	$(VSID_DESKTOP)

# No need for UI menu items for command-line tools, I think
#	c1541.desktop



.PHONY: clean
clean:
	rm -f $(GRESOURCE_TARGET)
	rm -f $(GRESOURCE_XML)
if WIN32_COMPILE
	rm -f $(WINDRES_COFF)
	rm -f $(WINDRES_RC_TARGET)
endif
if UNIX_COMPILE
if !UNIX_MACOSX_COMPILE
	rm -f $(X64_DESKTOP)
	rm -f $(X64SC_DESKTOP)
	rm -f $(X64DTV_DESKTOP)
	rm -f $(XSCPU64_DESKTOP)
	rm -f $(X128_DESKTOP)
	rm -f $(XVIC_DESKTOP)
	rm -f $(XPET_DESKTOP)
	rm -f $(XPLUS4_DESKTOP)
	rm -f $(XCBM5x0_DESKTOP)
	rm -f $(XCBM2_DESKTOP)
	rm -f $(VSID_DESKTOP)
endif
endif


.PHONY: distclean
distclean: clean

# vim: set noet ts=8 sts=8 sw=8:
