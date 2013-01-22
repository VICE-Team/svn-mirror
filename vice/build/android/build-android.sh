#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    exit 1
  fi
fi

curdir=`pwd`

cd src
echo generating src/translate_table.h
. ./gentranslatetable.sh <translate.txt >translate_table.h
echo generating src/translate.h
. ./gentranslate_h.sh <translate.txt >translate.h
echo generating src/infocontrib.h
. ./geninfocontrib_h.sh <../doc/vice.texi | sed -f infocontrib.sed >infocontrib.h
cd arch/android/AnVICE
echo building libvice.so
ndk-build
echo generating apk
ant debug
cd ../../../..
mv src/arch/android/AnVICE/bin/PreConfig-debug.apk ./AnVICE-arm.apk
