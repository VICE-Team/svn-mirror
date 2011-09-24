#!/bin/sh
# make-bindist.sh for the BEOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <cpu> <zip|nozip> <x64sc-included> <top-srcdir>
#                 $1      $2             $3    $4          $5               $6

STRIP=$1
VICEVERSION=$2
CPU=$3
ZIPKIND=$4
X64SC=$5
TOPSRCDIR=$6

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 x64dtv $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i ]
  then
    echo Error: executable file\(s\) not found, do a \"make\" first
    exit 1
  fi
done

echo Generating BEOS port binary distribution.
if test x"$CPU" = "xpowerpc" -o x"$CPU" = "xppc"; then
  BEOSCPU=powerpc
else
  BEOSCPU=x86
fi

rm -f -r BeVICE-$VICEVERSION.$BEOSCPU
mkdir BeVICE-$VICEVERSION.$BEOSCPU
for i in $EXECUTABLES
do
  $STRIP src/$i
  cp src/$i BeVICE-$VICEVERSION.$BEOSCPU
done
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/VIC20 BeVICE-$VICEVERSION.$BEOSCPU
cp -a $TOPSRCDIR/data/fonts BeVICE-$VICEVERSION.$BEOSCPU

cp -a $TOPSRCDIR/doc/html BeVICE-$VICEVERSION.$BEOSCPU
cp $TOPSRCDIR/doc/readmes/Readme-BeOS.txt BeVICE-$VICEVERSION.$BEOSCPU
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README BeVICE-$VICEVERSION.$BEOSCPU
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "Makefile*"`
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "*.vkm" -and ! -name "beos*.vkm"`
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "*.vsc"`
rm `find BeVICE-$VICEVERSION.$BEOSCPU -name "win_*.v*"`
rm BeVICE-$VICEVERSION.$BEOSCPU/html/texi2html

# just in case ...
rm -f -r `find BeVICE-$VICEVERSION.$BEOSCPU -name ".svn"`

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
