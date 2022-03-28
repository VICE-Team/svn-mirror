.PHONY: libarch

if DUMMY_COMPILE

SUBDIRS = shared

endif

if USE_GTK3UI

SUBDIRS = shared gtk3

libarch:
	(cd gtk3; $(MAKE) libarch.a)

else

if USE_SDLUI

SUBDIRS = shared sdl

libarch:
	(cd sdl; $(MAKE) libarch.a)

else

if USE_HEADLESSUI

SUBDIRS = shared headless

libarch:
	(cd headless; $(MAKE) libarch.a)

else

if WINDOWS_COMPILE

SUBDIRS = shared mingw32-pcap

endif
endif
endif
endif
