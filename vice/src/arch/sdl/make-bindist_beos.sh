#!/bin/sh

#
# make-bindist.sh - make binary distribution for the BeOS SDL port
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
# Usage: make-bindist.sh <strip> <vice-version> <cpu> <--enable-arch> <zip|nozip> <x64sc-included> <top-srcdir>
#                         $1      $2             $3    $4              $5          $6               $7
#

STRIP=$1
VICEVERSION=$2
CPU=$3
ENABLEARCH=$4
ZIPKIND=$5
X64SC=$6
TOPSRCDIR=$7

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 x64dtv xscpu64 $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
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

echo Generating BEOS SDL port binary distribution.
if test x"$CPU" = "xpowerpc" -o x"$CPU" = "xppc"; then
  BEOSCPU=powerpc
else
  BEOSCPU=x86
fi

rm -f -r SDLVICE-$BEOSCPU-beos-$VICEVERSION
mkdir SDLVICE-$BEOSCPU-beos-$VICEVERSION
for i in $EXECUTABLES
do
  $STRIP src/$i
  cp src/$i SDLVICE-$BEOSCPU-beos-$VICEVERSION
done
cp -a $TOPSRCDIR/data/C128 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/C64 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/SCPU64 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/DRIVES SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PET SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PLUS4 SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/PRINTER SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp -a $TOPSRCDIR/data/VIC20 SDLVICE-$BEOSCPU-beos-$VICEVERSION

cp -a $TOPSRCDIR/doc/html SDLVICE-$BEOSCPU-beos-$VICEVERSION
rm SDLVICE-$BEOSCPU-beos-$VICEVERSION/html/checklinks.sh
cp $TOPSRCDIR/doc/readmes/Readme-SDL.txt SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README SDLVICE-$BEOSCPU-beos-$VICEVERSION
cp $TOPSRCDIR/COPYING $TOPSRCDIR/NEWS SDLVICE-$BEOSCPU-beos-$VICEVERSION
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "Makefile*"`
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "*.vkm" -and ! -name "sdl*.vkm"`
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "*.vsc"`
rm `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name "win_*.v*"`
rm SDLVICE-$BEOSCPU-beos-$VICEVERSION/html/texi2html

# just in case ...
rm -f -r `find SDLVICE-$BEOSCPU-beos-$VICEVERSION -name ".svn"`

mkdir SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc
cp $TOPSRCDIR/doc/vice.pdf SDLVICE-$BEOSCPU-beos-$VICEVERSION/doc

if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip SDLVICE-$BEOSCPU-beos-$VICEVERSION
  else
    $ZIP SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip SDLVICE-$BEOSCPU-beos-$VICEVERSION
  fi
  rm -f -r SDLVICE-$BEOSCPU-beos-$VICEVERSION
  echo BEOS SDL port binary distribution archive generated as SDLVICE-$BEOSCPU-beos-$VICEVERSION.zip
else
  echo BEOS SDL port binary distribution directory generated as SDLVICE-$BEOSCPU-beos-$VICEVERSION
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
