#!/bin/sh
# make-bindist.sh for the DINGOO SDL port
#
# written by peiselulli
#
# make-bindist.sh <strip> <vice-version> <zip|nozip> <top-srcdir>
#                 $1      $2             $3          $4

STRIP=$1
VICEVERSION=$2
ZIPKIND=$3
TOPSRCDIR=$4

EMULATORS="x64 x64dtv x128 xpet xplus4 xvic"
CONSOLE_TOOLS=""
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i ]
  then
    echo Error: executable file\(s\) not found, do a \"make\" first
    exit 1
  fi
done

echo Generating DINGOO native port binary distribution.
rm -f -r Dingoo-$VICEVERSION
mkdir Dingoo-$VICEVERSION
for i in $EXECUTABLES
do
  cp $TOPSRCDIR/src/$i Dingoo-$VICEVERSION/$i.elf
  $DINGOO_SDK/tools/elf2app/elf2app Dingoo-$VICEVERSION/$i
  rm Dingoo-$VICEVERSION/$i.elf
  rm Dingoo-$VICEVERSION/$i.bin
done

cp $TOPSRCDIR/src/arch/sdl/dingoo-files/vicerc-native Dingoo-$VICEVERSION/vicerc-native
cp $TOPSRCDIR/src/arch/sdl/dingoo-files/hotkey-native Dingoo-$VICEVERSION/sdl-hotkey-C64.vkm

if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q Dingoo-$VICEVERSION.zip Dingoo-$VICEVERSION
  else
    $ZIP Dingoo-$VICEVERSION.zip Dingoo-$VICEVERSION
  fi
  rm -f -r Dingoo-$VICEVERSION
  echo DINGOO native port binary distribution archive generated as Dingoo-$VICEVERSION.zip
else
  echo DINGOO native port binary distribution directory generated as Dingoo-$VICEVERSION
fi
