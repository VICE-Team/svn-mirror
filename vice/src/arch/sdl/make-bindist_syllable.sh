#!/bin/sh
# make-bindist.sh for the Syllable SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <zip|nozip> <topsrcdir>
#                 $1      $2             $3       $4          $5

STRIP=$1
VERSION=$2
PREFIX=$3
ZIPKIND=$4
TOPSRCDIR=$5

if test x"$PREFIX" != "x/usr/VICE"; then
  echo Error: installation path is not /usr/VICE
  exit 1
fi

if [ ! -e /usr/VICE/bin/x64 -o ! -e /usr/VICE/bin/x64dtv -o ! -e /usr/VICE/bin/x128 -o ! -e /usr/VICE/bin/xvic -o ! -e /usr/VICE/bin/xpet -o ! -e /usr/VICE/bin/xplus4 -o ! -e /usr/VICE/bin/xcbm2 -o ! -e /usr/VICE/bin/c1541 -o ! -e /usr/VICE/bin/petcat -o ! -e /usr/VICE/bin/cartconv ]; then
  echo Error: \"make install\" needs to be done first
  exit 1
fi

echo Generating Syllable SDL port binary distribution.
rm -f -r SDLVICE-syllable-$VERSION
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/da/LC_MESSAGES
mv /usr/VICE/lib/locale/da/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/da/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/de/LC_MESSAGES
mv /usr/VICE/lib/locale/de/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/de/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/fr/LC_MESSAGES
mv /usr/VICE/lib/locale/fr/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/fr/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/it/LC_MESSAGES
mv /usr/VICE/lib/locale/it/LC_MESSAGES/vice.* SDLVICE-syllable/lib/locale/it/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/sv/LC_MESSAGES
mv /usr/VICE/lib/locale/sv/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/sv/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/pl/LC_MESSAGES
mv /usr/VICE/lib/locale/pl/LC_MESSAGES/vice.* SDLVICE-syllable/$VERSION/lib/locale/pl/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/nl/LC_MESSAGES
mv /usr/VICE/lib/locale/nl/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/nl/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/hu/LC_MESSAGES
mv /usr/VICE/lib/locale/hu/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/hu/LC_MESSAGES
mkdir -p SDLVICE-syllable-$VERSION/lib/locale/tr/LC_MESSAGE
mv /usr/VICE/lib/locale/tr/LC_MESSAGES/vice.* SDLVICE-syllable-$VERSION/lib/locale/tr/LC_MESSAGES
mkdir SDLVICE-syllable-$VERSION/bin
mv /usr/VICE/bin/* SDLVICE-syllable-$VERSION/bin
$STRIP SDLVICE-syllable-$VERSION/bin/*
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
