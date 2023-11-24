SUBDIRS = p64 linenoise-ng libzmbv md5

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
