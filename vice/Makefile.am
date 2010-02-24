# Process this file with automake to produce Makefile.in

SUBDIRS = $(POSUB) src build data man doc

# COMMON_EXTRA_DIST is what used to be the EXTRA_DIST, when adding dist
# files, please add them to the COMMON_EXTRA_DIST list.

COMMON_EXTRA_DIST = .descr \
	AUTHORS \
	build.minix \
	ChangeLog.1 \
	COPYING \
	FEEDBACK \
	Doxyfile \
	INSTALL \
	NEWS \
	vice.spec \
	vice-gtk.spec \
	vice-mint.spec


# PO_EXTRA_DIST is a list of files in the po directory that need to be
# included in the source archive, this list was added to fix the fact that
# 'make dist' only produced a proper/workable source archive when done on a
# *nix platform with nls.

PO_EXTRA_DIST = po/ChangeLog \
	po/Makefile.in.in \
	po/POTFILES.in \
	po/de.po \
	po/fr.po \
	po/hu.po \
	po/intl2po.c \
	po/it.po \
	po/nl.po \
	po/pl.po \
	po/po2tbl.sed \
	po/sv.po

if PO_SUBDIR_USED

EXTRA_DIST = $(COMMON_EXTRA_DIST)

else

EXTRA_DIST = $(COMMON_EXTRA_DIST) $(PO_EXTRA_DIST)

endif


.PHONY: x64 x128 x64dtv xvic xpet xplus4 xcbm2
x64:
	(cd src; $(MAKE) x64-all)

x128:
	(cd src; $(MAKE) x128-all)

if SUPPORT_X64DTV
x64dtv:
	(cd src; $(MAKE) x64dtv-all)
endif

xvic:
	(cd src; $(MAKE) xvic-all)

xpet:
	(cd src; $(MAKE) xpet-all)

xplus4:
	(cd src; $(MAKE) xplus4-all)

xcbm2:
	(cd src; $(MAKE) xcbm2-all)


install: installvice

# If we have no ChangeLog (which should be generated from SVN), generate a
# dummy one.

ChangeLog:
	touch ChangeLog

if AMIGA_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_amiga.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) nozip $(top_srcdir) $(EXEEXT)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_amiga.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) zip $(top_srcdir) $(EXEEXT)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) nozip $(top_srcdir) $(EXEEXT)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) zip $(top_srcdir) $(EXEEXT)
endif
endif

if BEOS_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_beos.sh $(STRIP) $(VICE_VERSION) $(host_cpu) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_beos.sh $(STRIP) $(VICE_VERSION) $(host_cpu) zip $(top_srcdir)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) zip $(top_srcdir)
endif
endif

if MSDOS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip $(top_srcdir)
endif

if UNIX_GP2X_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_gp2x.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_gp2x.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/gp2x/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/gp2x/make-bindist.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
endif
endif

if UNIX_DINGUX_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_dingoo.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_dingoo.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
endif

if UNIX_MACOSX_COMPILE
bindist:
	/bin/bash @ARCH_SRC_DIR@/../unix/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) nozip @UNIX_MACOSX_UITYPE@

bindistzip:
	/bin/bash @ARCH_SRC_DIR@/../unix/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) zip @UNIX_MACOSX_UITYPE@
endif

if UNIX_SKYOS_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_skyos.sh $(STRIP) $(VICE_VERSION) $(prefix) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_skyos.sh $(STRIP) $(VICE_VERSION) $(prefix) zip $(top_srcdir)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/skyos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/skyos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) zip $(top_srcdir)
endif
endif

if UNIX_QNX6_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_qnx6.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip $(top_srcdir) $(MAKE) $(host_cpu)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_qnx6.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip $(top_srcdir) $(MAKE) $(host_cpu)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/qnx6/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip $(top_srcdir) $(MAKE) $(host_cpu)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/qnx6/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip $(top_srcdir) $(MAKE) $(host_cpu)
endif
endif

if UNIX_QNX4_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/qnx4/make-bindist.sh $(VICE_VERSION) $(prefix) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/qnx4/make-bindist.sh $(VICE_VERSION) $(prefix) zip $(top_srcdir)
endif

if UNIX_HPUX_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/hpux/make-bindist.sh $(STRIP) $(PERL) $(VICE_VERSION) $(prefix) $(host_cpu) $(host_os) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/hpux/make-bindist.sh $(STRIP) $(PERL) $(VICE_VERSION) $(prefix) $(host_cpu) $(host_os) zip $(top_srcdir)
endif

if UNIX_SOLARIS_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sol $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sol $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
if GNOMEUI
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sol gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sol gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sol xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sol xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)
endif
endif
endif

if UNIX_OPENSTEP_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/openstep/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) nozip $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/openstep/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) zip $(top_srcdir) $(MAKE)
endif

if UNIX_OPENSERVER5_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco5 $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco5 $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
if GNOMEUI
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco5 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco5 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco5 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco5 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)
endif
endif
endif

if UNIX_OPENSERVER6_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco6 $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco6 $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
if GNOMEUI
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco6 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco6 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco6 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco6 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)
endif
endif
endif

if UNIX_UNIXWARE_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco7 $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_scosol.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco7 $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
if GNOMEUI
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco7 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco7 gtk $(infodir) $(mandir) $(top_srcdir) $(MAKE)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco7 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco7 xaw $(infodir) $(mandir) $(top_srcdir) $(MAKE)
endif
endif
endif

if WIN32_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_win32.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir) $(host_cpu)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_win32.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir) $(host_cpu)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir) $(host_cpu)
endif
endif

if RISCOS_COMPILE
if SDL_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_riscos.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_riscos.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip $(top_srcdir)
else
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip $(top_srcdir)
endif
endif

if SYLLABLE_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_syllable.sh $(STRIP) $(VICE_VERSION) $(prefix) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist_syllable.sh $(STRIP) $(VICE_VERSION) $(prefix) zip $(top_srcdir)
endif

@MAKE_BINDIST_TRUE@installvice:
@MAKE_BINDIST_TRUE@	@echo this platform doesn\'t use \"make install\", use \"make bindist\" instead

@MAKE_BINDIST_FALSE@installvice: install-recursive

cleantrans:
	rm `find $(top_srcdir) -name "*.po.c"`

# Manually define the dist targets so the correct tar usage will be used.
dist-gzip: distdir
	$(AMTAR) chf - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist dist-all: distdir
	$(AMTAR) chf - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)
