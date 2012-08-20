#!/bin/sh
# make-bindist.sh for the GP2X port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <system> <strip> <vice-version> <--enable-arch> <zip|nozip> <top-srcdir>
#                 $1       $2      $3             $4              $5          $6

SYSTEM=$1
STRIP=$2
VICEVERSION=$3
ENABLEARCH=$4
ZIPKIND=$5
TOPSRCDIR=$6

if test x"$SYSTEM" = "xGP2X"; then
  smallsystem="gp2x"
fi

if test x"$SYSTEM" = "xWIZ"; then
  smallsystem="wiz"
fi

echo Generating $SYSTEM port binary distribution.
rm -f -r vice-$smallsystem-$VICEVERSION
mkdir vice-$smallsystem-$VICEVERSION
$STRIP src/x64
$STRIP src/x128
$STRIP src/xvic
$STRIP src/xpet
$STRIP src/xplus4
$STRIP src/xcbm2
cp src/x64 src/x128 src/xvic vice-$smallsystem-$VICEVERSION
cp src/xpet src/xplus4 src/xcbm2 vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x64.gpe vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/x128.gpe vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xvic.gpe vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xpet.gpe vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xplus4.gpe vice-$smallsystem-$VICEVERSION
cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/xcbm2.gpe vice-$smallsystem-$VICEVERSION
mkdir vice-$smallsystem-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $TOPSRCDIR/data/CBM-II $TOPSRCDIR/data/DRIVES vice-$smallsystem-$VICEVERSION/data
cp -a $TOPSRCDIR/data/PET $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/VIC20 vice-$smallsystem-$VICEVERSION/data
mkdir vice-$smallsystem-$VICEVERSION/snapshots
mkdir vice-$smallsystem-$VICEVERSION/tmp
rm `find vice-$smallsystem-$VICEVERSION -name "Makefile*"`
rm `find vice-$smallsystem-$VICEVERSION -name "amiga_*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "beos_*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "dos_*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "os2*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "win_*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "sdl*.vkm"`
rm `find vice-$smallsystem-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q vice-$smallsystem-$VICEVERSION.zip vice-$smallsystem-$VICEVERSION
  else
    $ZIP vice-$smallsystem-$VICEVERSION.zip vice-$smallsystem-$VICEVERSION
  fi
  rm -f -r vice-$smallsystem-$VICEVERSION
  echo $SYSTEM port binary distribution archive generated as vice-$smallsystem-$VICEVERSION.zip
else
  echo $SYSTEM port binary distribution directory generated as vice-$smallsystem-$VICEVERSION
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
