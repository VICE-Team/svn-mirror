# Makefile.am to build libzmbv.a for VICE

AM_CPPFLAGS = @VICE_CPPFLAGS@
AM_CFLAGS = @VICE_CFLAGS@
AM_CXXFLAGS = @VICE_CXXFLAGS@
AM_LDFLAGS = @VICE_LDFLAGS@

if !HAVE_ZLIB
AM_CPPFLAGS += -DZMBV_USE_MINIZ
AM_CFLAGS += -DZMBV_USE_MINIZ
endif

noinst_LIBRARIES = libzmbv.a

libzmbv_a_SOURCES = \
			   zmbv.c \
			   zmbv_avi.c

EXTRA_DIST = \
	     zmbv_avi.h \
	     miniz.c \
	     zmbv.h

