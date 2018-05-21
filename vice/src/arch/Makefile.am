if DUMMY_COMPILE

SUBDIRS = android

endif

if NATIVE_GTK3_COMPILE

SUBDIRS = gtk3

else

if SDL_COMPILE

SUBDIRS = sdl

else

if UNIX_COMPILE

SUBDIRS = unix

endif

if WIN32_COMPILE

SUBDIRS = mingw32-pcap

endif

if BEOS_COMPILE

SUBDIRS = beos

endif

if AMIGA_COMPILE

SUBDIRS = amigaos

endif
endif
endif
