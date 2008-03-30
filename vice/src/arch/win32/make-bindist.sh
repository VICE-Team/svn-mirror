#!/bin/sh
# make-bindist.sh for the WIN32 port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

if [ ! -e src/x64.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating WIN32 port binary distribution.
rm -f -r WinVICE-$2
mkdir WinVICE-$2
$1 src/x64.exe
$1 src/x128.exe
$1 src/xvic.exe
$1 src/xpet.exe
$1 src/xplus4.exe
$1 src/xcbm2.exe
$1 src/c1541.exe
$1 src/petcat.exe
$1 src/cartconv.exe
cp src/x64.exe src/x128.exe src/xvic.exe WinVICE-$2
cp src/xpet.exe src/xplus4.exe src/xcbm2.exe WinVICE-$2
cp src/c1541.exe src/petcat.exe src/cartconv.exe WinVICE-$2
cp -a $4/data/C128 $4/data/C64 $4/data/CBM-II $4/data/DRIVES WinVICE-$2
cp -a $4/data/PET $4/data/PLUS4 $4/data/PRINTER $4/data/VIC20 WinVICE-$2
cp -a $4/data/fonts WinVICE-$2
cp -a $4/doc/html WinVICE-$2
cp $4/FEEDBACK $4/README WinVICE-$2
cp $4/doc/cartconv.txt WinVICE-$2
rm `find WinVICE-$2 -name "Makefile*"`
rm `find WinVICE-$2 -name "amiga_*.vkm"`
rm `find WinVICE-$2 -name "dos_*.vkm"`
rm `find WinVICE-$2 -name "os2*.vkm"`
rm `find WinVICE-$2 -name "osx*.vkm"`
rm `find WinVICE-$2 -name "beos_*.vkm"`
rm `find WinVICE-$2 -name "x11_*.vkm"`
rm `find WinVICE-$2 -name "RO*.vkm"`
rm WinVICE-$2/html/texi2html
if test x"$3" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q WinVICE-$2.zip WinVICE-$2
  else
    $ZIP WinVICE-$2.zip WinVICE-$2
  fi
  rm -f -r WinVICE-$2
  echo WIN32 port binary distribution archive generated as WinVICE-$2.zip
else
  echo WIN32 port binary distribution directory generated as WinVICE-$2
fi
