#!/bin/sh
# make-bindist.sh for the Syllable SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <--enable-arch> <zip|nozip> <x64sc-include> <topsrcdir>
#                 $1      $2             $3       $4              $5          $6              $7

STRIP=$1
VERSION=$2
PREFIX=$3
ENABLEARCH=$4
ZIPKIND=$5
X64SC=$6
TOPSRCDIR=$7

if test x"$PREFIX" != "x/usr/VICE"; then
  echo Error: installation path is not /usr/VICE
  exit 1
fi

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 xscpu64 x64dtv $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e /usr/VICE/bin/$i ]
  then
    echo Error: \"make install\" needs to be done first
    exit 1
  fi
done

echo Generating Syllable SDL port binary distribution.
rm -f -r SDLVICE-syllable-$VERSION
mkdir -p SDLVICE-syllable-$VERSION/bin
mv /usr/VICE/bin/* SDLVICE-syllable-$VERSION/bin
$STRIP SDLVICE-syllable-$VERSION/bin/*
mkdir -p SDLVICE-syllable-$VERSION/lib
mv /usr/VICE/lib/vice SDLVICE-syllable-$VERSION/lib
mkdir -p SDLVICE-syllable-$VERSION/share/man/man1
mv /usr/VICE/share/man/man1/* SDLVICE-syllable-$VERSION/share/man/man1/
mkdir -p SDLVICE-syllable-$VERSION/info
mv /usr/VICE/share/info/vice.info* SDLVICE-syllable-$VERSION/share/info
cp $TOPSRCDIR/src/arch/sdl/syllable-files/* SDLVICE-syllable-$VERSION
rm -f -r /usr/VICE
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-syllable-$VERSION.zip SDLVICE-syllable-$VERSION
  else
    $ZIP SDLVICE-syllable-$VERSION.zip SDLVICE-syllable-$VERSION
  fi
  mv SDLVICE-syllable-$VERSION.zip SDLVICE-syllable-$VERSION.application
  rm -f -r SDLVICE-syllable
  echo Syllable SDL port binary package generated as SDLVICE-syllable-$VERSION.application
else
  echo Syllable SDL port binary distribution directory generated as SDLVICE-syllable-$VERSION
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
