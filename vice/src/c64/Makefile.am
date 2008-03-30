if HAVE_RS232
acia_libadd = c64acia1.o c64rsuser.o
else
acia_libadd =
endif

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vicii \
	-I$(top_srcdir)/src/raster

noinst_LIBRARIES = libc64.a libc64c128.a libc64vsid.a

libc64_a_SOURCES = \
	c64.c \
	c64.h \
	c64cart.h \
	c64cartmem.c \
	c64cia.h \
	c64cia1.c \
	c64cia2.c \
	c64datasette.c \
	c64iec.c \
	c64iec.h \
	c64mem.c \
	c64mem.h \
	cartridge.c \
	patchrom.c \
	patchrom.h \
	reu.c \
	reu.h \
	c64tpi.c \
	c64tpi.h \
	psid.c \
	psid.h \
	psiddrv.a65 \
	psiddrv.h

libc64c128_a_SOURCES = \
	c64cia.h \
	c64cia2.c \
	c64datasette.c \
	patchrom.c \
	patchrom.h \
	c64tpi.c \
	c64tpi.h

libc64vsid_a_SOURCES = \
	c64cia.h \
	c64cia1.c \
	c64cia2.c \
	c64mem.c \
	c64mem.h \
	patchrom.c \
	patchrom.h

EXTRA_libc64_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64rsuser.c \
	c64rsuser.h

EXTRA_libc64c128_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64rsuser.c \
	c64rsuser.h

BUILT_SOURCES = psiddrv.h

EXTRA_DIST =

libc64_a_LIBADD = $(acia_libadd)
libc64c128_a_LIBADD = $(acia_libadd)

SUFFIXES = .a65

%.h : %.a65
	xa $< -o $(srcdir)/$*.o65
	od -v -An -w8 -tx1 $(srcdir)/$*.o65 | sed 's/\w\w/0x&,/g' > $(srcdir)/$@
