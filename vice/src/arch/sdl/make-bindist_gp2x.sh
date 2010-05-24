#!/bin/sh
# make-bindist.sh for the GP2X SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4

if [ ! -e src/x64 -o ! -e src/x64dtv -o ! -e src/x64sc -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating GP2X SDL port binary distribution.
rm -f -r SDLVICE-gp2x-$VICEVERSION
mkdir SDLVICE-gp2x-$VICEVERSION
$STRIP src/x64
$STRIP src/x64dtv
$STRIP src/x64sc
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
cp src/x64 src/x64dtv src/x128 src/xvic SDLVICE-gp2x-$VICEVERSION
cp src/xpet src/xplus4 src/xcbm2 src/x64sc SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x64.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x64dtv.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x64sc.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x128.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xvic.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xpet.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xplus4.gpe SDLVICE-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xcbm2.gpe SDLVICE-gp2x-$VICEVERSION
mkdir SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/VIC20 SDLVICE-gp2x-$VICEVERSION/data
cp $TOPSRCDIR/doc/ReadmeSDL.txt SDLVICE-gp2x-$VICEVERSION
mkdir SDLVICE-gp2x-$VICEVERSION/snapshots
mkdir SDLVICE-gp2x-$VICEVERSION/tmp
rm `find SDLVICE-gp2x-$VICEVERSION -name "Makefile*"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "amiga_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "beos_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "dos_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "os2*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "win_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "RO*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-gp2x-$VICEVERSION.zip SDLVICE-gp2x-$VICEVERSION
  else
    $ZIP SDLVICE-gp2x-$VICEVERSION.zip SDLVICE-gp2x-$VICEVERSION
  fi
  rm -f -r SDLVICE-gp2x-$VICEVERSION
  echo GP2X SDL port binary distribution archive generated as SDLVICE-gp2x-$VICEVERSION.zip
else
  echo GP2X SDL port binary distribution directory generated as SDLVICE-gp2x-$VICEVERSION
fi
