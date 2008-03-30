#!/bin/sh
echo Generating MSDOS port binary distribution.
rm -f -r dosvice
mkdir dosvice
$1 src/x64.exe
$1 src/x128.exe
$1 src/xvic.exe
$1 src/xpet.exe
$1 src/xplus4.exe
$1 src/xcbm2.exe
$1 src/c1541.exe
$1 src/petcat.exe
$1 src/cartconv.exe
cp src/x64.exe src/x128.exe src/xvic.exe dosvice
cp src/xpet.exe src/xplus4.exe src/xcbm2.exe dosvice
cp src/c1541.exe src/petcat.exe src/cartconv.exe dosvice
cp -a data/C128 data/C64 data/CBM-II data/DRIVES dosvice
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 dosvice
cp -a data/fonts dosvice
cp -a doc/html dosvice
cp FEEDBACK README dosvice
cp doc/Readme.dos dosvice
cp doc/cartconv.txt dosvice
rm `find dosvice -name "Makefile*"`
rm `find dosvice -name "amiga_*.vkm"`
rm `find dosvice -name "os2*.vkm"`
rm `find dosvice -name "beos_*.vkm"`
rm `find dosvice -name "win_*.vkm"`
rm `find dosvice -name "x11_*.vkm"`
rm dosvice/html/texi2html
if test x"$4" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice$2$3.zip dosvice
  else
    $ZIP vice$2$3.zip dosvice
  fi
  echo MSDOS port binary distribution archive generated as vice$2$3.zip
else
  echo MSDOS port binary distribution directory generated as dosvice
fi
