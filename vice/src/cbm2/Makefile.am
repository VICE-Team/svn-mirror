SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libcbm2.a

libcbm2_a_SOURCES = \
	crtc.c \
	c610.c \
	c610.h \
	c610acia.h \
	c610acia1.c \
	c610cia.h \
	c610cia1.c \
	c610cpu.c \
	c610datasette.c \
	c610iec.c \
	c610mem.c \
	c610mem.h \
	c610tpi.h \
	c610tpi1.c \
	c610tpi1.def \
	c610tpi2.c \
	c610tpi2.def

BUILT_SOURCES = c610tpi1.c c610tpi2.c 

c610dir = $(top_srcdir)/src/cbm2

c610tpi1.c: c610tpi1.def $(top_srcdir)/src/tpi-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c610tpi1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/tpi-tmpl.c \
	           $(c610dir)/c610tpi1.def $(c610dir)/c610tpi1.c; \
	   fi )

c610tpi2.c: c610tpi2.def $(top_srcdir)/src/tpi-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c610tpi2.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/tpi-tmpl.c \
	           $(c610dir)/c610tpi2.def $(c610dir)/c610tpi2.c; \
	   fi )

