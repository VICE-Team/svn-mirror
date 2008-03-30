SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libdrive.a

libdrive_a_SOURCES = \
        ciad.h \
        cia1571drive0.c \
        cia1571drive0.def \
        cia1571drive1.c \
        cia1571drive1.def \
        cia1581drive0.c \
        cia1581drive0.def \
        cia1581drive1.c \
        cia1581drive1.def \
        drive.c \
        drive.h \
        drivecpu0.c \
        drivecpu0.def \
        drivecpu1.c \
        drivecpu1.def \
        drivecpu.h \
        viad.h \
        via1drive0.c \
        via2drive0.c \
        via1drive1.c \
        via2drive1.c \
        wd1770.c \
        wd1770.h

EXTRA_DIST = \
	drivecpu-tmpl.c

BUILT_SOURCES = \
	cia1571drive0.c \
	cia1571drive1.c \
	cia1581drive0.c \
	cia1581drive1.c \
	drivecpu0.c \
	drivecpu1.c

drivedir = $(top_srcdir)/src/drive

drivecpu0.c: drivecpu0.def drivecpu-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`drivecpu0.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(drivedir)/drivecpu-tmpl.c \
	       $(drivedir)/drivecpu0.def $(drivedir)/drivecpu0.c; \
           fi )

drivecpu1.c: drivecpu1.def drivecpu-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`drivecpu1.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(drivedir)/drivecpu-tmpl.c \
	       $(drivedir)/drivecpu1.def $(drivedir)/drivecpu1.c; \
           fi )

cia1571drive0.c: cia1571drive0.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`cia1571drive0.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cia-tmpl.c \
		   $(drivedir)/cia1571drive0.def $(drivedir)/cia1571drive0.c; \
           fi )

cia1571drive1.c: cia1571drive1.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`cia1571drive1.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cia-tmpl.c \
		   $(drivedir)/cia1571drive1.def $(drivedir)/cia1571drive1.c; \
           fi )

cia1581drive0.c: cia1581drive0.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`cia1581drive0.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cia-tmpl.c \
		   $(drivedir)/cia1581drive0.def $(drivedir)/cia1581drive0.c; \
           fi )

cia1581drive1.c: cia1581drive1.def $(top_srcdir)/src/cia-tmpl.c $(top_srcdir)/src/convio
	@( echo "Rebuilding \`cia1581drive1.c'..."; \
	   if [ -s $(PERL) ]; then \
	       $(PERL) $(top_srcdir)/src/convio \
	           $(top_srcdir)/src/cia-tmpl.c \
		   $(drivedir)/cia1581drive1.def $(drivedir)/cia1581drive1.c; \
           fi )

