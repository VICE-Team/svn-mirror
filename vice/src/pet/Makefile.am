SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libpet.a

libpet_a_SOURCES = \
	crtc.c \
	crtc.def \
	crtc.h \
	pet.c \
	pet.h \
	petacia.h \
	petacia1.c \
	petacia1.def \
	petiec.c \
	petmem.c \
	petmem.h \
	petpia.h \
	petpia1.c \
	petpia1.def \
	petpia2.c \
	petpia2.def \
	pets.c \
	pets.h \
	petsound.c \
	petsound.h \
	petvia.c \
	petvia.def \
	petvia.h

BUILT_SOURCES = \
	crtc.c \
	petacia1.c \
	petpia1.c \
	petpia2.c \
	petvia.c

petdir = $(top_srcdir)/src/pet

petvia.c: petvia.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petvia.c'..."; \
	   if [ -s $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
	           $(petdir)/petvia.def $(petdir)/petvia.c; \
	   fi )

petpia1.c: petpia1.def $(top_srcdir)/src/pia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petpia1.c'..."; \
	   if [ -s $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/pia-tmpl.c \
	           $(petdir)/petpia1.def $(petdir)/petpia1.c; \
	   fi )

petpia2.c: petpia2.def $(top_srcdir)/src/pia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petpia2.c'..."; \
	   if [ -s $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/pia-tmpl.c \
	           $(petdir)/petpia2.def $(petdir)/petpia2.c; \
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

