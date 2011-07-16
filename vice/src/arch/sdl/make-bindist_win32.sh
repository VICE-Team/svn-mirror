#!/bin/sh
# make-bindist_win32.sh for the SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <x64sc-included> <top-srcdir> <cpu>
#                 $1      $2             $3          $4               $5           $6

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
X64SC=$4
TOPSRCDIR=$5
CPU=$6

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 x64dtv $SCFILE x128 xcbm2 xpet xplus4 xvic"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i.exe ]
  then
    echo Error: executable file\(s\) not found, do a \"make\" first
    exit 1
  fi
done

if test x"$CPU" = "xx86_64" -o x"$CPU" = "xamd64"; then
  WINXX="win64"
else
  WINXX="win32"
fi

echo Generating $WINXX SDL port binary distribution.
rm -f -r SDLVICE-$VICEVERSION-$WINXX
mkdir SDLVICE-$VICEVERSION-$WINXX
for i in $EXECUTABLES
do
  $STRIP src/$i.exe
  cp src/$i.exe SDLVICE-$VICEVERSION-$WINXX
done
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/VIC20 SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/fonts SDLVICE-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/doc/html SDLVICE-$VICEVERSION-$WINXX
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README SDLVICE-$VICEVERSION-$WINXX
cp $TOPSRCDIR/doc/readmes/Readme-SDL.txt SDLVICE-$VICEVERSION-$WINXX
rm `find SDLVICE-$VICEVERSION-$WINXX -name "Makefile*"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "amiga_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "dos_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "os2*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "osx*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "beos_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "x11_*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "RO*.vkm"`
rm `find SDLVICE-$VICEVERSION-$WINXX -name "win*.v*"`
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
