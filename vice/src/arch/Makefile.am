if DUMMY_COMPILE

SUBDIRS = shared android

endif

if NATIVE_GTK3_COMPILE

SUBDIRS = shared gtk3

else

if SDL_COMPILE

SUBDIRS = shared sdl

else

if HEADLESS_COMPILE

SUBDIRS = shared headless

else

if WIN32_COMPILE

SUBDIRS = shared mingw32-pcap

endif
endif
endif
endif
