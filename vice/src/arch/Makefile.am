if SDL_COMPILE

SUBDIRS = sdl

else

if UNIX_COMPILE

SUBDIRS = unix

endif

if WIN32_COMPILE

SUBDIRS = win32 win64

endif

if MSDOS_COMPILE

SUBDIRS = msdos

endif

if RISCOS_COMPILE

SUBDIRS = riscos

endif

if OS2_COMPILE

SUBDIRS = os2

endif

if BEOS_COMPILE

SUBDIRS = beos

endif

if AMIGA_COMPILE

SUBDIRS = amigaos

endif
endif

noinst_HEADERS = \
	platform.h \
	platform_aix_version.h \
	platform_compiler.h \
	platform_cpu_type.h \
	platform_freebsd_version.h \
	platform_linux_libc_version.h \
	platform_netbsd_version.h \
	platform_openbsd_version.h \
	platform_qnx6_version.h

EXTRA_DIST = \
	watcom-source-wrappers/geninfocontrib32.c \
	watcom-source-wrappers/gentranslate32.c \
	watcom-source-wrappers/genwinres32.c \
	watcom-source-wrappers/resid-sid.cc \
	watcom-source-wrappers/residdtv-envelope.cc \
	watcom-source-wrappers/residdtv-extfilt.cc \
	watcom-source-wrappers/residdtv-filter.cc \
	watcom-source-wrappers/residdtv-sid.cc \
	watcom-source-wrappers/residdtv-version.cc \
	watcom-source-wrappers/residdtv-voice.cc \
	watcom-source-wrappers/residdtv-wave.cc \
	watcom-source-wrappers/residfp-envelope.cc \
	watcom-source-wrappers/residfp-extfilt.cc \
	watcom-source-wrappers/residfp-filter.cc \
	watcom-source-wrappers/residfp-pot.cc \
	watcom-source-wrappers/residfp-sid.cc \
	watcom-source-wrappers/residfp-version.cc \
	watcom-source-wrappers/residfp-voice.cc \
	watcom-source-wrappers/residfp-wave.cc \
	watcom-source-wrappers/vic20cart-generic.c \
	watcom-source-wrappers/viciisc.c \
	watcom-source-wrappers/viciisc-cmdline-options.c \
	watcom-source-wrappers/viciisc-color.c \
	watcom-source-wrappers/viciisc-draw.c \
	watcom-source-wrappers/viciisc-fetch.c \
	watcom-source-wrappers/viciisc-irq.c \
	watcom-source-wrappers/viciisc-mem.c \
	watcom-source-wrappers/viciisc-phi1.c \
	watcom-source-wrappers/viciisc-resources.c \
	watcom-source-wrappers/viciisc-snapshot.c \
	watcom-source-wrappers/viciisc-timing.c \
	watcom-source-wrappers/x128-main.c \
	watcom-source-wrappers/x64-main.c \
	watcom-source-wrappers/x64dtv-main.c \
	watcom-source-wrappers/x64sc-main.c \
	watcom-source-wrappers/xcbm2-main.c \
	watcom-source-wrappers/xpet-main.c \
	watcom-source-wrappers/xplus4-main.c \
	watcom-source-wrappers/xvic-main.c
