#!/bin/sh
# make-bindist.sh for the MSDOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-ver-major> <vice-ver-minor> <zip|nozip> <top-srcdir>
#                 $1      $2               $3               $4          $5

STRIP=$1
VICEVERMAJOR=$2
VICEVERMINOR=$3
ZIPKIND=$4
TOPSRCDIR=$5

if [ ! -e src/x64.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating MSDOS port binary distribution.
rm -f -r dosvice
mkdir dosvice
$STRIP src/x64.exe
$STRIP src/x128.exe
$STRIP src/xvic.exe
$STRIP src/xpet.exe
$STRIP src/xplus4.exe
$STRIP src/xcbm2.exe
$STRIP src/c1541.exe
$STRIP src/petcat.exe
$STRIP src/cartconv.exe
cp src/x64.exe src/x128.exe src/xvic.exe dosvice
cp src/xpet.exe src/xplus4.exe src/xcbm2.exe dosvice
cp src/c1541.exe src/petcat.exe src/cartconv.exe dosvice
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $TOPSRCDIR/data/CBM-II $TOPSRCDIR/data/DRIVES dosvice
cp -a $TOPSRCDIR/data/PET $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER $TOPSRCDIR/data/VIC20 dosvice
cp -a $TOPSRCDIR/data/fonts dosvice
cp -a $TOPSRCDIR/doc/html dosvice
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README dosvice
cp $TOPSRCDIR/doc/Readme.dos dosvice
cp $TOPSRCDIR/doc/cartconv.txt dosvice
rm `find dosvice -name "Makefile*"`
rm `find dosvice -name "amiga_*.vkm"`
rm `find dosvice -name "os2*.vkm"`
rm `find dosvice -name "osx*.vkm"`
rm `find dosvice -name "beos_*.vkm"`
rm `find dosvice -name "win_*.vkm"`
rm `find dosvice -name "x11_*.vkm"`
rm `find dosvice -name "RO*.vkm"`
rm dosvice/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice$VICEVERMAJOR$VICEVERMINOR.zip dosvice
  else
    $ZIP vice$VICEVERMAJOR$VICEVERMINOR.zip dosvice
  fi
  rm -f -r dosvice
  echo MSDOS port binary distribution archive generated as vice$VICEVERMAJOR$VICEVERMINOR.zip
else
  echo MSDOS port binary distribution directory generated as dosvice
fi
