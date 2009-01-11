#!/bin/bash
#
# build snapshot binaries for x11 gtk and cocoa from the SVN 
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#
# Usage:
#  build-vice-release [-s] [<vice-svn-repository>] [<path to extlib>] [<target dir>] [<arch>] [<uis>]
#  -s = snapshot

echo "--- build VICE release from SVN repository ---"

# is a snapshot
SNAPSHOT=0
if [ "x$1" = "x-s" ]; then
  SNAPSHOT=1
  shift
fi

# check repository directory
SVN_REPO="$1"
if [ "x$SVN_REPO" = x ]; then
  SVN_REPO=vice-emu-trunk
fi
if [ ! -d "$SVN_REPO" ]; then
  echo "ERROR: SVN repository '$SVN_REPO' not found!"
  exit 1
fi
SVN_REPO="`(cd \"$SVN_REPO\" && pwd -P)`"
echo "SVN repository: $SVN_REPO"

# check extlib directory
EXTLIB="$2"
if [ "x$EXTLIB" = x ]; then
  EXTLIB=extlib
fi
if [ ! -d "$EXTLIB" ]; then
  echo "ERROR: external libs '$EXTLIB' not found!"
  exit 1
fi
EXTLIB="`(cd \"$EXTLIB\" && pwd)`"
echo "external libs:  $EXTLIB"

# check target dir
BUILD_DIR="$3"
if [ "x$BUILD_DIR" = x ]; then
  if [ $SNAPSHOT = 1 ]; then
    BUILD_DIR=BUILD-snapshot
  else
    BUILD_DIR=BUILD-release
  fi
fi
if [ -d "$BUILD_DIR" ]; then
  echo "ERROR: $BUILD_DIR already exists!"
  exit 1
fi
BUILD_DIR="`pwd`/$BUILD_DIR"
echo "build dir:      $BUILD_DIR"

# get revision of build
REVISION=`(cd "$SVN_REPO" && svn info | grep Revision | awk '{ print $2 }')`
echo "SVN revision:   $REVISION"

# create build src dir
echo "creating dir    $BUILD_DIR"
mkdir -p "$BUILD_DIR"
if [ ! -d "$BUILD_DIR" ]; then
  echo "ERROR: can't creat dir!"
  exit 1
fi

# export fresh source
SRC_DIR="$BUILD_DIR/src"
echo "exporting src   $SRC_DIR"
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

# configure snapshot
echo "configuring new source tree"
(cd "$SRC_DIR" && ./autogen.sh)
if [ $? != 0 ]; then
  echo "ERROR: configuring VICE..."
  exit 1
fi

# build dists
ARCH="$4"
if [ "x$ARCH" = "x" ]; then
  ARCH="ub"
fi
UI="$5"
if [ "x$UI" = "x" ]; then
  UI="x11 gtk cocoa+10.4 cocoa+10.5"
fi
if [ "$UI" != "none" ]; then
  echo "--- binaries for $UI ---"
  for dist in $UI ; do
    UI_TYPE="${dist%+*}"
    if [ "$UI_TYPE" != "$dist" ]; then
      SDK="${dist#*+}"
    else
      SDK="10.4"
    fi
    echo "building binaries for $UI_TYPE $ARCH $SDK"
    LOG="$BUILD_DIR/build-$dist.log"
    (cd "$SRC_DIR" && $BASH build/macosx/build-vice-dist.sh $ARCH "$UI_TYPE" dmg "$EXTLIB" "$BUILD_DIR" "$SDK") >"$LOG" 2>&1 
    FILES="$(ls $BUILD_DIR/$UI_TYPE-$SDK/$ARCH/*.dmg 2>/dev/null)"
    echo "generated files: $FILES"
    if [ "x$FILES" = "x" ]; then
      echo "no file found!"
      exit 1
    fi
    fgrep +++ "$LOG"
    fgrep warning: "$LOG" | sort | uniq
    du -sh "$FILES"
    mv "$FILES" "$BUILD_DIR"
  done
fi
  
# packing source
echo "--- source ---"
echo "packing source"
DEST_DIR="$BUILD_DIR/tarball"
mkdir "$DEST_DIR"
if [ ! -d "$DEST_DIR" ]; then
  echo "ERROR: creating dir"
  exit 1
fi
(cd "$DEST_DIR" && $SRC_DIR/configure && make dist) >/dev/null 2>&1
FILES="$(ls $DEST_DIR/*.tar.gz 2>/dev/null)"
echo "generated: $FILES"
du -sh "$FILES"
mv "$FILES" "$BUILD_DIR"

echo "--- ready ---"
exit 0
