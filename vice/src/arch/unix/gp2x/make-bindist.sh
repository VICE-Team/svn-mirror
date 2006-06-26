#!/bin/sh
echo Generating GP2X port binary distribution.
rm -f -r ../VICE2x-$2
mkdir ../VICE2x-$2
$1 src/x64
$1 src/x128
$1 src/xvic
$1 src/xpet
$1 src/xplus4
$1 src/xcbm2
cp src/x64 src/x128 src/xvic ../VICE2x-$2
cp src/xpet src/xplus4 src/xcbm2 ../VICE2x-$2
cp src/arch/gp2x/gpe-files/x64.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/x128.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/xvic.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/xpet.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/xplus4.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/xcbm2.gpe ../VICE2x-$2
cp src/arch/gp2x/gpe-files/vsid.gpe ../VICE2x-$2
cp -a data/C128 data/C64 data/CBM-II data/DRIVES ../VICE2x-$2
cp -a data/PET data/PLUS4 data/VIC20 ../VICE2x-$2
mkdir ../VICE2x-$2/snapshots
mkdir ../VICE2x-$2/tmp
rm `find ../VICE2x-$2 -name "Makefile*"`
rm `find ../VICE2x-$2 -name "amiga_*.vkm"`
rm `find ../VICE2x-$2 -name "beos_*.vkm"`
rm `find ../VICE2x-$2 -name "dos_*.vkm"`
rm `find ../VICE2x-$2 -name "os2*.vkm"`
rm `find ../VICE2x-$2 -name "win_*.vkm"`
if test x"$3" = "xzip"; then
  cd ..
  if test x"$ZIP" = "x"; then
    zip -r -9 -q VICE2x-$2.zip VICE2x-$2
  else
    $ZIP VICE2x-$2.zip VICE2x-$2
  fi
  echo GP2X port binary distribution archive generated as ../VICE2x-$2.zip
else
  echo GP2X port binary distribution directory generated as ../VICE2x-$2
fi
