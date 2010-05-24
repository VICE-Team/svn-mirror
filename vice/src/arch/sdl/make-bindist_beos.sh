#!/bin/sh
# make-bindist.sh for the BEOS SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <cpu> <zip|nozip> <top-srcdir>
#                 $1      $2             $3    $4          $5

STRIP=$1
VICEVERSION=$2
CPU=$3
ZIPKIND=$4
TOPSRCDIR=$5

if [ ! -e src/x64 -o ! -e src/x64dtv -o ! -e src/x64sc -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating BEOS SDL port binary distribution.
if test x"$CPU" = "xpowerpc" -o x"$CPU" = "xppc"; then
  BEOSCPU=powerpc
else
  BEOSCPU=x86
fi

rm -f -r SDLVICE-$BEOSCPU-beos-$VICEVERSION
mkdir SDLVICE-$BEOSCPU-beos-$VICEVERSION
$STRIP src/x64
$STRIP src/x64dtv
$STRIP src/x64sc
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
$STRIP src/c1541
$STRIP src/petcat
$STRIP src/cartconv
cp src/x64 src/x64dtv src/x128 src/xvic SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp src/xpet src/xplus4 src/xcbm2 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp src/c1541 src/petcat src/cartconv SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp src/x64sc SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/C128 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/C64 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/DRIVES SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PET SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PLUS4 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PRINTER SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/VIC20 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/fonts SDLVICE-$BEOSCPU-beos-$VICEVERSION
mkdir SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp -a $TOPSRCDIR/doc/html SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/doc/iec-bus.txt SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/doc/mon.txt SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/doc/cartconv.txt SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/doc/ReadmeSDL.txt SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README SDLVICE-$BEOSCPU-beos-$VICEVERSION
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "Makefile*"`
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "*.vkm" -and ! -name "sdl*.vkm"`
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "*.vsc"`
if [ -e SDLVICE-$BEOSCPU-beos-$VICEVERSION/html/texi2html ]
then
  rm SDLVICE-$BEOSCPU-beos-$VICEVERSION/html/texi2html
fi
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip SDLVICE-$BEOSCPU-beos-$VICEVERSION
  else
    $ZIP SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip SDLVICE-$BEOSCPU-beos-$VICEVERSION
  fi
  rm -f -r SDLVICE-$BEOSCPU-beos-$VICEVERSION
  echo BEOS SDL port binary distribution archive generated as SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip
else
  echo BEOS SDL port binary distribution directory generated as SDLVICE-$BEOSCPU-beos-$VICEVERSION
fi
