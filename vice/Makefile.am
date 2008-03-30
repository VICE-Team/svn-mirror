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
	$(srcdir)/src/arch/msdos/make-bindist.sh $(STRIP)
endif

if WIN32_COMPILE
bindist:
	$(srcdir)/src/arch/win32/make-bindist.sh $(STRIP) $(VICE_VERSION)
endif

if BEOS_COMPILE
bindist:
	$(srcdir)/src/arch/beos/make-bindist.sh $(STRIP) $(VICE_VERSION)
endif

if QNX_COMPILE
bindist:
	$(srcdir)/src/arch/unix/qnx/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS)
endif

if SOLARIS_COMPILE
bindist:
	$(srcdir)/src/arch/unix/solaris/make-bindist.sh $(STRIP) $(VICE_VERSION) $(prefix) $(CROSS)
endif

if AMIGA_COMPILE
bindist:
	$(srcdir)/src/arch/amigaos/make-bindist.sh $(STRIP)
endif
