INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/vdrive

noinst_LIBRARIES = libdrive.a

libdrive_a_SOURCES = \
        ciad.h \
        cia1571drive0.c \
        cia1571drive1.c \
        cia1581drive0.c \
        cia1581drive1.c \
        drive.c \
        drive.h \
        drivecpu0.c \
        drivecpu1.c \
        drivecpu.h \
        fdc.h \
        fdc.c \
        riot1drive0.c \
        riot1drive1.c \
        riot2drive0.c \
        riot2drive1.c \
        riotd.h \
        viad.h \
        via1drive0.c \
        via2drive0.c \
        via1drive1.c \
        via2drive1.c \
        wd1770.c \
        wd1770.h

EXTRA_DIST = \
	drivecpucore.c 

