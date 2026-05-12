SUBDIRS = p64 linenoise-ng libzmbv md5

if HAVE_USBSID
SUBDIRS += libusbsiddrv
endif

if HAVE_RESIDFP
SUBDIRS += libresidfp
endif

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_HEADERS = \
	opencbm.h

.PHONY: libp64

libp64:
	(cd p64; $(MAKE))

.PHONY: libzmbv

libzmbv:
	(cd libzmbv; $(MAKE))

.PHONY: libusbsiddrv
if HAVE_USBSID
libusbsiddrv:
	(cd libusbsiddrv; $(MAKE))
endif

.PHONY: libresidfp
if HAVE_RESIDFP
libresidfp:
	(cd libresidfp; $(MAKE))
endif

install installdirs: SUBDIRS := $(filter-out libresidfp, $(SUBDIRS))
