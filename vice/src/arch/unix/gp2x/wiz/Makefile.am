# Makefile.am for the WIZ version of `libarchgwizsys.a'.

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/c64 \
	-I$(top_srcdir)/src/c128 \
	-I$(top_srcdir)/src/vic20 \
	-I$(top_srcdir)/src/pet \
	-I$(top_srcdir)/src/cbm2 \
	-I$(top_srcdir)/src/sid \
	-I$(top_srcdir)/src/imagecontents \
	-I$(top_srcdir)/src/lib/p64 \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libarchwizsys.a

libarchwizsys_a_SOURCES = \
	gp2xwiz.h \
	pollux_set.c \
	pollux_set.h \
	polluxregs.h \
	sys_cacheflush.h \
	sys_cacheflush.S \
	uppermem.c \
	uppermem.h \
	warm.c \
	warm.h \
	wiz_funcs.c \
	wiz_lib.c \
	wiz_lib.h
