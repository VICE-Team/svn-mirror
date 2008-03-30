INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libdiskimage.a

EXTRA_libdiskimage_a_SOURCES = \
	rawimage.c \
	rawimage.h \
        realimage.c \
        realimage.h

libdiskimage_a_SOURCES = \
	createdisk.c \
	createdisk.h \
	diskimage.c \
	fsimage-gcr.c \
	fsimage-gcr.h \
	fsimage-probe.c \
	fsimage-probe.h \
	fsimage.c \
	fsimage.h \
	x64.h

if HAVE_RAWDRIVE
if HAVE_REALDEVICE
libdiskimage_a_DEPENDENCIES = rawimage.o realimage.o
libdiskimage_a_LIBADD = rawimage.o realimage.o
else
libdiskimage_a_DEPENDENCIES = rawimage.o
libdiskimage_a_LIBADD = rawimage.o
endif
else
if HAVE_REALDEVICE
libdiskimage_a_DEPENDENCIES = realimage.o
libdiskimage_a_LIBADD = realimage.o
else
libdiskimage_a_DEPENDENCIES =
libdiskimage_a_LIBADD =
endif
endif

