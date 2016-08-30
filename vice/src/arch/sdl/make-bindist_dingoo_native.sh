#!/bin/sh

#
# make-bindist.sh - make binary distribution for the native SDL Dingoo port
#
# Written by
#  peiselulli
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#  02111-1307  USA.
#
# Usage: make-bindist.sh <strip> <vice-version> <--enable-arch> <zip|nozip> <top-srcdir>
#                         $1      $2             $3              $4          $5
#

STRIP=$1
VICEVERSION=$2
ENABLEARCH=$3
ZIPKIND=$4
TOPSRCDIR=$5

EMULATORS="x64 x64dtv xscpu64 x128 xpet xplus4 xvic vsid"
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

mkdir Dingoo-$VICEVERSION/doc
cp $TOPSRCDIR/doc/readmes.ReadmeSDL.txt Dingoo-$VICEVERSION/doc
cp $TOPSRCDIR/doc/vice.txt Dingoo-$VICEVERSION/doc

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
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
