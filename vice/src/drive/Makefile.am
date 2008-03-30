SUBDIRS = \
	iec \
	ieee \
	tcbm

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/monitor \
	-I$(top_srcdir)/src/vdrive

noinst_LIBRARIES = libdrive.a

libdrive_a_SOURCES = \
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
	iec.h \
	ieee.h \
	rotation.c \
	rotation.h \
	tcbm.h \
	via1d.c \
	via2d.c \
	viad.h

.PHONY: libdriveiec libdriveieee libdrivetcbm

libdriveiec:
	(cd iec; $(MAKE))
libdriveieee:
	(cd ieee; $(MAKE))
libdrivetcbm:
	(cd tcbm; $(MAKE))

