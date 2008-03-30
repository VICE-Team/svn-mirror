# Process this file with automake to produce Makefile.in

SUBDIRS = src data man doc

EXTRA_DIST = FEEDBACK TODO BUGS acconfig.h

.PHONY: x64 x128 vic20 pet
x64:
	(cd src; $(MAKE) x64)

x128:
	(cd src; $(MAKE) x128)

vic20:
	(cd src; $(MAKE) xvic)

pet:
	(cd src; $(MAKE) xpet)
