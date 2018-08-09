# vim: set noet ts=8:
#
# Makefile.am for the shared archdep code


AM_CPPFLAGS = \
	@ARCH_INCLUDES@ \
	$(extra_includes) \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src


noinst_LIBRARIES = libarchdep.a

libarchdep_a_SOURCES = \
	archdep_program_name.c \
	archdep_startup_log_error.c

EXTRA_DIST = \
	archdep_program_name.h \
	archdep_startup_log_error.h
