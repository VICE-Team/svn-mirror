#!/bin/sh

#
# make-bindist.sh - make binary distribution for the windows SDL port
#
# Written by
#  Marco van den Heuvel <blackystardust68@yahoo.com>
#  Bas Wassink <b.wassink@ziggo.nl>
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
# Usage: make-bindist.sh <strip=$1> <vice-version=$2> <--enable-arch=$3>
#                        <zip|nozip=$4> <x64-included=$5> <top-srcdir=$6>
#                        <top-builddir=$7> <cpu=$8> <SDL-version=$9>
#

STRIP=$1
VICEVERSION=$2
ENABLEARCH=$3
ZIPKIND=$4
X64INC=$5
TOPSRCDIR=$6
TOPBUILDDIR=$7
CPU=$8
SDLVERSION=$9


# Try to get the SVN revision
#echo "Trying to get SVN revision"
SVN_SUFFIX=""
svnrev_string=`svnversion $TOPSRCDIR`
if test "$?" != "0"; then
    #echo "No svnversion found"
    # nop:
    :
else
    # Choose the second number (usually higher) if it exists; drop letter suffixes.
    svnrev=`echo "$svnrev_string" | sed 's/^\([0-9]*:\)*\([0-9]*\)*.*/\2/'`
    #echo "svnrev string: $svnrev"
    # Only a number is extracted.
    if test -n "$svnrev"
        then SVN_SUFFIX="-r$svnrev"
    fi
fi


# check if we have the old x64 binary
if test x"$X64INC" = "xyes"; then
  X64FILE="x64"
else
  X64FILE=""
fi


EMULATORS="$X64FILE x64sc xscpu64 x64dtv x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
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

if test x"$SDLVERSION" = "x2"; then
  echo Generating $WINXX SDL2 port binary distribution.
  SDLNAME="SDL2VICE"
else
  echo Generating $WINXX SDL port binary distribution.
  SDLNAME="SDLVICE"
fi

BINDIST_DIR="$SDLNAME-$VICEVERSION-$WINXX$SVN_SUFFIX"

rm -f -r $BINDIST_DIR
mkdir $BINDIST_DIR

# strip binaries. FIXME: shouldn't this only happen with --disable-debug?
for i in $EXECUTABLES
do
  $STRIP src/$i.exe
  cp src/$i.exe $BINDIST_DIR
done


if test x"$CROSS" != "xtrue"; then
  # assume MSYS2 on Windows here
  BUILDPATH=$BINDIST_DIR
  cp `ntldd -R $BUILDPATH/x64sc.exe|gawk '/\\\\bin\\\\/{print $3;}'|cygpath -f -` $BUILDPATH
fi

cp -a $TOPSRCDIR/data/C128 $TOPSRCDIR/data/C64 $BINDIST_DIR
cp -a $TOPSRCDIR/data/C64DTV $TOPSRCDIR/data/CBM-II $BINDIST_DIR
cp -a $TOPSRCDIR/data/DRIVES $TOPSRCDIR/data/PET $BINDIST_DIR
cp -a $TOPSRCDIR/data/PLUS4 $TOPSRCDIR/data/PRINTER $BINDIST_DIR
cp -a $TOPSRCDIR/data/SCPU64 $TOPSRCDIR/data/VIC20 $BINDIST_DIR
cp -a $TOPSRCDIR/doc/html $BINDIST_DIR
rm -f $BINDIST_DIR/html/checklinks.sh
cp $TOPSRCDIR/FEEDBACK $TOPSRCDIR/README $BINDIST_DIR
cp $TOPSRCDIR/COPYING $TOPSRCDIR/NEWS $BINDIST_DIR
cp $TOPSRCDIR/doc/readmes/Readme-SDL.txt $BINDIST_DIR
rm -f `find $BINDIST_DIR -name "Makefile*"`
rm -f `find $BINDIST_DIR -name "amiga_*.vkm"`
rm -f `find $BINDIST_DIR -name "osx*.vkm"`
rm -f `find $BINDIST_DIR -name "beos_*.vkm"`
rm -f `find $BINDIST_DIR -name "x11_*.vkm"`
rm -f $BINDIST_DIR/html/texi2html

mkdir $BINDIST_DIR/doc
# FIXME: This doesn't work with out-of-tree builds, we need a $TOPSRCDIR, but
#        that isn't passed to this script.
cp $TOPBUILDDIR/doc/vice.pdf $BINDIST_DIR/doc

if test x"$ZIPKIND" = "xzip"; then
  if test x"$ZIP" = "x"; then
    zip -r -9 -q $BINDIST_DIR.zip $BINDIST_DIR
  else
    $ZIP $BINDIST_DIR.zip $BINDIST_DIR
  fi
  rm -f -r $BINDIST_DIR
  if test x"$SDLVERSION" = "x2"; then
      echo $WINXX SDL2 port binary distribution archive generated as $BINDIST_DIR.zip
  else
    echo $WINXX SDL port binary distribution archive generated as $BINDIST_DIR.zip
  fi
else
  if test x"$SDLVERSION" = "x2"; then
    echo $WINXX SDL2 port binary distribution directory generated as $BINDIST_DIR
  else
    echo $WINXX SDL port binary distribution directory generated as $BINDIST_DIR
  fi
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
