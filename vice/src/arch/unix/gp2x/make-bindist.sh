#!/bin/sh
# make-bindist.sh for the GP2X port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4

echo Generating GP2X port binary distribution.
rm -f -r vice-gp2x-$VICEVERSION
mkdir vice-gp2x-$VICEVERSION
$STRIP src/x64
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
cp src/x64 src/x128 src/xvic vice-gp2x-$VICEVERSION
cp src/xpet src/xplus4 src/xcbm2 vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x64.gpe vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x128.gpe vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xvic.gpe vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xpet.gpe vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xplus4.gpe vice-gp2x-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xcbm2.gpe vice-gp2x-$VICEVERSION
mkdir vice-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $TOPSRCDIR/data/CBM-II $TOPSRCDIR/data/DRIVES vice-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/PET $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/VIC20 vice-gp2x-$VICEVERSION/data
mkdir vice-gp2x-$VICEVERSION/snapshots
mkdir vice-gp2x-$VICEVERSION/tmp
rm `find vice-gp2x-$VICEVERSION -name "Makefile*"`
rm `find vice-gp2x-$VICEVERSION -name "amiga_*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "beos_*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "dos_*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "os2*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "win_*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "RO*.vkm"`
rm `find vice-gp2x-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice-gp2x-$VICEVERSION.zip vice-gp2x-$VICEVERSION
  else
    $ZIP vice-gp2x-$VICEVERSION.zip vice-gp2x-$VICEVERSION
  fi
  rm -f -r vice-gp2x-$VICEVERSION
  echo GP2X port binary distribution archive generated as vice-gp2x-$VICEVERSION.zip
else
  echo GP2X port binary distribution directory generated as vice-gp2x-$VICEVERSION
fi
