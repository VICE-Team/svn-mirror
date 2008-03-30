# Process this file with automake to produce Makefile.in

SUBDIRS = $(POSUB) src data man doc

EXTRA_DIST = AUTHORS FEEDBACK COPYING INSTALL NEWS acconfig.h vice.spec

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
