SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/sid

noinst_LIBRARIES = libc610.a

libc610_a_SOURCES = \
	crtc.c \
	crtc.def \
	c610.c \
	c610.h \
	c610acia.h \
	c610acia1.c \
	c610acia1.def \
	c610cia.h \
	c610cia1.c \
	c610cia1.def \
	c610cpu.c \
	c610cpu.def \
	c610mem.c \
	c610mem.h \
	c610tpi.h \
	c610tpi1.c \
	c610tpi1.def \
	c610tpi2.c \
	c610tpi2.def

BUILT_SOURCES = c610acia1.c c610tpi1.c c610tpi2.c c610cia1.c crtc.c c610cpu.c

c610dir = $(top_srcdir)/src/c610

c610acia1.c: c610acia1.def $(top_srcdir)/src/acia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c610acia1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/acia-tmpl.c \
	           $(c610dir)/c610acia1.def $(c610dir)/c610acia1.c; \
	   fi )

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

c610cia1.c: c610cia1.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c610cia1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cia-tmpl.c \
	           $(c610dir)/c610cia1.def $(c610dir)/c610cia1.c; \
	   fi )

crtc.c: crtc.def $(top_srcdir)/src/crtc-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`crtc.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/crtc-tmpl.c \
	           $(c610dir)/crtc.def $(c610dir)/crtc.c; \
	   fi )

c610cpu.c: c610cpu.def $(top_srcdir)/src/cpu-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`c610cpu.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cpu-tmpl.c \
	           $(c610dir)/c610cpu.def $(c610dir)/c610cpu.c; \
	   fi )

