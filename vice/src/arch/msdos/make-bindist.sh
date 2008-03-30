#!/bin/sh
# make-bindist.sh for the MSDOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-ver-major> <vice-ver-minor> <zip|nozip> <top-srcdir>
#                 $1      $2               $3               $4          $5

if [ ! -e src/x64.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

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
cp -a $5/data/C128 $5/data/C64 $5/data/CBM-II $5/data/DRIVES dosvice
cp -a $5/data/PET $5/data/PLUS4 $5/data/PRINTER $5/data/VIC20 dosvice
cp -a $5/data/fonts dosvice
cp -a $5/doc/html dosvice
cp $5/FEEDBACK $5/README dosvice
cp $5/doc/Readme.dos dosvice
cp $5/doc/cartconv.txt dosvice
rm `find dosvice -name "Makefile*"`
rm `find dosvice -name "amiga_*.vkm"`
rm `find dosvice -name "os2*.vkm"`
rm `find dosvice -name "osx*.vkm"`
rm `find dosvice -name "beos_*.vkm"`
rm `find dosvice -name "win_*.vkm"`
rm `find dosvice -name "x11_*.vkm"`
rm `find dosvice -name "RO*.vkm"`
rm dosvice/html/texi2html
if test x"$4" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice$2$3.zip dosvice
  else
    $ZIP vice$2$3.zip dosvice
  fi
  rm -f -r dosvice
  echo MSDOS port binary distribution archive generated as vice$2$3.zip
else
  echo MSDOS port binary distribution directory generated as dosvice
fi
