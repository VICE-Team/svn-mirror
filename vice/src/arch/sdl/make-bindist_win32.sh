#!/bin/sh
# make-bindist_win32.sh for the SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir> <cpu>
#                 $1      $2             $3          $4           $5

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4
CPU=$5

if [ ! -e src/x64.exe -o ! -e src/x64dtv.exe -o ! -e src/x64sc.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

if test x"$CPU" = "xx86_64" -o x"$CPU" = "xamd64"; then
  WINXX="win64"
else
  WINXX="win32"
fi

echo Generating $WINXX SDL port binary distribution.
rm -f -r SDLVICE-$VICEVERSION-$WINXX
mkdir SDLVICE-$VICEVERSION-$WINXX
$STRIP src/x64.exe
$STRIP src/x64dtv.exe
$STRIP src/x64sc.exe
$STRIP src/x128.exe
$STRIP src/xvic.exe
$STRIP src/xpet.exe
$STRIP src/xplus4.exe
$STRIP src/xcbm2.exe
$STRIP src/c1541.exe
$STRIP src/petcat.exe
$STRIP src/cartconv.exe
cp src/x64.exe src/x64dtv.exe src/x128.exe SDLVICE-$VICEVERSION-$WINXX
cp src/xvic.exe src/xpet.exe src/xplus4.exe SDLVICE-$VICEVERSION-$WINXX
cp src/xcbm2.exe src/c1541.exe src/petcat.exe SDLVICE-$VICEVERSION-$WINXX
cp src/cartconv.exe src/x64sc.exe SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/VIC20 SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/fonts SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/doc/html SDLVICE-$VICEVERSION-$WINXX
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README SDLVICE-$VICEVERSION-$WINXX
cp $TOPSRCDIR/doc/cartconv.txt SDLVICE-$VICEVERSION-$WINXX
cp $TOPSRCDIR/doc/ReadmeSDL.txt SDLVICE-$VICEVERSION-$WINXX
rm `find SDLVICE-$VICEVERSION-$WINXX -name "Makefile*"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "amiga_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "dos_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "os2*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "osx*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "beos_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "x11_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "RO*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "win*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "*.vsc"`
rm SDLVICE-$VICEVERSION-$WINXX/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-$VICEVERSION-$WINXX.zip SDLVICE-$VICEVERSION-$WINXX
  else
    $ZIP SDLVICE-$VICEVERSION-$WINXX.zip SDLVICE-$VICEVERSION-$WINXX
  fi
  rm -f -r SDLVICE-$VICEVERSION-$WINXX
  echo $WINXX SDL port binary distribution archive generated as SDLVICE-$VICEVERSION-$WINXX.zip
else
  echo $WINXX SDL port binary distribution directory generated as SDLVICE-$VICEVERSION-$WINXX
fi
