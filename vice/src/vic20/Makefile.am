if HAVE_RS232
rs232_libadd = vic20rsuser.o
else
rs232_libadd =
endif

SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libvic20.a

libvic20_a_SOURCES = \
	cartridge.c \
	vic.c \
	vic.h \
	vic20.c \
	vic20.h \
	vic20datasette.c \
	vic20iec.c \
	vic20iec.h \
	vic20mem.c \
	vic20mem.h \
	vic20rsuser.c \
	vic20rsuser.h \
	vic20sound.c \
	vic20sound.h \
	vic20via.h \
	vic20via1.c \
	vic20via2.c

EXTRA_DIST = vic20.dsp

libvic20_a_LIBADD = $(rs232_libadd)

