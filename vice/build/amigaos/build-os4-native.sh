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

./configure
gcc src/arch/amigaos/fixdeps.c -o ./fixdeps
cd src
../fixdeps
cd ..
make
make bindist
