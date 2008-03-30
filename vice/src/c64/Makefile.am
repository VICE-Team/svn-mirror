SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/true1541

noinst_LIBRARIES = libc64.a libc64c128.a

libc64_a_SOURCES = \
	c64.c \
	c64.h \
	c64acia.h \
	c64acia1.c \
	c64acia1.def\
	c64acia2.c \
	c64acia2.def\
	c64cia.h \
	c64cia1.c \
	c64cia1.def \
	c64cia2.c \
	c64cia2.def \
	c64iec.c \
	c64iec.h \
	c64mem.c \
	c64mem.h \
	patchrom.c \
	patchrom.h \
	reu.c \
	reu.h \
	sid.c \
	sid.h \
	sprcrunch.h \
	sprcycles.h \
	sprites.h \
	tpi.c \
	tpi.h \
	vicii.c \
	vicii.h \
	wave6581.h \
	wave8580.h

libc64c128_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64acia1.def \
	c64acia2.c \
	c64acia2.def \
	c64cia.h \
	c64cia1.c \
	c64cia1.def \
	c64cia2.c \
	c64cia2.def \
	c64iec.c \
	c64iec.h \
	patchrom.c \
	patchrom.h \
	sid.c \
	sid.h \
	sprcrunch.h \
	sprcycles.h \
	sprites.h \
	tpi.c \
	tpi.h \
	vicii.c \
	vicii.h \
	wave6581.h \
	wave8580.h

BUILT_SOURCES = c64cia1.c c64cia2.c

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

c64acia2.c: c64acia2.def $(top_srcdir)/src/acia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c64acia2.c'..."; \
           if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/acia-tmpl.c \
	       	   $(c64dir)/c64acia2.def $(c64dir)/c64acia2.c; \
           fi )


