#!/bin/sh
# make-bindist.sh for the windows ports
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

if [ ! -e src/x64.exe -o ! -e src/x64dtv.exe -o ! -e src/x128.exe -o ! -e src/xvic.exe -o ! -e src/xpet.exe -o ! -e src/xplus4.exe -o ! -e src/xcbm2.exe -o ! -e src/c1541.exe -o ! -e src/petcat.exe -o ! -e src/cartconv.exe ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

if test x"$CPU" = "xx86_64" -o x"$CPU" = "xamd64"; then
  WINXX="Win64"
  WINVICE="WinVICE-$VICEVERSION-x64"
else
  if test x"$CPU" = "xia64"; then
    WINXX="Win64 ia64"
    WINVICE="WinVICE-$VICEVERSION-ia64"
  else
    WINXX="Win32"
    WINVICE="WinVICE-$VICEVERSION-x86"
  fi
fi

echo Generating $WINXX port binary distribution.
rm -f -r $WINVICE
mkdir $WINVICE
$STRIP src/x64.exe
$STRIP src/x64dtv.exe
$STRIP src/x128.exe
$STRIP src/xvic.exe
$STRIP src/xpet.exe
$STRIP src/xplus4.exe
$STRIP src/xcbm2.exe
$STRIP src/c1541.exe
$STRIP src/petcat.exe
$STRIP src/cartconv.exe
cp src/x64.exe src/x64dtv.exe src/x128.exe src/xvic.exe src/xpet.exe $WINVICE
cp src/xplus4.exe src/xcbm2.exe src/c1541.exe src/petcat.exe $WINVICE
cp src/cartconv.exe $WINVICE
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $TOPSRCDIR/data/C64DTV $WINVICE
cp -a $TOPSRCDIR/data/CBM-II $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET $WINVICE
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER $WINVICE
cp -a $TOPSRCDIR/data/VIC20 $TOPSRCDIR/data/fonts $TOPSRCDIR/doc/html $WINVICE
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README $TOPSRCDIR/doc/cartconv.txt $WINVICE
rm `find $WINVICE -name "Makefile*"`
rm `find $WINVICE -name "amiga_*.vkm"`
rm `find $WINVICE -name "dos_*.vkm"`
rm `find $WINVICE -name "os2*.vkm"`
rm `find $WINVICE -name "osx*.vkm"`
rm `find $WINVICE -name "beos_*.vkm"`
rm `find $WINVICE -name "x11_*.vkm"`
rm `find $WINVICE -name "RO*.vkm"`
rm $WINVICE/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q $WINVICE.zip $WINVICE
  else
    $ZIP $WINVICE.zip $WINVICE
  fi
  rm -f -r $WINVICE
  echo $WINXX port binary distribution archive generated as $WINVICE.zip
else
  echo $WINXX port binary distribution directory generated as $WINVICE
fi
