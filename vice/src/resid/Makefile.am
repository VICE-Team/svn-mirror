## Process this file with automake to create Makefile.in

lib_LIBRARIES = libresid.a

libresid_a_SOURCES = sid.cc voice.cc wave.cc envelope.cc filter.cc pot.cc version.cc $(noinst_DATA:.dat=.cc)

pkginclude_HEADERS = siddefs.h sid.h voice.h wave.h envelope.h filter.h pot.h

noinst_DATA = wave6581_PST.dat wave6581_PS_.dat wave6581_P_T.dat wave6581__ST.dat wave8580_PST.dat wave8580_PS_.dat wave8580_P_T.dat wave8580__ST.dat

noinst_SCRIPTS = samp2src.pl

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

SUFFIXES = .dat

.dat.cc:
	$(PERL) ./samp2src.pl $* $< $@

maintainer-clean-libLIBRARIES:
	rm -f $(noinst_DATA:.dat=.cc)
