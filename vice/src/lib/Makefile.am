SUBDIRS = p64 linenoise-ng

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

noinst_HEADERS = \
	opencbm.h

.PHONY: libp64

libp64:
	(cd p64; $(MAKE))
