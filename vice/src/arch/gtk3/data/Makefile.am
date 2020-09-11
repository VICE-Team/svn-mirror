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


XDG_VENDOR=vice-org

# Define emu binary names
X64_BINARY := $(shell echo x64 | sed '$(transform)')
X64SC_BINARY := $(shell echo x64sc | sed '$(transform)')
X64DTV_BINARY := $(shell echo x64dtv | sed '$(transform)')
XSCPU64_BINARY := $(shell echo xscpu64 | sed '$(transform)')
X128_BINARY := $(shell echo x128 | sed '$(transform)')
XPET_BINARY := $(shell echo xpet | sed '$(transform)')
XVIC_BINARY := $(shell echo xvic | sed '$(transform)')
XPLUS4_BINARY := $(shell echo xplus4 | sed '$(transform)')
XCBM5x0_BINARY := $(shell echo xcbm5x0 | sed '$(transform)')
XCBM2_BINARY := $(shell echo xcbm2 | sed '$(transform)')
VSID_BINARY := $(shell echo vsid | sed '$(transform)')

# Define emu .desktop file names
X64_DESKTOP = "$(XDG_VENDOR)-$(X64_BINARY).desktop"
X64SC_DESKTOP = "$(XDG_VENDOR)-$(X64SC_BINARY).desktop"
X64DTV_DESKTOP = "$(XDG_VENDOR)-$(X64DTV_BINARY).desktop"
XSCPU64_DESKTOP = "$(XDG_VENDOR)-$(XSCPU64_BINARY).desktop"
X128_DESKTOP = "$(XDG_VENDOR)-$(X128_BINARY).desktop"
XVIC_DESKTOP = "$(XDG_VENDOR)-$(XVIC_BINARY).desktop"
XPLUS4_DESKTOP = "$(XDG_VENDOR)-$(XPLUS4_BINARY).desktop"
XPET_DESKTOP = "$(XDG_VENDOR)-$(XPET_BINARY).desktop"
XCBM5x0_DESKTOP = "$(XDG_VENDOR)-$(XCBM5x0_BINARY).desktop"
XCBM2_DESKTOP = "$(XDG_VENDOR)-$(XCBM2_BINARY).desktop"
VSID_DESKTOP = "$(XDG_VENDOR)-$(VSID_BINARY).desktop"

VICE_DIR_FILE = $(top_builddir)/src/arch/gtk3/data/$(XDG_VENDOR).pokefinder.vice.directory


# Right now I need this to regenerate every time. Proper installation of XDG
# desktop files and directories is pretty shitty to figure out. Once this crap
# works we can remove the PHONY



$(VICE_DIR_FILE): $(XDG_VENDOR).pokefinder.vice.directory.proto
	echo "prefix = $(prefix)"
	sed "s@__PREFIX__@$(prefix)@" $< >$@


# x64
if SUPPORT_X64
$(X64_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(X64_BINARY)" \
		"Crappy C64 emulator" \
		"C64_1024.svg"
endif

# x64sc
$(X64SC_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(X64SC_BINARY)" \
		"Cycle-exact C64 emulator" \
		"C64_1024.svg"

# x64dtv
$(X64DTV_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(X64DTV_BINARY)" \
		"DTV64 emulator" \
		"DTV_1024.svg"

# xscpu64
$(XSCPU64_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" "$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XSCPU64_BINARY)" \
		"SuperCPU C64 emulator" \
		"SCPU_1024.svg"

# x128
$(X128_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(X128_BINARY)" \
		"C128 emulator" \
		"C128_1024.svg"


# xvic
$(XVIC_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XVIC_BINARY)" \
		"VIC-20 emulator" \
		"VIC20_1024.svg"

# xpet (has "*256.svg" vs "*1024.svg" with the rest of the files, which is weird)
$(XPET_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XPET_BINARY)" \
		"PET emulator" \
		"PET_256.svg"


# xcbm5x0
$(XCBM5x0_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XCBM5x0_BINARY)" \
		"CBM-II model 5x0 emulator" \
		"CBM2_1024.svg"


# xcbm2
$(XCBM2_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XCBM2_BINARY)" \
		"CBM-II model 6x0/7x0 emulator" \
		"CBM2_1024.svg"


# xplus4
$(XPLUS4_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(XPLUS4_BINARY)" \
		"Plus-4 / C16 emulator" \
		"Plus4_1024.svg"


# vsid
$(VSID_DESKTOP): emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
		"$(VSID_BINARY)" \
		"SID Player" \
		"SID_1024.svg"


# c1541
# Shell tool, icon doesn't make much sense
c1541.desktop: emu.desktop
	$(top_srcdir)/src/arch/gtk3/data/mkdesktopfile.sh \
		"$(top_srcdir)" \
		"$(top_builddir)" \
		"$(top_srcdir)/src/arch/gtk3/data/emu.desktop" \
		"${prefix}" \
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

if SUPPORT_X64
gtk3data_DATA += \
	$(X64_DESKTOP)
endif

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
if SUPPORT_X64
	rm -f $(X64_DESKTOP)
endif
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


install: $(VICE_DIR_FILE) emu.desktop
if INSTALL_DESKTOP_FILES
	echo "Doing icon install!"
if SUPPORT_X64
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(X64_DESKTOP)"
endif
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(X64SC_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(X64DTV_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XSCPU64_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(X128_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XPLUS4_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XVIC_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XCBM5x0_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XCBM2_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(XPET_DESKTOP)"
	$(XDG_DESKTOP_MENU) install "$(VICE_DIR_FILE)" "$(VSID_DESKTOP)"
endif


# vim: set noet ts=8 sts=8 sw=8:
