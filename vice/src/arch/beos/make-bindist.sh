#!/bin/sh
echo Generating BEOS port binary distribution.
rm -f -r vice-$2
mkdir vice-$2
$1 src/x64
$1 src/x128
$1 src/xvic
$1 src/xpet
$1 src/xplus4
$1 src/xcbm2
$1 src/c1541
$1 src/petcat
$1 src/cartconv
cp src/x64 src/x128 src/xvic vice-$2
cp src/xpet src/xplus4 src/xcbm2 vice-$2
cp src/c1541 src/petcat src/cartconv vice-$2
cp -a data/C128 data/C64 data/CBM-II data/DRIVES vice-$2
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 vice-$2
cp -a data/fonts vice-$2
mkdir vice-$2/doc
cp -a doc/html vice-$2/doc
cp doc/iec-bus.txt vice-$2/doc
cp doc/mon.txt vice-$2/doc
cp doc/cartconv.txt vice-$2/doc
cp doc/Readme.beos vice-$2/doc
cp FEEDBACK README vice-$2
rm `find vice-$2 -name "Makefile*"`
rm `find vice-$2 -name "amiga_*.vkm"`
rm `find vice-$2 -name "dos_*.vkm"`
rm `find vice-$2 -name "os2*.vkm"`
rm `find vice-$2 -name "win_*.vkm"`
rm `find vice-$2 -name "x11_*.vkm"`
rm vice-$2/html/texi2html
if test x"$3" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q BeVICE-$2.zip vice-$2
  else
    $ZIP BeVICE-$2.zip vice-$2
  fi
  echo BEOS port binary distribution archive generated as BeVICE-$2.zip
else
  echo BEOS port binary distribution directory generated as vice-$2
fi
