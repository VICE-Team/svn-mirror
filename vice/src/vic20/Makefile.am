if HAVE_RS232
rs232_libadd = vic20rsuser.o
else
rs232_libadd =
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/raster

noinst_LIBRARIES = libvic20.a

libvic20_a_SOURCES = \
	cartridge.c \
	vic.c \
	vic.h \
	vic-cmdline-options.c \
	vic-cmdline-options.h \
	vic-draw.c \
	vic-draw.h \
	vic-mem.c \
	vic-mem.h \
	vic-resources.c \
	vic-resources.h \
	vic-snapshot.c \
	vic-snapshot.h \
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

libvic20_a_LIBADD = $(rs232_libadd)

