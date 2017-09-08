#!/bin/sh

#
# make-bindist.sh - make binary distribution for the windows GTK3 port
#
# Written by
#  Marco van den Heuvel <blackystardust68@yahoo.com>
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
# Usage: make-bindist.sh <strip> <vice-version> <--enable-arch> <zip|nozip> <x64sc-included> <top-srcdir> <cpu>
#                         $1      $2             $3              $4          $5               $6           $7
#

STRIP=$1
VICEVERSION=$2
ENABLEARCH=$3
ZIPKIND=$4
X64SC=$5
TOPSRCDIR=$6
CPU=$7

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 xscpu64 x64dtv $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
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

echo Generating $WINXX GTK3 port binary distribution.
GTK3NAME="GTK3VICE"

rm -f -r $GTK3NAME-$VICEVERSION-$WINXX
mkdir $GTK3NAME-$VICEVERSION-$WINXX
for i in $EXECUTABLES
do
  $STRIP src/$i.exe
  cp src/$i.exe $GTK3NAME-$VICEVERSION-$WINXX
done
cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $GTK3NAME-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II $GTK3NAME-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET $GTK3NAME-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER $GTK3NAME-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/data/SCPU64 $TOPSRCDIR/data/VIC20 $GTK3NAME-$VICEVERSION-$WINXX
cp -a $TOPSRCDIR/doc/html $GTK3NAME-$VICEVERSION-$WINXX
rm $GTK3NAME-$VICEVERSION-$WINXX/html/checklinks.sh
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README $GTK3NAME-$VICEVERSION-$WINXX
cp $TOPSRCDIR/COPYING $TOPSRCDIR/NEWS $GTK3NAME-$VICEVERSION-$WINXX
cp $TOPSRCDIR/doc/readmes/Readme-GTK3.txt $GTK3NAME-$VICEVERSION-$WINXX
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "Makefile*"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "amiga_*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "dos_*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "osx*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "beos_*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "sdl_*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "x11_*.vkm"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "win*.v*"`
rm `find $GTK3NAME-$VICEVERSION-$WINXX -name "*.vsc"`
rm $GTK3NAME-$VICEVERSION-$WINXX/html/texi2html
mkdir $GTK3NAME-$VICEVERSION-$WINXX/doc
cp $TOPSRCDIR/doc/vice.chm $GTK3NAME-$VICEVERSION-$WINXX/doc
cp $TOPSRCDIR/doc/vice.hlp $GTK3NAME-$VICEVERSION-$WINXX/doc
cp $TOPSRCDIR/doc/vice.pdf $GTK3NAME-$VICEVERSION-$WINXX/doc
if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q $GTK3NAME-$VICEVERSION-$WINXX.zip $GTK3NAME-$VICEVERSION-$WINXX
  else
    $ZIP $GTK3NAME-$VICEVERSION-$WINXX.zip $GTK3NAME-$VICEVERSION-$WINXX
  fi
  rm -f -r $GTK3NAME-$VICEVERSION-$WINXX
  echo $WINXX GTK3 port binary distribution archive generated as $GTK3NAME-$VICEVERSION-$WINXX.zip
else
  echo $WINXX GTK3 port binary distribution directory generated as $GTK3NAME-$VICEVERSION-$WINXX
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
