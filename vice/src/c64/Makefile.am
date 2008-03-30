if HAVE_RS232
acia_libadd = c64acia1.o c64rsuser.o
else
acia_libadd =
endif


SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vicii

noinst_LIBRARIES = libc64.a libc64c128.a

libc64_a_SOURCES = \
	c64.c \
	c64.h \
	c64cart.h \
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
	c64tpi.def \
	c64tpi.h

libc64c128_a_SOURCES = \
	c64cia.h \
	c64cia2.c \
	c64datasette.c \
	patchrom.c \
	patchrom.h \
	c64tpi.c \
	c64tpi.def \
	c64tpi.h

BUILT_SOURCES = \
	c64tpi.c

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

libc64_a_LIBADD = $(acia_libadd)
libc64c128_a_LIBADD = $(acia_libadd)

c64dir = $(top_srcdir)/src/c64

c64tpi.c: c64tpi.def $(top_srcdir)/src/tpi-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64tpi.c'..."; \
           if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/tpi-tmpl.c \
	       	   $(c64dir)/c64tpi.def $(c64dir)/c64tpi.c; \
           fi )


