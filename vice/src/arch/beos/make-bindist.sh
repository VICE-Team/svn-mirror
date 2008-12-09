#!/bin/sh
# make-bindist.sh for the BEOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <cpu> <zip|nozip> <top-srcdir>
#                 $1      $2             $3    $4          $5

STRIP=$1
VICEVERSION=$2
CPU=$3
ZIPKIND=$4
TOPSRCDIR=$5

if [ ! -e src/x64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

echo Generating BEOS port binary distribution.
if test x"$CPU" = "xpowerpc" -o x"$CPU" = "xppc"; then
  BEOSCPU=powerpc
else
  BEOSCPU=x86
fi

rm -f -r BeVICE-$VICEVERSION.$BEOSCPU
mkdir BeVICE-$VICEVERSION.$BEOSCPU
$STRIP src/x64
$STRIP src/x64dtv
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
$STRIP src/c1541
$STRIP src/petcat
$STRIP src/cartconv
cp src/x64 src/x64dtv src/x128 src/xvic BeVICE-$VICEVERSION.$BEOSCPU
cp src/xpet src/xplus4 src/xcbm2 BeVICE-$VICEVERSION.$BEOSCPU
cp src/c1541 src/petcat src/cartconv BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/VIC20 BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/fonts BeVICE-$VICEVERSION.$BEOSCPU
mkdir BeVICE-$VICEVERSION.$BEOSCPU/doc
cp -a $TOPSRCDIR/doc/html BeVICE-$VICEVERSION.$BEOSCPU/doc
cp $TOPSRCDIR/doc/iec-bus.txt BeVICE-$VICEVERSION.$BEOSCPU/doc
cp $TOPSRCDIR/doc/mon.txt BeVICE-$VICEVERSION.$BEOSCPU/doc
cp $TOPSRCDIR/doc/cartconv.txt BeVICE-$VICEVERSION.$BEOSCPU/doc
cp $TOPSRCDIR/doc/Readme.beos BeVICE-$VICEVERSION.$BEOSCPU/doc
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README BeVICE-$VICEVERSION.$BEOSCPU
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "Makefile*"`
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "*.vkm" -and ! -name "beos*.vkm"`
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "*.vsc"`
if [ -e BeVICE-$VICEVERSION.$BEOSCPU/html/texi2html ]
then
  rm BeVICE-$VICEVERSION.$BEOSCPU/html/texi2html
fi
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q BeVICE-$VICEVERSION.$BEOSCPU.zip BeVICE-$VICEVERSION.$BEOSCPU
  else
    $ZIP BeVICE-$VICEVERSION.$BEOSCPU.zip BeVICE-$VICEVERSION.$BEOSCPU
  fi
  rm -f -r BeVICE-$VICEVERSION.$BEOSCPU
  echo BEOS port binary distribution archive generated as BeVICE-$VICEVERSION.$BEOSCPU.zip
else
  echo BEOS port binary distribution directory generated as BeVICE-$VICEVERSION.$BEOSCPU
fi
