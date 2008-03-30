SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive

noinst_LIBRARIES = libpet.a

libpet_a_SOURCES = \
	crtc.c \
	pet.c \
	pet.h \
	petacia.h \
	petacia1.c \
	petiec.c \
	petmem.c \
	petmem.h \
	petpia.h \
	petpia1.c \
	petpia1.def \
	petpia2.c \
	petpia2.def \
	pets.h \
	petsound.c \
	petsound.h \
	petvia.c \
	petvia.h

BUILT_SOURCES = \
	petpia1.c \
	petpia2.c

petdir = $(top_srcdir)/src/pet

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

