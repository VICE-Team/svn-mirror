SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libc128.a

libc128_a_SOURCES = \
	c128.c \
	c128.h \
	c128cia1.def \
	c128cia1.c \
	c128cpu.c \
	c128cpu.def \
	c128iec.c \
	c128mem.c \
	c128mem.h \
	vdc.c \
	vdc.h

BUILT_SOURCES = c128cpu.c c128cia1.c

c128dir = $(top_srcdir)/src/c128

c128cia1.c: c128cia1.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c128cia1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/cia-tmpl.c \
	       	   $(c128dir)/c128cia1.def $(c128dir)/c128cia1.c; \
           fi )

c128cpu.c: c128cpu.def $(top_srcdir)/src/cpu-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c128cpu.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	       	   $(top_srcdir)/src/cpu-tmpl.c \
	       	   $(c128dir)/c128cpu.def $(c128dir)/c128cpu.c; \
           fi )

