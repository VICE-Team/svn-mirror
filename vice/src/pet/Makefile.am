SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libpet.a

libpet_a_SOURCES = \
	crtc.c \
	crtc.h \
	pet.c \
	pet.h \
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

BUILT_SOURCES = petvia.c

petdir = $(top_srcdir)/src/pet

petvia.c: petvia.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`petvia.c'..."; \
	   if [ -s $(PERL) ] ; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
	           $(petdir)/petvia.def $(petdir)/petvia.c; \
	   fi )
