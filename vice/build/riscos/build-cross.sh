#!/bin/sh
#
# By default, a 32bit ARM6 target will be built; you can change this by 
# calling this script with -cpu <cpuname>, where valid CPU names are
#
# - arm3: 26bit code
# - arm6: 32bit ARM6 code
# - strongarm: 32bit StrongARM code
#
# The default target C library is libscl, but you can change that with
# -clib <libname>, where valid library names are libscl or unixlib.
#
# Trying to build configurations not supported by your GCCSDK version
# will cause the script to fail, of course.
# Before you run this script, you need to do the following steps:
#
# 1) point the environment variable GCCSDKDIR to the directory containing
#    your GCCSDK crosscompiler (i.e. $GCCSDKDIR/bin/gcc is the compiler)
# 2) Make sure $GCCSDKDIR/local/include contains the ZLib header files
# 3) Make sure $GCCSDKDIR/local/wimplib contains the Wimplib header files
# 4) Make sure you have the ZLib and Wimplib libraries in the correct
#    places. The root directory for libs is $GCCSDKDIR/local/lib/<clibname>
#    where <clibname> is "libscl" for the Shared C Library or "unixlib"
#    for (you guessed it) UnixLib. Relative to this path, put you 26bit
#    libs in lib26, 32bit/ARM6 libs in lib32/arch3 and 32bit/StrongARM libs
#    in lib32/arch4.
#

if [ ! "$GCCSDKDIR" ]; then
  echo "Set environment variable GCCSDKDIR first"
  exit
fi

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

TARGETCLIB=libscl
CPUNAME="arm6"
CPUFLAGS=""
OPTFLAGS="-O3 -fexpensive-optimizations"
CXXEXTRAFLAGS="-fno-rtti -fno-exceptions"

while [ "$1" ]; do
  consumed=1
  case "$1" in
    -cpu) if [ "$2" ]; then CPUNAME="$2"; consumed=2; fi;;
    -clib) if [ "$2" ]; then TARGETCLIB="$2"; consumed=2; fi;;
    -opt) if [ "$2" ]; then OPTFLAGS="$2"; consumed=2; fi;;
    -cxx) if [ "$2" ]; then CXXEXTRAFLAGS="$2"; consumed=2; fi;;
    *) echo "Unknown switch $1, ignored";;
  esac
  shift $consumed
done

if [ "$1" ]; then
  CPUNAME="$1"
fi

case $CPUNAME in
  strongarm) CPUFLAGS="-mcpu=strongarm -mapcs-32";;
  arm6) CPUFLAGS="-mcpu=arm6 -mapcs-32";;
  arm3) CPUFLAGS="";;
  *) echo "Unknown CPU name $CPUNAME"; exit;;
esac
export CPUFLAGS

if [ "`${GCCSDKDIR}/bin/gcc --version | grep 2.95.4`" ]; then
  CPPLINKFLAGS=""
else
  CPPLINKFLAGS="-lstdc++"
fi
export CPPLINKFLAGS

case $TARGETCLIB in
  unixlib) BASECLIBFLAGS="";;
  libscl) BASECLIBFLAGS=-mlibscl;;
  *) echo "Unknown target library (must be unixlib or libscl)"; exit;;
esac
export BASECLIBFLAGS
#echo "CPU=${CPUNAME}, CFLAGS=${CPUFLAGS}, CLIB=${TARGETCLIB}"; exit;

configureoptions="--host=arm-riscos --enable-dependency-tracking --without-x --with-readline --without-png --disable-zlibtest"
export WIMPLIBDIR=${GCCSDKDIR}/local/wimplib
export INCLUDES="-I${GCCSDKDIR}/local/include -I${WIMPLIBDIR}"
#export INCLUDES="-I${WIMPLIBDIR}"
export COMPCOREFLAGS="${BASECLIBFLAGS} ${CPUFLAGS}"
export CC=${GCCSDKDIR}/bin/gcc
export CXX=${GCCSDKDIR}/bin/gcc
export AR=${GCCSDKDIR}/bin/ar
export STRIP=${GCCSDKDIR}/bin/strip
export RANLIB=true
export CPPFLAGS="${INCLUDES} -DRISCOS"
export CFLAGS="${OPTFLAGS} -Wall -mpoke-function-name ${COMPCOREFLAGS} ${INCLUDES} -DRISCOS -DFIXPOINT_ARITHMETIC"
export CXXFLAGS="${CFLAGS} ${CXXEXTRAFLAGS}"
if [ "`echo $CPUFLAGS | grep mapcs-32`" ]; then
  if [ "`echo $CPUFLAGS | grep strongarm`" ]; then
    SUPPLIBDIR=lib32/arch4
  else
    SUPPLIBDIR=lib32/arch3
  fi
else
  SUPPLIBDIR=lib26
fi
export SUPPORTLIBPATH=${GCCSDKDIR}/local/lib/${TARGETCLIB}/${SUPPLIBDIR}
#export SUPPORTLIBPATH=${GCCSDKDIR}/local/wimplib
export LIBS="${COMPCOREFLAGS} -L${SUPPORTLIBPATH} -lwimp -lz ${CPPLINKFLAGS}"
./configure ${configureoptions}
make src/Makefile.in src/Makefile src/resid/Makefile.in src/resid/Makefile

# clean up binaries configure didn't expect (!RunImage* rather than a.out)
for i in \!RunImage \!RunImage,ff8 src/resid/\!RunImage src/resid/\!RunImage,ff8; do
  if [ -f "$i" ]; then
    rm "$i"
  fi
done

# make sure config.h isn't created from a future make run
cd src; make config.h.in config.h
rm config.h
ln -s arch/riscos/config.h config.h

make
make bindistzip
