#!/bin/sh
# make-bindist.sh for the GP2X port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

echo Generating GP2X port binary distribution.
rm -f -r vice-gp2x-$2
mkdir vice-gp2x-$2
$1 src/x64
$1 src/x128
$1 src/xvic
$1 src/xpet
$1 src/xplus4
$1 src/xcbm2
cp src/x64 src/x128 src/xvic vice-gp2x-$2
cp src/xpet src/xplus4 src/xcbm2 vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/x64.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/x128.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/xvic.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/xpet.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/xplus4.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/xcbm2.gpe vice-gp2x-$2
cp $4/src/arch/unix/gp2x/gpe-files/vsid.gpe vice-gp2x-$2
mkdir vice-gp2x-$2/data
cp -a $4/data/C128 $4/data/C64 $4/data/CBM-II $4/data/DRIVES vice-gp2x-$2/data
cp -a $4/data/PET $4/data/PLUS4 $4/data/VIC20 vice-gp2x-$2/data
mkdir vice-gp2x-$2/snapshots
mkdir vice-gp2x-$2/tmp
rm `find vice-gp2x-$2 -name "Makefile*"`
rm `find vice-gp2x-$2 -name "amiga_*.vkm"`
rm `find vice-gp2x-$2 -name "beos_*.vkm"`
rm `find vice-gp2x-$2 -name "dos_*.vkm"`
rm `find vice-gp2x-$2 -name "os2*.vkm"`
rm `find vice-gp2x-$2 -name "win_*.vkm"`
if test x"$3" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice-gp2x-$2.zip vice-gp2x-$2
  else
    $ZIP vice-gp2x-$2.zip vice-gp2x-$2
  fi
  rm -f -r vice-gp2x-$2
  echo GP2X port binary distribution archive generated as vice-gp2x-$2.zip
else
  echo GP2X port binary distribution directory generated as vice-gp2x-$2
fi
