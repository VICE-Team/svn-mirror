.PHONY: libarch

if DUMMY_COMPILE

SUBDIRS = shared android

endif

if NATIVE_GTK3_COMPILE

SUBDIRS = shared gtk3

libarch:
	(cd gtk3; $(MAKE) libarch.a)

else

if SDL_COMPILE

SUBDIRS = shared sdl

libarch:
	(cd sdl; $(MAKE) libarch.a)

else

if HEADLESS_COMPILE

SUBDIRS = shared headless

libarch:
	(cd headless; $(MAKE) libarch.a)

else

if WIN32_COMPILE

SUBDIRS = shared mingw32-pcap

endif
endif
endif
endif
