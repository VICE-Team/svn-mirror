#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.proto ]; then
  cd ../..
  if [ ! -f configure.proto ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

curdir=`pwd`

CC=owcc CXX=owcc RANLIB=true STRIP=wstrip AR="/bin/sh $curdir/src/arch/win32/watcom/watcom-ar.sh" ./configure -v --host=i686-pc-mingw32 --enable-native-tools=owcc --disable-dependency-tracking
make
make bindist
