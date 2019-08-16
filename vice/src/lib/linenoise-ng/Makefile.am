# vim: set noet ts=8 sw=8 sts=8:
#
# Makefile.am to build liblinenoise-ng.a for VICE
#
#

# This needs to change into:
#	$VICE_CXXFLAGS -Wno-unused-variable -Wno-unused-function @CXXFLAGS@
#
# Once we alter configure.proto to not override C[XX|PP]FLAGS
#
AM_CXXFLAGS = -Wno-used-variable -Wno-unused-function

noinst_LIBRARIES = liblinenoiseng.a

liblinenoiseng_a_SOURCES = \
			   ConvertUTF.cpp \
			   linenoise.cpp \
			   wcwidth.cpp



EXTRA_DIST = \
	     ConvertUTF.h \
	     linenoise.h

