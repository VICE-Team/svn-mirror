#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

curdir=`pwd`

CC=owcc CXX=owcc RANLIB=true STRIP=wstrip AR="/bin/sh $curdir/src/arch/win32/watcom/watcom-ar.sh" ./configure --host=i686-pc-mingw32
make
make bindist
