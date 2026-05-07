
AUTOMAKE_OPTIONS = foreign subdir-objects

EXTRA_DIST = utpp

if ENABLE_TEST

AM_CPPFLAGS = \
-I$(top_srcdir)/src \
-I$(top_builddir)/src

TESTS = \
TestEnvelopeGenerator \
TestWaveformGenerator \
TestSpline \
TestSID \
TestDac \
TestLimiter

check_PROGRAMS = $(TESTS)

TestEnvelopeGenerator_SOURCES = \
Main.cpp \
TestEnvelopeGenerator.cpp

TestWaveformGenerator_SOURCES = \
Main.cpp \
TestWaveformGenerator.cpp

TestSpline_SOURCES = \
Main.cpp \
TestSpline.cpp

TestSID_SOURCES = \
Main.cpp \
TestSID.cpp
TestSID_LDADD = $(top_builddir)/src/libresidfp.la

TestDac_SOURCES = \
Main.cpp \
TestDac.cpp

TestLimiter_SOURCES = \
Main.cpp \
TestLimiter.cpp

endif
