## Makefile.am for `libuiclient.a'.

AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/headless \
	$(extra_includes)

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_OBJCFLAGS = @VICE_OBJCFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@

noinst_LIBRARIES = libuiclient.a

libuiclient_a_SOURCES = \
	netexpect.c \
	uiclient.c \
	uiclientnetwork.c \
	uiclientutil.c

BUILT_SOURCES = \
	uiclientmachine.h

uiclientmachine.h: ../../../machine.h uiclient_machine_generate.sh
	$(AM_V_GEN) $(srcdir)/uiclient_machine_generate.sh < $< > $@

EXTRA_DIST = \
	netexpect.h \
	uiclient.h \
	uiclientnetwork.h \
	uiclientutil.h

bin_PROGRAMS = uiclienttest

uiclienttest_libs = \
	libuiclient.a

uiclienttest_SOURCES = \
	uiclienttest.c

uiclienttest_LDADD = \
	$(uiclienttest_libs)

uiclienttest_DEPENDENCIES = $(uiclienttest_libs)
