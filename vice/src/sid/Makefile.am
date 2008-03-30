if HAVE_RESID
resid_libadd = resid.o
else
resid_libadd =
endif

SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	@RESID_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsid.a

libsid_a_SOURCES = \
	sid.c \
	sid.h \
	wave6581.h \
	wave8580.h

EXTRA_libsid_a_SOURCES = \
	resid.cc \
	resid.h

libsid_a_LIBADD = $(resid_libadd)

siddir = $(top_srcdir)/src/sid

