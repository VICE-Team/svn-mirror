#!/bin/sh
# make-bindist.sh for the WIN32 port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4

if [ ! -e src/x64.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating WIN32 port binary distribution.
rm -f -r WinVICE-$VICEVERSION
mkdir WinVICE-$VICEVERSION
$STRIP src/x64.exe
$STRIP src/x128.exe
$STRIP src/xvic.exe
$STRIP src/xpet.exe
$STRIP src/xplus4.exe
$STRIP src/xcbm2.exe
$STRIP src/c1541.exe
$STRIP src/petcat.exe
$STRIP src/cartconv.exe
cp src/x64.exe src/x128.exe src/xvic.exe WinVICE-$VICEVERSION
cp src/xpet.exe src/xplus4.exe src/xcbm2.exe WinVICE-$VICEVERSION
cp src/c1541.exe src/petcat.exe src/cartconv.exe WinVICE-$VICEVERSION
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $TOPSRCDIR/data/CBM-II $TOPSRCDIR/data/DRIVES WinVICE-$VICEVERSION
cp -a $TOPSRCDIR/data/PET $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER $TOPSRCDIR/data/VIC20 WinVICE-$VICEVERSION
cp -a $TOPSRCDIR/data/fonts WinVICE-$VICEVERSION
cp -a $TOPSRCDIR/doc/html WinVICE-$VICEVERSION
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README WinVICE-$VICEVERSION
cp $TOPSRCDIR/doc/cartconv.txt WinVICE-$VICEVERSION
rm `find WinVICE-$VICEVERSION -name "Makefile*"`
rm `find WinVICE-$VICEVERSION -name "amiga_*.vkm"`
rm `find WinVICE-$VICEVERSION -name "dos_*.vkm"`
rm `find WinVICE-$VICEVERSION -name "os2*.vkm"`
rm `find WinVICE-$VICEVERSION -name "osx*.vkm"`
rm `find WinVICE-$VICEVERSION -name "beos_*.vkm"`
rm `find WinVICE-$VICEVERSION -name "x11_*.vkm"`
rm `find WinVICE-$VICEVERSION -name "RO*.vkm"`
rm WinVICE-$VICEVERSION/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q WinVICE-$VICEVERSION.zip WinVICE-$VICEVERSION
  else
    $ZIP WinVICE-$VICEVERSION.zip WinVICE-$VICEVERSION
  fi
  rm -f -r WinVICE-$VICEVERSION
  echo WIN32 port binary distribution archive generated as WinVICE-$VICEVERSION.zip
else
  echo WIN32 port binary distribution directory generated as WinVICE-$VICEVERSION
fi
