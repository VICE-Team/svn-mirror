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
	watcom-source-wrappers/vic20cart-generic.c \
	watcom-source-wrappers/gentranslate32.c \
	watcom-source-wrappers/genwinres32.c \
	watcom-source-wrappers/x128-main.c \
	watcom-source-wrappers/x64dtv-main.c \
	watcom-source-wrappers/x64sc-main.c \
	watcom-source-wrappers/xcbm2-main.c
