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

[ -z ${CODE_SIGN_ID+set} ] && CODE_SIGN_ID=""

echo "Generating macOS binary distribution."
echo "  UI type: $UI_TYPE"

# setup BUILD dir
BUILD_DIR=$(echo "vice-$UI_TYPE-$VICE_VERSION" | tr '[:upper:]' '[:lower:]')

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

  if [ "$lib" = "" ] || [ -e "$lib_dest" ]; then
    return
  fi

  cp "$lib" "$lib_dest"
  chmod 644 "$lib_dest"

  # copy this lib's libs
  LIB_LIBS=`otool -L "$lib_dest" | egrep '^\s+/(opt|usr)/local/' | grep -v "$lib_basename" | awk '{print $1}'`

  for lib_lib in $LIB_LIBS; do
    copy_lib_recursively "$lib_lib"
  done
}

# --- create bundle ------------------------------------------------------------

BUNDLE="VICE"
APP_NAME=$BUILD_DIR/$BUNDLE.app
APP_CONTENTS=$APP_NAME/Contents
APP_MACOS=$APP_CONTENTS/MacOS
APP_RESOURCES=$APP_CONTENTS/Resources
APP_ETC=$APP_RESOURCES/etc
APP_SHARE=$APP_RESOURCES/share
APP_COMMON=$APP_SHARE/vice/common
APP_ICONS=$APP_SHARE/vice/icons
APP_ROMS=$APP_SHARE/vice
APP_DOCS=$APP_SHARE/vice/doc
APP_BIN=$APP_RESOURCES/bin
APP_LIB=$APP_RESOURCES/lib

if ! which -s platypus; then
  echo "ERROR: platypus not found (sudo port install platypus)"
  exit 1
fi

make_app_bundle() {
  local app_name=$1
  local app_path=$BUILD_DIR/$app_name.app
  local app_launcher=$2
  local output=$(mktemp)

  platypus \
    -a $app_name \
    -o None \
    -i "$RUN_PATH/Resources/VICE.icns" \
    -V "$VICE_VERSION" \
    -u "The VICE Team" \
    -I "org.viceteam.$app_name" \
    -c "$app_launcher" \
    -D \
    -R \
    -B \
    "$app_path" \
    2&>1 > $output
  
  PLATYPUS_STATUS=$?
  
  if [ $PLATYPUS_STATUS -ne 0 ]; then
    echo "ERROR: platypus failed with $PLATYPUS_STATUS. Output:"
    cat $output
    rm $output

    exit 1
  fi

  #
  # For some reason can't set the CFBundlePackageType key directly using platypus.
  # Without this the codesigning works but spctl --assess --verbose *.app results
  # in "rejected (the code is valid but does not seem to be an app)" -- which
  # means it won't get past gatekeeper properly, at least on 10.14. But it will on
  # 10.15 if it's notarised. Wtf Apple.
  #

  /usr/libexec/PlistBuddy -c "Add CFBundlePackageType string APPL" "$app_path/Contents/Info.plist"
}

echo "  bundling $BUNDLE.app: "
echo -n "    "

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


# --- copy roms and data into bundle -------------------------------------------

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

# any config files from /etc?
if [ -d etc ]; then
  mkdir -p $APP_ETC
  echo -n "[etc"
  (cd etc && tar cf - *) | (cd "$APP_ETC" && tar xf -)
  echo -n "] "
fi

echo  


# --- embed emu binaries -------------------------------------------------------

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

  echo -n "[platypus] "
  make_app_bundle $emu $RUN_PATH/$REDIRECT_LAUNCHER

  # ready
  echo
done


# --- emu runtime depenencies --------------------------------------------------

cp "$TOP_DIR/src/arch/shared/macOS-common-runtime.sh" "$APP_BIN/common-runtime.sh"

# Can use dtrace in a terminal and run x64sc.app to find runtime libs that aren't directly linked:
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

# Some libs are loaded at runtime
if grep -q "^#define HAVE_EXTERNAL_LAME " "src/config.h"; then
  copy_lib_recursively /opt/local/lib/libmp3lame.dylib
fi

# ffmpeg
if grep -q "^#define EXTERNAL_FFMPEG " "src/config.h"; then
  copy_lib_recursively "$(find /opt/local/lib -type f -name 'libavformat.*.dylib')"
  copy_lib_recursively "$(find /opt/local/lib -type f -name 'libavcodec.*.dylib')"
  copy_lib_recursively "$(find /opt/local/lib -type f -name 'libavutil.*.dylib')"
  copy_lib_recursively "$(find /opt/local/lib -type f -name 'libswscale.*.dylib')"
  copy_lib_recursively "$(find /opt/local/lib -type f -name 'libswresample.*.dylib')"
fi

# --- copy tools ---------------------------------------------------------------

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
  cp src/$tool $APP_BIN/$tool
  
  # strip binary
  if [ x"$STRIP" = "xstrip" ]; then
    echo -n "[strip] "
    /usr/bin/strip $APP_BIN/$tool
  fi

  # copy any needed "local" libs
  LOCAL_LIBS=`otool -L $APP_BIN/$tool | egrep '^\s+/(opt|usr)/local/' | awk '{print $1}'`

  for lib in $LOCAL_LIBS; do
      copy_lib_recursively $lib
  done

  # ready
  echo
done


# --- deduplicate libs ---------------------------------------------------------

echo "Deduplicating libs"

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


# --- update linking for distribution ------------------------------------------

relink () {
  local thing=$1
  local thing_basename=`basename $lib`

  #
  # Any link to a lib in /opt/local is updated to @rpath/$lib
  #

  set +o pipefail
  THING_LIBS=`otool -L $thing | egrep '^\s+/(opt|usr)/local/' | awk '{print $1}'`
  set -o pipefail

  for thing_lib in $THING_LIBS; do
    install_name_tool -change $thing_lib @rpath/$(basename $thing_lib) $thing
  done

  #
  # Update the shared library identification name - ignores non shared libraries
  # so we can blindly call it.
  #

  install_name_tool -id @rpath/$thing_basename $thing

  #
  # If any existing rpaths exist, remove them.
  # Then an an rpath to our bundled lib folder.
  #

  set +o pipefail
  THING_RPATHS=$(otool -l $thing | grep -A2 LC_RPATH | grep path | sed -E 's/^[[:space:]]+path[[:space:]]+(.*)[[:space:]]+\(offset.*$/\1/')
  set -o pipefail

  for rpath in $THING_RPATHS; do
    install_name_tool -delete_rpath $rpath $thing
  done
  
  install_name_tool -add_rpath @executable_path/../lib $thing
}

echo "Relinking libs and binaries to relative bundle paths"

for lib in $(find $APP_LIB -type f -name '*.dylib' -or -name '*.so'); do
  relink $lib
  chmod 444 $lib
done

for bin in $BINARIES $TOOLS; do
  relink $APP_BIN/$bin
  chmod 555 $APP_BIN/$bin
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # these copied cache files would otherwise point to local files
  sed -i '' -e 's,/opt/local,@executable_path/..,' $APP_ETC/gtk-3.0/gtk.immodules 
  sed -i '' -e 's,/opt/local,@executable_path/..,' $APP_ETC/gtk-3.0/gdk-pixbuf.loaders
  sed -i '' -e 's,/opt/local,@executable_path/..,' $(find $APP_LIB/gdk-pixbuf-* -name loaders.cache)
fi


# --- create general command line launchers ------------------------------------

echo "  creating command line launchers"
# TODO replace these with a small C program that we can code sign.
for emu in $EMULATORS $TOOLS; do
  cat << "  HEREDOC" | sed 's/^    //' > "$BIN_DIR/$emu"
    #!/bin/bash
    cd $(dirname "$0")
    export PROGRAM="$(basename "$0")"
    ../VICE.app/Contents/Resources/script "$@"
  HEREDOC
  chmod +x "$BIN_DIR/$emu"
done

cat << "HEREDOC" | sed 's/^  //' > "$BIN_DIR/README.txt"
  These launchers are intended to be invoked from a terminal and make
  visible the log output that is hidden when lanching the *.app versions.

  From macOS 10.15 (Catalina) onwards, double clicking these in Finder
  will not initially work due to not being able to codesign a shell script.
  But after trying, you can open the 'Security & Privacy' section of System
  Preferences and there will be an 'Open Anyway' button that allows it.
HEREDOC


# --- copy docs ----------------------------------------------------------------

echo "  copying documents"

mkdir $BUILD_DIR/doc
cp README $BUILD_DIR/doc/README.txt
cp doc/vice.pdf $BUILD_DIR/doc/

# Readme-GTK3.txt is pointless but the others have content
if [ "$UI_TYPE" != "GTK3" ]; then
  cp "$TOP_DIR/doc/readmes/Readme-$UI_TYPE.txt" $BUILD_DIR/doc/
fi

# --- copy fonts ---------------------------------------------------------------

FONTS="C64_Pro_Mono-STYLE.ttf"
echo "  copying fonts"
for FONT in $FONTS ; do
  cp "$TOP_DIR/data/common/$FONT" "$APP_COMMON/"
done

if [ "$UI_TYPE" = "GTK3" ]; then
  # --- copy vice.gresource ---
  echo "  copying vice.gresource"
  cp "data/common/vice.gresource" "$APP_COMMON/"
fi


# --- wtf permissions. ---------------------------------------------------------

# platypus produces 777 binaries, which is awesome.

find $BUILD_DIR -type f -exec chmod a-w {} \;
find $BUILD_DIR -type d -exec chmod 755 {} \;


# --- code signing (for Apple notarisation) ------------------------------------

code_sign_file () {
  codesign -s "$CODE_SIGN_ID" --timestamp --options runtime -f "$1"
}

if [ -n "$CODE_SIGN_ID" ]; then
  echo "  code signing"

  for lib in $(find $APP_LIB -type f -name '*.dylib' -or -name '*.so'); do
    code_sign_file $lib
  done

  for bin in $BINARIES $TOOLS ; do
    code_sign_file $APP_BIN/$bin
  done

  for app in $(find $BUILD_DIR -type d -name '*.app'); do
    code_sign_file $app
  done
fi


# --- make dmg? ----------------------------------------------------------------

if [ x"$ZIP" = "xnozip" ]; then
  echo "ready. created dist directory: $BUILD_DIR"
  du -sh $BUILD_DIR
else
  du -sh $BUILD_DIR

  # image name
  BUILD_IMG=$BUILD_DIR.dmg
  BUILD_TMP_IMG=$BUILD_DIR.tmp.dmg
  DMG_DIR=.dmg.tmp

  # The dmg will contain a single draggable folder and a symlink to /Applications
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

  # Sign the final DMG
  if [ -n "$CODE_SIGN_ID" ]; then
    echo "  signing DMG"
    code_sign_file $BUILD_IMG
  fi

  echo "ready. created dist file: $BUILD_IMG"
  du -sh $BUILD_IMG
  md5 -q $BUILD_IMG
fi

# --- show warnings ------------------------------------------------------------

if [ -z "$CODE_SIGN_ID" ]; then
  cat << "  HEREDOC" | sed 's/^ *//'

    ****
    Bindist is not codesigned. To sign, export the CODE_SIGN_ID environment variable to
    something like "Developer ID Application: <NAME> (<ID>)".

    Run 'security find-identity -v -p codesigning' to list available identities.
    ****

  HEREDOC
fi

if test x"$ENABLEARCH" = "xyes"; then
  cat << "  HEREDOC" | sed 's/^ *//'
    
    ****
    Warning: binaries are optimized for your system and might not run on a different system,
    use --enable-arch=no to avoid this.
    ****

  HEREDOC
fi
