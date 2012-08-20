#!/bin/sh
# make-bindist.sh for the GP2X SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <--enable-arch> <zip|nozip> <top-srcdir>
#                 $1      $2             $3              $4          $5

STRIP=$1
VICEVERSION=$2
ENABLEARCH=$3
ZIPKIND=$4
TOPSRCDIR=$5

EMULATORS="x64 x64dtv x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
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

echo Generating GP2X SDL port binary distribution.
rm -f -r SDLVICE-gp2x-$VICEVERSION
mkdir SDLVICE-gp2x-$VICEVERSION
for i in $EXECUTABLES
do
  $STRIP src/$i
  cp src/$i SDLVICE-gp2x-$VICEVERSION
done
for i in $EMULATORS
do
  cp $TOPSRCDIR/src/arch/unix/gp2x/gpe-files/$i.gpe SDLVICE-gp2x-$VICEVERSION
done
mkdir SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET SDLVICE-gp2x-$VICEVERSION/data
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/VIC20 SDLVICE-gp2x-$VICEVERSION/data
cp $TOPSRCDIR/doc/readmes/Readme-SDL.txt SDLVICE-gp2x-$VICEVERSION
mkdir SDLVICE-gp2x-$VICEVERSION/snapshots
mkdir SDLVICE-gp2x-$VICEVERSION/tmp
rm `find SDLVICE-gp2x-$VICEVERSION -name "Makefile*"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "amiga_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "beos_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "dos_*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "os2*.vkm"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "win_*.v*"`
rm `find SDLVICE-gp2x-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-gp2x-$VICEVERSION.zip SDLVICE-gp2x-$VICEVERSION
  else
    $ZIP SDLVICE-gp2x-$VICEVERSION.zip SDLVICE-gp2x-$VICEVERSION
  fi
  rm -f -r SDLVICE-gp2x-$VICEVERSION
  echo GP2X SDL port binary distribution archive generated as SDLVICE-gp2x-$VICEVERSION.zip
else
  echo GP2X SDL port binary distribution directory generated as SDLVICE-gp2x-$VICEVERSION
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
