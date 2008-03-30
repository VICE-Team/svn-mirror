SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libpet.a

libpet_a_SOURCES = \
	crtc.c \
	crtc.def \
	pet.c \
	pet.h \
	petacia.h \
	petacia1.c \
	petacia1.def \
	petmem.c \
	petmem.h \
	pets.c \
	pets.h \
	petsound.c \
	petsound.h \
	petvia.c \
	petvia.def \
	petvia.h \
	pia.c \
	pia.h

BUILT_SOURCES = petvia.c petacia1.c crtc.c

petdir = $(top_srcdir)/src/pet

petvia.c: petvia.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petvia.c'..."; \
	   if [ -s $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
	           $(petdir)/petvia.def $(petdir)/petvia.c; \
	   fi )

petacia1.c: petacia1.def $(top_srcdir)/src/acia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petacia1.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/acia-tmpl.c \
	           $(petdir)/petacia1.def $(petdir)/petacia1.c; \
	   fi )

crtc.c: crtc.def $(top_srcdir)/src/crtc-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`crtc.c'..."; \
	   if [ -x $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/crtc-tmpl.c \
	           $(petdir)/crtc.def $(petdir)/crtc.c; \
	   fi )

