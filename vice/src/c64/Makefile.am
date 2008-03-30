SUBDIRS = cart

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/vicii \
	-I$(top_srcdir)/src/monitor

noinst_LIBRARIES = libc64.a libc64c128.a

libc64_a_SOURCES = \
	c64-cmdline-options.c \
	c64-cmdline-options.h \
	c64-resources.c \
	c64-resources.h \
	c64-snapshot.c \
	c64-snapshot.h \
	c64.c \
	c64.h \
	c64acia.h \
	c64acia1.c \
	c64cart.h \
	c64cia.h \
	c64cia1.c \
	c64cia2.c \
	c64datasette.c \
	c64iec.c \
	c64iec.h \
	c64io.c \
	c64io.h \
	c64keyboard.c \
	c64keyboard.h \
	c64mem.c \
	c64mem.h \
	c64memlimit.c \
	c64memlimit.h \
	c64pla.c \
	c64pla.h \
	c64rom.c \
	c64rom.h \
	c64rsuser.c \
	c64rsuser.h \
	c64tpi.c \
	c64tpi.h \
	patchrom.c \
	patchrom.h \
        psid.c \
        psid.h \
        psiddrv.a65 \
        psiddrv.h \
        reloc65.c \
	reu.c \
	reu.h

libc64c128_a_SOURCES = \
	c64acia.h \
	c64acia1.c \
	c64cart.h \
	c64cia.h \
	c64cia2.c \
	c64datasette.c \
	c64keyboard.c \
	c64keyboard.h \
	c64pla.c \
	c64pla.h \
	c64rsuser.c \
	c64rsuser.h \
	c64tpi.c \
	c64tpi.h \
	patchrom.c \
	patchrom.h \
	reu.c \
	reu.h

BUILT_SOURCES = psiddrv.h

EXTRA_DIST =

SUFFIXES = .a65

%.h : %.a65
	xa -R -G $< -o $(srcdir)/$*.o65
	od -v -An -w8 -tx1 $(srcdir)/$*.o65 | sed 's/\w\w/0x&,/g' > $(srcdir)/$@

.PHONY: libc64cart

libc64cart:
	(cd cart; $(MAKE))
