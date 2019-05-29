#!/bin/bash

#
# make-bindist.sh - make binary distribution for the Mac OSX port
#
# Written by
#  Christian Vogelgsang <chris@vogelgsang.org>
#  David Hogan <david.q.hogan@gmail.com>
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
# Usage: make-bindist.sh <top_srcdir> <strip> <vice-version> <--enable-arch> <zip|nozip> <ui_type>
#                         $1           $2      $3             $4              $5         $6
#

echo "Generating macOS binary distribution."
echo "  UI type: $UI_TYPE"

# check binary type
TEST_BIN=src/x64sc
if [ ! -x $TEST_BIN ]; then
  echo "error missing binary $TEST_BIN"
  exit 1
fi
BIN_TYPE=`file $TEST_BIN | grep "$TEST_BIN:" | sed -e 's/executable//g' -e 's/Mach-O//g' -e 's/64-bit//g' | awk '{print $2}'`
if [ x"$BIN_TYPE" = "xi386" ]; then
  BIN_FORMAT=i386
elif [ x"$BIN_TYPE" = "xx86_64" ]; then
  BIN_FORMAT=x86_64
elif [ x"$BIN_TYPE" = "xppc" ]; then
  BIN_FORMAT=ppc
else
  echo "fatal: unknown bin type '$BIN_TYPE'"
  exit 1
fi
echo "  binary format: $BIN_FORMAT"

# setup BUILD dir
BUILD_DIR=VICE-macOS-$UI_TYPE-$BIN_FORMAT-$VICE_VERSION
TOOL_DIR=$BUILD_DIR/tools

if [ -d $BUILD_DIR ]; then
  rm -rf $BUILD_DIR
fi

mkdir $BUILD_DIR
mkdir $TOOL_DIR

# define emulators and command line tools
EMULATORS="xscpu64 x64dtv x64sc x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
TOOLS="c1541 petcat cartconv"

# define data files for emulators
ROM_COMMON="DRIVES PRINTER"
ROM_x64=C64
ROM_xscpu64=SCPU64
ROM_x64sc=C64
ROM_x64dtv=C64DTV
ROM_x128=C128
ROM_xcbm2=CBM-II
ROM_xcbm5x0=CBM-II
ROM_xpet=PET
ROM_xplus4=PLUS4
ROM_xvic=VIC20
ROM_vsid=C64
# files to remove from ROM directory

if [ "$UI_TYPE" = "GTK3" ]; then
  ROM_REMOVE="sdl_*.v?m"
elif [ "$UI_TYPE" = "SDL2" ]; then
  ROM_REMOVE="gtk3_*.v?m"
fi

DOC_REMOVE="Makefile.* *.c *.mak *.sh *.tex *.texi *.pl *.chm *.guide *.hlp *.inf building readmes"
# define droppable file types
DROP_TYPES="x64|p64|g64|d64|d71|d81|t64|tap|prg|p00|crt|reu"
DROP_FORMATS="x64 p64 g64 d64 d71 d81 t64 tap prg p00 crt reu"

# launcher scripts
LAUNCHER=macOS-launcher.sh
REDIRECT_LAUNCHER=../shared/macOS-redirect-launcher.sh

copy_tree () {
  (cd "$1" && tar --exclude 'Makefile*' --exclude .svn -c -f - .) | (cd "$2" && tar xf -)
}

create_info_plist () {
  SRC="$1"
  TGT="$2"

  # add filetypes to Info.plist
  
  sed -e "s/XVERSIONX/$VICE_VERSION/g" \
      -e "s/XNAMEX/$BUNDLE/g" \
      < "$SRC" > "$TGT"
}

copy_lib_recursively () {
    local lib=$1
    local lib_basename=`basename $lib`
    local lib_dest="$APP_LIB/$lib_basename"

    if [ -e "$lib_dest" ]; then
      return
    fi

    cp $lib $lib_dest

    # Process this lib's libs
    LIB_LIBS=`otool -L $lib_dest | egrep '^\s+/(opt|usr)/local/' | grep -v $lib_basename | awk '{print $1}'`

    for lib_lib in $LIB_LIBS; do
        copy_lib_recursively $lib_lib
    done
}

# --- create bundle ---

# make sure icon is available
if [ ! -e $RUN_PATH/Resources/VICE.icns ]; then
  echo "ERROR: missing icon: $RUNPATH/Resources/VICE.icns"
  exit 1
fi

# make sure Info.plist is available
if [ ! -e $RUN_PATH/Info.plist ]; then
  echo "ERROR: missing: $RUN_PATH/Info.plist"
  exit 1
fi

BUNDLE="VICE"
APP_NAME=$BUILD_DIR/$BUNDLE.app
APP_CONTENTS=$APP_NAME/Contents
APP_MACOS=$APP_CONTENTS/MacOS
APP_RESOURCES=$APP_CONTENTS/Resources
APP_ETC=$APP_RESOURCES/etc
APP_SHARE=$APP_RESOURCES/share
APP_GUI=$APP_RESOURCES/lib/vice/gui
APP_FONTS=$APP_RESOURCES/lib/vice/fonts
APP_ICONS=$APP_RESOURCES/lib/vice/icons
APP_ROMS=$APP_RESOURCES/lib/vice
APP_DOCS=$APP_RESOURCES/lib/vice/doc
APP_BIN=$APP_RESOURCES/bin
APP_LIB=$APP_RESOURCES/lib

echo "  bundling $BUNDLE.app: "
echo -n "    "

# --- use platypus for bundling ---

set +o errexit
PLATYPUS_PATH="`which platypus`"
set -o errexit
if [ -e "$PLATYPUS_PATH" ]; then
  PLATYPUS_VERSION=`$PLATYPUS_PATH -v | cut -f 3 -d ' '`
  echo "  using platypus: $PLATYPUS_PATH version $PLATYPUS_VERSION"
else
  echo "ERROR: platypus not found (brew install platypus)"
  exit 1
fi

# create a dedicated .app bundle that launches via VICE.app
echo -n "[platypus] "
$PLATYPUS_PATH \
    -a VICE \
    -o None \
    -i $RUN_PATH/Resources/VICE.icns \
    -V "$VICE_VERSION" \
    -u "The VICE Team" \
    -I "org.viceteam.VICE" \
    -c $RUN_PATH/$LAUNCHER \
    -R \
    -B \
    $APP_NAME
PLATYPUS_STATUS=$?
if [ $PLATYPUS_STATUS -ne 0 ]; then
  echo "ERROR: platypus failed with $PLATYPUS_STATUS"
  exit $PLATYPUS_STATUS
fi

# # destination for launcher script
# LAUNCHER_SCRIPT_REL="Resources/script"
# # make launcher executable
# chmod 755 $APP_RESOURCES/script

echo -n "[dirs] "
mkdir -p $APP_ROMS
mkdir -p $APP_DOCS
mkdir -p $APP_BIN
mkdir -p $APP_LIB
mkdir -p $APP_SHARE
mkdir -p $APP_FONTS
mkdir -p $APP_ICONS
mkdir -p $APP_GUI

# copy roms and data into bundle
echo -n "[common ROMs] "
for rom in $ROM_COMMON ; do
  if [ ! -d $TOP_DIR/data/$rom ]; then
    echo "ERROR: missing ROM: $TOP_DIR/data/$rom"
    exit 1
  fi
  if [ ! -d "$APP_ROMS/$rom" ]; then
      mkdir "$APP_ROMS/$rom"
  fi
  copy_tree "$TOP_DIR/data/$rom" "$APP_ROMS/$rom"
  (cd "$APP_ROMS/$rom" && eval "rm -f $ROM_REMOVE")
done

# copy manual into bundle
echo -n "[manual] "
cp "$TOP_DIR/doc/vice.pdf" "$APP_DOCS"

# embed c1541
echo -n "[c1541] "
if [ ! -e src/c1541 ]; then
  echo "ERROR: missing binary: src/c1541"
  exit 1
fi
cp src/c1541 $APP_BIN/

# strip embedded c1541 binary
if [ x"$STRIP" = "xstrip" ]; then
  echo -n "[strip c1541] "
  /usr/bin/strip $APP_BIN/c1541
fi

# # any dylibs required?
# if [ -d lib ]; then
#   mkdir -p $APP_LIB
#   DYLIBS=`find lib -name *.dylib`
#   NUMDYLIBS=`echo $DYLIBS | wc -w`
#   echo -n "[dylibs"
#   for lib in $DYLIBS ; do
#     echo -n "."
#     cp $lib $APP_LIB
#   done
#   echo -n "] "
# fi

# any config files from /etc?
if [ -d etc ]; then
  mkdir -p $APP_ETC
  echo -n "[etc"
  (cd etc && tar cf - *) | (cd "$APP_ETC" && tar xf -)
  echo -n "] "
fi

# ready with bundle
echo  

# --- embed binaries ---
if [ "$BUNDLE" = "VICE" ]; then
  BINARIES="$EMULATORS"
else
  BINARIES="$BUNDLE"
fi
for emu in $BINARIES ; do
  echo -n "     embedding $emu: "

  # copy binary
  echo -n "[binary] "
  if [ ! -e src/$emu ]; then
    echo "ERROR: missing binary: src/$emu"
    exit 1
  fi
  cp src/$emu $APP_BIN/$emu

  # strip binary
  if [ x"$STRIP" = "xstrip" ]; then
    echo -n "[strip] "
    /usr/bin/strip $APP_BIN/$emu
  fi

  # copy any needed "local" libs
  LOCAL_LIBS=`otool -L $APP_BIN/$emu | egrep '^\s+/(opt|usr)/local/' | awk '{print $1}'`

  for lib in $LOCAL_LIBS; do
      copy_lib_recursively $lib

      # relink the emu binary to the relative lib copy
      install_name_tool -change $lib @executable_path/../lib/$(basename $lib) $APP_BIN/$emu
  done

  # copy emulator ROM
  eval "ROM=\${ROM_$emu}"
  echo -n "[ROM=$ROM] "
  if [ ! -d $TOP_DIR/data/$ROM ]; then
    echo "ERROR: missing ROM: $TOP_DIR/data/$ROM"
    exit 1
  fi
  if [ ! -d "$APP_ROMS/$ROM" ]; then
      mkdir "$APP_ROMS/$ROM"
  fi
  copy_tree "$TOP_DIR/data/$ROM" "$APP_ROMS/$ROM"
  (cd $APP_ROMS/$ROM && eval "rm -f $ROM_REMOVE")

  # ready
  echo

  # create a dedicated .app bundle that launches via VICE.app
  echo -n "[platypus] "
  $PLATYPUS_PATH \
      -a $emu \
      -o None \
      -i $RUN_PATH/Resources/VICE.icns \
      -V "$VICE_VERSION" \
      -u "The VICE Team" \
      -I "org.viceteam.VICE" \
      -c $RUN_PATH/$REDIRECT_LAUNCHER \
      -R \
      -B \
      $BUILD_DIR/$emu
  PLATYPUS_STATUS=$?
  if [ $PLATYPUS_STATUS -ne 0 ]; then
    echo "ERROR: platypus failed with $PLATYPUS_STATUS"
    exit $PLATYPUS_STATUS
  fi
done


# --- runtime depenencies ---


# Can use dtrace in a terminal and run vsid.app to find these:
# sudo dtrace -n 'syscall::stat*:entry /execname=="vsid"/ { printf("%s", copyinstr(arg0)); }'
# sudo dtrace -n 'syscall::open*:entry /execname=="vsid"/ { printf("%s", copyinstr(arg0)); }'

if [ "$UI_TYPE" = "GTK3" ]; then
  cp -r /usr/local/lib/gdk-pixbuf-* $APP_LIB
  cp -r /usr/local/lib/gtk-3.* $APP_LIB
  cp -r /usr/local/etc/gtk-3.* $APP_ETC
  cp -r /usr/local/share/glib-* $APP_SHARE

  # various locale stuff
  mkdir $APP_SHARE/locale
  cp -r /usr/local/opt/gettext/share/locale/* $APP_SHARE/locale
  cp -r /usr/local/opt/gtk+3/share/locale/* $APP_SHARE/locale

  import_scalable_gtk_icons() {
    local in_icons="/usr/local/share/icons/$1"
    local out_icons="$APP_SHARE/icons/$1"

    mkdir "$out_icons"
    # Rewrite the index.theme file to exclude large non-vector assets
    awk '
      BEGIN {
        directories_found = 0
      }
      /^Directories=/ {
        directories_found = 1
        printf "Directories="
        system("echo " $0 " | tr \"=,\" \"\n\" | grep ^scalable | tr \"\n\" \",\" ")
        printf "\n"
        next
      }
      {
        if (!directories_found) {
          print
          next
        }
      }
      /^\[/ {
        eat = 0
        if ($0 !~ /^\[scalable\//) {
          eat = 1
        }
      }
      {
        if (!eat) {
          print
        }
      }' < "$in_icons/index.theme" > "$out_icons/index.theme"
    cp -r "$in_icons/scalable" "$out_icons/"

    # create the icon-theme.cache file
    gtk3-update-icon-cache "$out_icons/"
  }

  mkdir $APP_SHARE/icons
  import_scalable_gtk_icons Adwaita

  # dqh: disabled hicolor on macos as i don't think it's the full version and it doesn't
  # appear to be used ...
  #
  # # hicolor is small, just copy it. It doesn't have any scalable assets.
  # cp -r /usr/local/share/icons/hicolor "$APP_SHARE/icons/"
  # gtk3-update-icon-cache "$APP_SHARE/icons/hicolor"
fi

# .so libs need their libs too
for lib in `find $APP_LIB -name '*.so'`; do
    LIB_LIBS=`otool -L $lib | egrep '^\s+/(opt|usr)/local/' | awk '{print $1}'`

    for lib_lib in $LIB_LIBS; do
        copy_lib_recursively $lib_lib
    done
done

# Something loads this at runtime, thankfully DYLD_FALLBACK_LIBRARY_PATH will be searched for it
copy_lib_recursively /usr/local/lib/libmp3lame.dylib

# --- update lib linking ---

relink_lib () {
    local lib=$1
    local lib_basename=`basename $lib`

    LIB_LIBS=`otool -L $lib | egrep '^\s+/(opt|usr)/local/' | grep -v $lib_basename | awk '{print $1}'`

    chmod 644 $lib

    for lib_lib in $LIB_LIBS; do
        install_name_tool -change $lib_lib @executable_path/../lib/$(basename $lib_lib) $lib
    done

    chmod 444 $lib
}

for lib in `find $APP_LIB -name '*.dylib'`; do
    relink_lib $lib
done
for lib in `find $APP_LIB -name '*.so'`; do
    relink_lib $lib
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # these copied cache files would otherwise point to local files
  sed -i '' -e 's,/usr/local/Cellar/gtk+3/[^/]*/,@executable_path/../,g' $(find $APP_LIB/gtk-* -name immodules.cache)
  sed -i '' -e 's,/usr/local/lib,@executable_path/../lib,' $(find $APP_LIB/gdk-pixbuf-* -name loaders.cache)
fi

# print bundle size
echo -n "    => " ; du -sh "$APP_NAME" | awk '{ print $1 }'

# --- copy tools ---
for tool in $TOOLS ; do
  echo -n "  copying tool $tool: "

  # copy binary
  echo -n "[binary] "
  if [ ! -e src/$tool ]; then
    echo "ERROR: missing binary: src/$tool"
    exit 1
  fi
  cp src/$tool $TOOL_DIR/
  
  # strip binary
  if [ x"$STRIP" = "xstrip" ]; then
    echo -n "[strip] "
    /usr/bin/strip $TOOL_DIR/$tool
  fi

  # ready
  echo
done

# --- copy general command line launcher ---
echo "  linking example command line launchers"
for emu in $EMULATORS ; do
  (cd $TOOL_DIR && ln -s ../VICE.app/Contents/Resources/bin/$emu)
done

# --- copy docs ---
echo "  copying documents"
cp $TOP_DIR/FEEDBACK $BUILD_DIR/FEEDBACK.txt
cp $TOP_DIR/README $BUILD_DIR/README.txt
mkdir "$BUILD_DIR/doc"
copy_tree "$TOP_DIR/doc" "$BUILD_DIR/doc"
mv $BUILD_DIR/doc/readmes/Readme-$UI_TYPE.txt $BUILD_DIR/
(cd $BUILD_DIR/doc && eval "rm -rf $DOC_REMOVE")

# --- copy fonts ---
FONTS="CBM.ttf"
echo "  copying fonts"
for FONT in $FONTS ; do
  cp "$TOP_DIR/data/fonts/$FONT" "$APP_FONTS/"
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # --- copy vice.gresource ---
  echo "  copying vice.gresource"
  cp "$TOP_DIR/src/arch/gtk3/data/vice.gresource" "$APP_GUI/"
fi

# --- sign apps with apple developer id ---
echo "  TODO: sign app bundles with apple developer id"

# --- make dmg? ---
if [ x"$ZIP" = "xnozip" ]; then
  echo "ready. created dist directory: $BUILD_DIR"
  du -sh $BUILD_DIR
else
  # image name
  BUILD_IMG=$BUILD_DIR.dmg
  BUILD_TMP_IMG=$BUILD_DIR.tmp.dmg
  
  # Create the image and format it
  echo "  creating DMG"
  hdiutil create -fs HFS+ -srcfolder $BUILD_DIR $BUILD_TMP_IMG -volname $BUILD_DIR -ov -quiet

  # Compress the image
  echo "  compressing DMG"
  hdiutil convert $BUILD_TMP_IMG -format UDZO -o $BUILD_IMG -ov -quiet
  rm -f $BUILD_TMP_IMG

  echo "ready. created dist file: $BUILD_IMG"
  du -sh $BUILD_IMG
  md5 -q $BUILD_IMG
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
