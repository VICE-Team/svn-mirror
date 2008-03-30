if HAVE_RESID
if HAVE_RS232
resid_acia_libadd = resid.o c64acia1.o c64rsuser.o
else
resid_acia_libadd = resid.o
endif
else
if HAVE_RS232
resid_acia_libadd = c64acia1.o c64rsuser.o
else
resid_acia_libadd =
endif
endif


SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	@RESID_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libc64.a libc64c128.a

libc64_a_SOURCES = \
	c64.c \
	c64.h \
	c64cia.h \
	c64cia1.c \
	c64cia1.def \
	c64cia2.c \
	c64cia2.def \
	c64iec.c \
	c64iec.h \
	c64mem.c \
	c64mem.h \
	cartridge.c \
	patchrom.c \
	patchrom.h \
	reu.c \
	reu.h \
	sid.c \
	sid.h \
	sprcrunch.h \
	sprcycles.h \
	sprites.h \
	c64tpi.c \
	c64tpi.def \
	c64tpi.h \
	vicii.c \
	vicii.h \
	wave6581.h \
	wave8580.h

libc64c128_a_SOURCES = \
	c64cia.h \
	c64cia2.c \
	c64cia2.def \
	patchrom.c \
	patchrom.h \
	sid.c \
	sid.h \
	sprcrunch.h \
	sprcycles.h \
	sprites.h \
	c64tpi.c \
	c64tpi.def \
	c64tpi.h \
	vicii.c \
	vicii.h \
	wave6581.h \
	wave8580.h

BUILT_SOURCES = \
	c64acia1.c \
	c64cia1.c \
	c64cia2.c

EXTRA_libc64_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64acia1.def \
	c64rsuser.c \
	c64rsuser.h \
	resid.cc \
	resid.h

EXTRA_libc64c128_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64acia1.def \
	c64rsuser.c \
	resid.cc \
	resid.h

libc64_a_LIBADD = $(resid_acia_libadd)
libc64c128_a_LIBADD = $(resid_acia_libadd)

c64dir = $(top_srcdir)/src/c64

c64cia1.c: c64cia1.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64cia1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/cia-tmpl.c \
	       	   $(c64dir)/c64cia1.def $(c64dir)/c64cia1.c; \
           fi )

c64cia2.c: c64cia2.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64cia2.c'..."; \
           if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/cia-tmpl.c \
	       	   $(c64dir)/c64cia2.def $(c64dir)/c64cia2.c; \
           fi )

c64acia1.c: c64acia1.def $(top_srcdir)/src/acia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64acia1.c'..."; \
           if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/acia-tmpl.c \
	       	   $(c64dir)/c64acia1.def $(c64dir)/c64acia1.c; \
           fi )

c64tpi.c: c64tpi.def $(top_srcdir)/src/tpi-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64tpi.c'..."; \
           if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/tpi-tmpl.c \
	       	   $(c64dir)/c64tpi.def $(c64dir)/c64tpi.c; \
           fi )


