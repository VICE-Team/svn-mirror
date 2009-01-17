#!/bin/bash
#
# build snapshot binaries for x11 gtk and cocoa from the SVN 
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#

echo "--- build VICE release from SVN repository ---"

# default parameter
SNAPSHOT=0
LINK_SRC=0
ONLY_BIN=0
DEL_TARGET=0
EXTLIB="extlib"
BUILD_DIR=""
ARCH="ub"
UI="x11 gtk cocoa+10.4 cocoa+10.5"

usage() {
  cat <<EOF

  Usage:
    $0 [options] <vice-svn-repository>

  Options:                                                                 Default:
    -s                       do snapshot build                             [release build]
    -l                       link repository directory and use it directly [fresh svn export]
    -b                       only binaries and no source package           [with source package]
    -f                       force delete target directory if it exists    [abort if exists]
    -e <path to extlib>      set path of external library directory        [extlib]
    -o <target dir>          set target directory                          [BUILD-snapshot/release]
    -a <arch>                set arch type, e.g. i386 ppc ub               [$ARCH]
    -u <uis>                 set uis <ui>[+<sdk>]                          [$UI]
EOF
  exit 2
}

# parse arguments
while getopts "slbfe:o:a:u:" i ; do
  case "$i" in
    s) SNAPSHOT=1;;
    l) LINK_SRC=1;;
    b) ONLY_BIN=1;;
    f) DEL_TARGET=1;;
    e) EXTLIB="$OPTARG";;
    o) BUILD_DIR="$OPTARG";;
    a) ARCH="$OPTARG";;
    u) UI="$OPTARG";;
    ?) usage;;
  esac
done
shift $(($OPTIND-1))

# mode
if [ $SNAPSHOT = 1 ]; then
  echo "mode:           snapshot"
else
  echo "mode:           RELASE"
fi

# check repository directory
SVN_REPO="$1"
if [ "x$SVN_REPO" = "x" ]; then
  echo "ERROR: no repository given!"
  usage
fi
if [ ! -d "$SVN_REPO" ]; then
  echo "ERROR: SVN repository '$SVN_REPO' not found!"
  exit 1
fi
if [ ! -s "$SVN_REPO/.svn" ]; then
  echo "ERROR: No SVN repository '$SVN_REPO'!"
  exit 1
fi
if [ ! -x "$SVN_REPO/autogen.sh" ]; then
  echo "ERROR: SVN repository '$SVN_REPO' contains no VICE!"
  exit 1
fi
SVN_REPO="`(cd \"$SVN_REPO\" && pwd -P)`"
echo "SVN repository: $SVN_REPO"

# get revision of build
REVISION=`(cd "$SVN_REPO" && svn info | grep Revision | awk '{ print $2 }')`
echo "SVN revision:   $REVISION"

# check extlib directory
if [ ! -d "$EXTLIB" ]; then
  echo "ERROR: external libs '$EXTLIB' not found!"
  exit 1
fi
EXTLIB="`(cd \"$EXTLIB\" && pwd)`"
echo "external libs:  $EXTLIB"

# check target dir
if [ "x$BUILD_DIR" = x ]; then
  if [ $SNAPSHOT = 1 ]; then
    BUILD_DIR=BUILD-snapshot
  else
    BUILD_DIR=BUILD-release
  fi
fi
if [ -d "$BUILD_DIR" ]; then
  if [ $DEL_TARGET = 0 ]; then
    echo "ERROR: $BUILD_DIR already exists!"
    exit 1
  else
    rm -rf "$BUILD_DIR"
  fi
fi
BUILD_DIR="`pwd`/$BUILD_DIR"
echo "build dir:      $BUILD_DIR"

# create build src dir
mkdir -p "$BUILD_DIR"
if [ ! -d "$BUILD_DIR" ]; then
  echo "ERROR: can't creat dir!"
  exit 1
fi

SRC_DIR="$BUILD_DIR/src"
if [ $LINK_SRC = 1 ]; then
  # link existing source
  echo "linking src:    $SRC_DIR"
  ln -sf "$SVN_REPO" "$SRC_DIR"
else
  # export fresh source
  echo "exporting src:  $SRC_DIR"
  svn export "$SVN_REPO" "$SRC_DIR"
  if [ $? != 0 ]; then
    echo "ERROR: export faild!"
    exit 1
  fi

  # patch BUILD version if doing snapshot
  if [ $SNAPSHOT = 1 ]; then
    # tag
    DATE=`date '+%Y%m%d'`
    TAG="-r${REVISION}_$DATE"

    # patch VICE_VERSION_BUILD
    echo "patching configure.in: $TAG"
    PATCH_VVB="$SRC_DIR/configure.in"
    perl -pi -e "s/(VICE_VERSION_BUILD=\d+).*\$/\$1$TAG/" $PATCH_VVB
    if [ $? != 0 ]; then
      echo "ERROR: patching..."
      exit 1
    fi
  fi
fi

# configure snapshot
echo "--- configuring source tree ---"
(cd "$SRC_DIR" && ./autogen.sh)
if [ $? != 0 ]; then
  echo "ERROR: configuring VICE..."
  exit 1
fi

# build dists
if [ "$UI" != "none" ]; then
  echo "--- binaries for $UI ---"
  for dist in $UI ; do
    UI_TYPE="${dist%+*}"
    if [ "$UI_TYPE" != "$dist" ]; then
      SDK="${dist#*+}"
    else
      SDK="10.4"
    fi
    echo "-- building binaries for $UI_TYPE $ARCH $SDK --"
    LOG="$BUILD_DIR/build-$dist.log"
    (cd "$SRC_DIR" && $BASH build/macosx/build-vice-dist.sh $ARCH "$UI_TYPE" dmg "$EXTLIB" "$BUILD_DIR" "$SDK") >"$LOG" 2>&1 
    FILES="$(ls $BUILD_DIR/$UI_TYPE-$SDK/$ARCH/*.dmg 2>/dev/null)"
    echo "generated files: $FILES"
    if [ "x$FILES" = "x" ]; then
      echo "no file found!"
      exit 1
    fi
    fgrep +++ "$LOG"
    echo " -warnings begin-"
    fgrep warning: "$LOG" | sort | uniq
    echo " -warnings end-"
    du -sh "$FILES"
    mv "$FILES" "$BUILD_DIR"
  done
fi
  
# packing source
if [ $ONLY_BIN = 0 ]; then
  echo "--- source ---"
  echo "packing source"
  DEST_DIR="$BUILD_DIR/tarball"
  mkdir "$DEST_DIR"
  if [ ! -d "$DEST_DIR" ]; then
    echo "ERROR: creating dir"
    exit 1
  fi
  (cd "$DEST_DIR" && $SRC_DIR/configure && make dist) >/dev/null 2>&1
  # move generated files to top level
  FILES="$(ls $DEST_DIR/*.tar.gz 2>/dev/null)"
  echo "generated: $FILES"
  du -sh "$FILES"
  mv "$FILES" "$BUILD_DIR"
fi

echo "--- ready ---"
exit 0
