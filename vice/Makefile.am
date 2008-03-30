# Process this file with automake to produce Makefile.in

SUBDIRS = src data man doc

EXTRA_DIST = FEEDBACK TODO BUGS acconfig.h vice.spec

.PHONY: x64 x128 xvic xpet xcbm2 vsid
x64:
	(cd src; $(MAKE) x64-all)

x128:
	(cd src; $(MAKE) x128-all)

xvic:
	(cd src; $(MAKE) xvic-all)

xpet:
	(cd src; $(MAKE) xpet-all)

xcbm2:
	(cd src; $(MAKE) xcbm2-all)
