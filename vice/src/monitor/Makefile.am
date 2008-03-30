INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/drive \
	-I$(top_srcdir)/src/vdrive

YFLAGS = -dt

noinst_LIBRARIES = libmonitor.a

noinst_HEADERS = \
	mon_parse.h

libmonitor_a_SOURCES = \
	asm.c \
	asm.h \
	mon.c \
	mon.h \
	mon_lex.l \
	mon_parse.y

BUILT_SOURCES = mon_parse.h mon_parse.c

