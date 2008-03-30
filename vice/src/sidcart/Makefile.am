SUFFIXES = .def

INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libsidcart.a

libsidcart_a_SOURCES = \
	fastsid.c \
	fastsid.h \
	sidcart-cmdline-options.c \
	sidcart-cmdline-options.h \
	sidcart-resources.c \
	sidcart-resources.h \
	sidcart.c \
	sidcart.h

EXTRA_DIST =

sidcartdir = $(top_srcdir)/src/sidcart
