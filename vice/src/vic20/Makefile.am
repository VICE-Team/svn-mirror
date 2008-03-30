SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/true1541

noinst_LIBRARIES = libvic20.a

libvic20_a_SOURCES = \
	cartridge.c \
	vic.c \
	vic.h \
	vic20.c \
	vic20.h \
	vic20iec.c \
	vic20iec.h \
	vic20mem.c \
	vic20mem.h \
	vic20sound.c \
	vic20sound.h \
	vic20via.h \
	vic20via1.c \
	vic20via1.def \
	vic20via2.c \
	vic20via2.def

BUILT_SOURCES = vic20via1.c vic20via2.c

vic20dir = $(top_srcdir)/src/vic20

vic20via1.c: vic20via1.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`vic20via1.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
		   $(vic20dir)/vic20via1.def $(vic20dir)/vic20via1.c; \
           fi )

vic20via2.c: vic20via2.def $(top_srcdir)/src/via-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`vic20via2.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/via-tmpl.c \
		   $(vic20dir)/vic20via2.def $(vic20dir)/vic20via2.c; \
           fi )
