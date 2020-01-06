#!/bin/bash
set -o errexit
set -o nounset

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

# setup BUILD dir
BUILD_DIR=VICE-$UI_TYPE-$VICE_VERSION

SVN_VERSION=$(svn info --show-item revision "$TOP_DIR" 2>/dev/null || true)
if [[ ! -z "$SVN_VERSION" ]]; then
  BUILD_DIR="$BUILD_DIR-r$SVN_VERSION"
  # if the source is not clean, add '-uncommitted-changes' to the name
  if [[ ! -z "$(svn status -q "$TOP_DIR")" ]]; then
    BUILD_DIR="$BUILD_DIR-uncommitted-changes"
  fi
fi

if [ -d $BUILD_DIR ]; then
  rm -rf $BUILD_DIR
fi

mkdir $BUILD_DIR

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

# define droppable file types
DROP_TYPES="x64|p64|g64|d64|d71|d81|t64|tap|prg|p00|crt|reu"
DROP_FORMATS="x64 p64 g64 d64 d71 d81 t64 tap prg p00 crt reu"

# runtime scripts
MACOS_SCRIPTS=macOS-runtime-scripts.inc
LAUNCHER=../shared/macOS-launcher.sh
REDIRECT_LAUNCHER=../shared/macOS-redirect-launcher.sh

copy_tree () {
  (cd "$1" && tar --exclude 'Makefile*' --exclude .svn -c -f - .) | (cd "$2" && tar xf -)
}

resolve_link () {
  local link=$1
  local link_target=$(readlink $link)

  echo "$(cd $(dirname $link) && cd $(dirname $link_target) && pwd -P)/$(basename $link_target)"
}

copy_lib_recursively () {
  local lib="$1"
  local lib_basename=`basename "$lib"`
  local lib_dest="$APP_LIB/$lib_basename"

  if [ -e "$lib_dest" ]; then
    return
  fi

  cp "$lib" "$lib_dest"

  # Process this lib's libs
  LIB_LIBS=`otool -L "$lib_dest" | egrep '^\s+/(opt|usr)/local/' | grep -v "$lib_basename" | awk '{print $1}'`

  for lib_lib in $LIB_LIBS; do
    copy_lib_recursively "$lib_lib"
  done
}

# --- create bundle ---

BUNDLE="VICE"
APP_NAME=$BUILD_DIR/$BUNDLE.app
APP_CONTENTS=$APP_NAME/Contents
APP_MACOS=$APP_CONTENTS/MacOS
APP_RESOURCES=$APP_CONTENTS/Resources
APP_ETC=$APP_RESOURCES/etc
APP_SHARE=$APP_RESOURCES/share
APP_COMMON=$APP_RESOURCES/lib/vice/common
APP_ICONS=$APP_RESOURCES/lib/vice/icons
APP_ROMS=$APP_RESOURCES/lib/vice
APP_DOCS=$APP_RESOURCES/lib/vice/doc
APP_BIN=$APP_RESOURCES/bin
APP_LIB=$APP_RESOURCES/lib

echo "  bundling $BUNDLE.app: "
echo -n "    "

# --- use platypus for bundling ---


PLATYPUS_PATH="/opt/local/bin/platypus"
if [ -e "$PLATYPUS_PATH" ]; then
  PLATYPUS_VERSION=`$PLATYPUS_PATH -v | cut -f 3 -d ' '`
  echo "  using platypus: $PLATYPUS_PATH version $PLATYPUS_VERSION"
else
  echo "ERROR: platypus not found (sudo port install platypus)"
  exit 1
fi

make_app_bundle() {
  local app_name=$1
  local app_path=$BUILD_DIR/$app_name.app
  local app_launcher=$2

  echo -n "[platypus] "
  $PLATYPUS_PATH \
      -a $app_name \
      -o None \
      -i "$RUN_PATH/Resources/VICE.icns" \
      -V "$VICE_VERSION" \
      -u "The VICE Team" \
      -I "org.viceteam.$app_name" \
      -c "$2" \
      -R \
      -B \
      "$app_path"
  PLATYPUS_STATUS=$?
  if [ $PLATYPUS_STATUS -ne 0 ]; then
    echo "ERROR: platypus failed with $PLATYPUS_STATUS"
    exit $PLATYPUS_STATUS
  fi
}

make_app_bundle $BUNDLE $RUN_PATH/$LAUNCHER

echo -n "[dirs] "
mkdir -p $APP_ETC
mkdir -p $APP_SHARE
mkdir -p $APP_COMMON
mkdir -p $APP_ICONS
mkdir -p $APP_ROMS
mkdir -p $APP_DOCS
mkdir -p $APP_BIN
mkdir -p $APP_LIB


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
cp "doc/vice.pdf" "$APP_DOCS"

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

  make_app_bundle $emu $RUN_PATH/$REDIRECT_LAUNCHER
done


# --- runtime depenencies ---
cp "$TOP_DIR/src/arch/shared/macOS-common-runtime.sh" "$APP_BIN/common-runtime.sh"

# Can use dtrace in a terminal and run vsid.app to find runtime libs that aren't directly linked:
# sudo dtrace -n 'syscall::*stat*:entry /execname=="x64sc"/ { printf("%s", copyinstr(arg0)); }'
# sudo dtrace -n 'syscall::*open*:entry /execname=="x64sc"/ { printf("%s", copyinstr(arg0)); }'

if [ "$UI_TYPE" = "SDL2" ]; then
  cp "$TOP_DIR/src/arch/sdl/macOS-ui-runtime.sh" "$APP_BIN/ui-runtime.sh"
elif [ "$UI_TYPE" = "GTK3" ]; then
  cp "$TOP_DIR/src/arch/gtk3/macOS-ui-runtime.sh" "$APP_BIN/ui-runtime.sh"

  # Gtk runtime stuff
  cp -r /opt/local/lib/gdk-pixbuf-2.0 $APP_LIB
  cp -r /opt/local/lib/gtk-3.0 $APP_LIB
  cp -r /opt/local/etc/gtk-3.0 $APP_ETC
  cp -r /opt/local/share/glib-2.0 $APP_SHARE

  # Get rid of any compiled python that came with glib share
  find $APP_SHARE -name '*.pyc' -exec rm {} \;

  import_scalable_gtk_icons() {
    local in_icons="/opt/local/share/icons/$1"
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
    gtk-update-icon-cache-3.0 "$out_icons/"
  }

  mkdir $APP_SHARE/icons

  import_scalable_gtk_icons Adwaita

  # hicolor is small, just copy it. It doesn't have any scalable assets.
  cp -r /opt/local/share/icons/hicolor "$APP_SHARE/icons/"
  gtk-update-icon-cache-3.0 "$APP_SHARE/icons/hicolor"
fi

# .so libs need their libs too
for lib in `find $APP_LIB -name '*.so'`; do
  LIB_LIBS=`otool -L $lib | egrep '^\s+/(opt|usr)/local/' | awk '{print $1}'`

  for lib_lib in $LIB_LIBS; do
    copy_lib_recursively $lib_lib
  done
done

# Some libs are loaded at runtime, thankfully DYLD_FALLBACK_LIBRARY_PATH will be searched for them
copy_lib_recursively /opt/local/lib/libmp3lame.dylib


# --- deduplicate libs ---

echo "Deduplicating libs ..."

for lib in $(find $APP_LIB -name '*.dylib' | sort -V -r); do
  if [ -L "$lib" ]; then
    continue
  fi

  for potential_duplicate in $(find $APP_LIB -type f -name '*.dylib' | sort -V -r); do
    if [ "$potential_duplicate" = "$lib" ]; then
      continue
    fi

    if cmp -s "$potential_duplicate" "$lib"; then
      echo "Replacing $(basename $potential_duplicate) with symlink to $(basename $lib)"
      rm "$potential_duplicate"
      ln -s "$(basename "$lib")" "$potential_duplicate"
    fi
  done
done


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

echo "Relinking libs to relative bundle paths ..."

for lib in $(find $APP_LIB -type f -name '*.dylib' -or -name '*.so'); do
    relink_lib $lib
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # these copied cache files would otherwise point to local files
  sed -i '' -e 's,/opt/local,@executable_path/..,' $APP_ETC/gtk-3.0/gtk.immodules 
  sed -i '' -e 's,/opt/local,@executable_path/..,' $APP_ETC/gtk-3.0/gdk-pixbuf.loaders
  sed -i '' -e 's,/opt/local,@executable_path/..,' $(find $APP_LIB/gdk-pixbuf-* -name loaders.cache)
fi


# --- copy tools ---

BIN_DIR=$BUILD_DIR/bin
mkdir $BIN_DIR

for tool in $TOOLS ; do
  echo -n "  copying tool $tool: "

  # copy binary
  echo -n "[binary] "
  if [ ! -e src/$tool ]; then
    echo "ERROR: missing binary: src/$tool"
    exit 1
  fi
  cp src/$tool $BIN_DIR/
  
  # strip binary
  if [ x"$STRIP" = "xstrip" ]; then
    echo -n "[strip] "
    /usr/bin/strip $BIN_DIR/$tool
  fi

  # ready
  echo
done

# --- create general command line launchers ---
echo "  creating command line launchers"
for emu in $EMULATORS ; do
  cat <<-"  HEREDOC" > "$BIN_DIR/$emu"
    #!/bin/bash
    cd $(dirname "$0")
    export PROGRAM="$(basename "$0")"
    ../VICE.app/Contents/Resources/script
  HEREDOC
  chmod +x "$BIN_DIR/$emu"
done

# --- copy docs ---
echo "  copying documents"
cp $TOP_DIR/FEEDBACK $BUILD_DIR/FEEDBACK.txt

mkdir $BUILD_DIR/doc
cp README $BUILD_DIR/doc/README.txt
cp doc/vice.pdf $BUILD_DIR/doc/

# Readme-GTK3.txt is pointless but the others have content
if [ "$UI_TYPE" != "GTK3" ]; then
  cp "$TOP_DIR/doc/readmes/Readme-$UI_TYPE.txt" $BUILD_DIR/doc/
fi

# --- copy fonts ---
FONTS="CBM.ttf"
echo "  copying fonts"
for FONT in $FONTS ; do
  cp "$TOP_DIR/data/common/$FONT" "$APP_COMMON/"
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # --- copy vice.gresource ---
  echo "  copying vice.gresource"
  cp "src/arch/gtk3/data/vice.gresource" "$APP_COMMON/"
fi

# --- make dmg? ---
if [ x"$ZIP" = "xnozip" ]; then
  echo "ready. created dist directory: $BUILD_DIR"
  du -sh $BUILD_DIR
else
  du -sh $BUILD_DIR

  # image name
  BUILD_IMG=$BUILD_DIR.dmg
  BUILD_TMP_IMG=$BUILD_DIR.tmp.dmg
  DMG_DIR=.dmg.tmp

  # Want the dmg folder to contain a single draggable folder
  rm -rf $DMG_DIR
  mkdir $DMG_DIR
  ln -s /Applications $DMG_DIR/
  mv $BUILD_DIR $DMG_DIR
  
  # Create the image and format it
  echo "  creating DMG"
  hdiutil create -fs HFS+ -srcfolder $DMG_DIR $BUILD_TMP_IMG -volname $BUILD_DIR -ov -quiet

  mv $DMG_DIR/$BUILD_DIR .
  rm -rf $DMG_DIR

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
