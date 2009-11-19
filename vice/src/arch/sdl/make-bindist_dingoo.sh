#!/bin/sh
# make-bindist.sh for the DINGOO SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4

echo Generating DINGOO SDL port binary distribution.
rm -f -r SDLVICE-dingoo-$VICEVERSION
mkdir SDLVICE-dingoo-$VICEVERSION
$STRIP src/x64
$STRIP src/x64dtv
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
cp src/x64 src/x64dtv src/x128 src/xvic SDLVICE-dingoo-$VICEVERSION
cp src/xpet src/xplus4 src/xcbm2 SDLVICE-dingoo-$VICEVERSION
cp $TOPSRCDIR/src/arch/sdl/dingoo-files/*.dge SDLVICE-dingoo-$VICEVERSION
cp $TOPSRCDIR/src/arch/sdl/dingoo-files/sdl-vicerc SDLVICE-dingoo-$VICEVERSION
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 SDLVICE-dingoo-$VICEVERSION
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-dingoo-$VICEVERSION
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-dingoo-$VICEVERSION
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET SDLVICE-dingoo-$VICEVERSION
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/VIC20 SDLVICE-dingoo-$VICEVERSION
cp $TOPSRCDIR/doc/ReadmeSDL.txt SDLVICE-dingoo-$VICEVERSION
rm `find SDLVICE-dingoo-$VICEVERSION -name "Makefile*"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "amiga_*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "beos_*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "dos_*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "os2*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "win_*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "RO*.vkm"`
rm `find SDLVICE-dingoo-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-dingoo-$VICEVERSION.zip SDLVICE-dingoo-$VICEVERSION
  else
    $ZIP SDLVICE-dingoo-$VICEVERSION.zip SDLVICE-dingoo-$VICEVERSION
  fi
  rm -f -r SDLVICE-dingoo-$VICEVERSION
  echo DINGOO SDL port binary distribution archive generated as SDLVICE-dingoo-$VICEVERSION.zip
else
  echo DINGOO SDL port binary distribution directory generated as SDLVICE-dingoo-$VICEVERSION
fi
