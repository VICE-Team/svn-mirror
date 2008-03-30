# Process this file with automake to produce Makefile.in

SUBDIRS = $(POSUB) src data man doc

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


if MSDOS_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/msdos/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) nozip

bindistzip:
	$(SHELL) $(srcdir)/src/arch/msdos/make-bindist.sh $(STRIP) $(VICE_VERSION_MAJOR) $(VICE_VERSION_MINOR) zip
endif

if WIN32_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/win32/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip

bindistzip:
	$(SHELL) $(srcdir)/src/arch/win32/make-bindist.sh $(STRIP) $(VICE_VERSION) zip
endif

if BEOS_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/beos/make-bindist.sh $(STRIP) $(VICE_VERSION) nozip

bindistzip:
	$(SHELL) $(srcdir)/src/arch/beos/make-bindist.sh $(STRIP) $(VICE_VERSION) zip
endif

if UNIX_QNX_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/unix/qnx/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS)
endif

if UNIX_SOLARIS_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/unix/solaris/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS)
endif

if AMIGA_COMPILE
bindist:
	$(SHELL) $(srcdir)/src/arch/amigaos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) nozip $(EXEEXT)

bindistzip:
	$(SHELL) $(srcdir)/src/arch/amigaos/make-bindist.sh $(STRIP) $(VICE_VERSION) $(host_cpu) $(host_os) zip $(EXEEXT)
endif
