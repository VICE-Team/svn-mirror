# Process this file with automake to produce Makefile.in

SUBDIRS = $(POSUB) src build data man doc

EXTRA_DIST = AUTHORS FEEDBACK COPYING INSTALL NEWS vice.spec

.PHONY: x64 x128 xvic xpet xplus4 xcbm2
x64:
	(cd src; $(MAKE) x64-all)

x128:
	(cd src; $(MAKE) x128-all)

xvic:
	(cd src; $(MAKE) xvic-all)

xpet:
	(cd src; $(MAKE) xpet-all)

xplus4:
	(cd src; $(MAKE) xplus4-all)

xcbm2:
	(cd src; $(MAKE) xcbm2-all)


install: installvice

if AMIGA_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) nozip $(top_srcdir) $(EXEEXT)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) zip $(top_srcdir) $(EXEEXT)
endif

if BEOS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) zip $(top_srcdir)
endif

if MSDOS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip $(top_srcdir)
endif

if UNIX_GP2X_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/gp2x/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/gp2x/make-bindist.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
endif

if UNIX_MACOSX_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) nozip @UNIX_MACOSX_UITYPE@

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) zip @UNIX_MACOSX_UITYPE@
endif

if UNIX_SKYOS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/skyos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/skyos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) zip $(top_srcdir)
endif

if UNIX_QNX6_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/qnx6/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/qnx6/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip $(top_srcdir)
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
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sol $(infodir) $(mandir) $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sol $(infodir) $(mandir) $(top_srcdir)
endif

if UNIX_OPENSERVER5_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco5 $(infodir) $(mandir) $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco5 $(infodir) $(mandir) $(top_srcdir)
endif

if UNIX_OPENSERVER6_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco6 $(infodir) $(mandir) $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco6 $(infodir) $(mandir) $(top_srcdir)
endif

if UNIX_UNIXWARE_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip sco7 $(infodir) $(mandir) $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/sco_sol/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip sco7 $(infodir) $(mandir) $(top_srcdir)
endif

if WIN32_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
endif

if RISCOS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip $(top_srcdir)
endif

@MAKE_BINDIST_TRUE@installvice:
@MAKE_BINDIST_TRUE@	echo this platform doesn\'t use \"make install\", use \"make bindist\" instead

@MAKE_BINDIST_FALSE@installvice: install-recursive

cleantrans:
	rm `find $(top_srcdir) -name "*.po.c"`
