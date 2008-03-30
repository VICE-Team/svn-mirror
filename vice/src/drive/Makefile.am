SUBDIRS = \
	tcbm

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/vdrive \
	-I$(top_srcdir)/src/monitor

noinst_LIBRARIES = libdrive.a

libdrive_a_SOURCES = \
	cia1571d.c \
	cia1581d.c \
	ciad.h \
	drive-cmdline-options.c \
	drive-cmdline-options.h \
	drive-resources.c \
	drive-resources.h \
	drive-snapshot.c \
	drive-snapshot.h \
	drive.c \
	drive.h \
	drivecpu.c \
	drivecpu.h \
	drivemem.c \
	driveimage.c \
	driveimage.h \
	drivemem.h \
	driverom.c \
	driverom.h \
	drivesync.c \
	drivesync.h \
	drivetypes.h \
	fdc.h \
	fdc.c \
	riot1d.c \
	riot2d.c \
	riotd.h \
	tcbm.h \
	via1d.c \
	via2d.c \
	viad.h \
	wd1770.c \
	wd1770.h

.PHONY: libdrivetcbm

libdrivetcbm:
	(cd tcbm; $(MAKE))

