SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libtrue1541.a

libtrue1541_a_SOURCES = \
	1541cpu.c \
	1541cpu.h \
	true1541.c \
	true1541.h \
	viad.h \
	viad1.c \
	viad1.def \
	viad2.c \
	viad2.def

true1541dir = $(top_srcdir)/src/true1541

viad1.c: viad1.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`viad1.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
		   $(true1541dir)/viad1.def $(true1541dir)/viad1.c; \
           fi )

viad2.c: viad2.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`viad2.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
		   $(true1541dir)/viad2.def $(true1541dir)/viad2.c; \
           fi )
