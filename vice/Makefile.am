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
	$(SHELL) @ARCH_SRC_DIR@/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) nozip

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/macosx/make-bindist.sh $(top_srcdir) $(STRIP) $(VICE_VERSION) zip
endif

if UNIX_QNX_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/qnx/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/qnx/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip $(top_srcdir)
endif

if UNIX_SOLARIS_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/solaris/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/solaris/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS) zip $(top_srcdir)
endif

if WIN32_COMPILE
bindist:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip $(top_srcdir)

bindistzip:
	$(SHELL) @ARCH_SRC_DIR@/make-bindist.sh $(STRIP) $(VICE_VERSION) zip $(top_srcdir)
endif

cleantrans:
	rm `find $(top_srcdir) -name "*.po.c"`
