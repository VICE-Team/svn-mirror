INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive

noinst_LIBRARIES = libserial.a

EXTRA_libserial_a_SOURCES = \
        realdevice.c \
        realdevice.h

libserial_a_SOURCES = \
	fsdrive.c \
	fsdrive.h \
	iec-bus.c \
	iec-bus.h \
	serial-device.c \
	serial-device.h \
	serial-iec-lib.c \
	serial-iec-lib.h \
	serial-iec.c \
	serial-iec.h \
	serial-realdevice.c \
	serial-realdevice.h \
	serial-trap.c \
	serial-trap.h \
	serial.c

if HAVE_REALDEVICE
libserial_a_DEPENDENCIES = realdevice.o
libserial_a_LIBADD = realdevice.o
else
libserial_a_DEPENDENCIES =
libserial_a_LIBADD =
endif
