# build-vice-dist.sh
#
# build a binary distribution of VICE for macs
# you can select the architecture (including universal binaries),
# the ui type (x11 or gtk) and the dist type (dir or dmg)
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#
# call this script from vice top-level directory!
#

echo "---------- Build Mac Binary Distribution ----------"

# check for vice version in configure.in
if [ ! -e configure.in ]; then
  echo "please run this script from inside the VICE source directory!"
  exit 1
fi
# fetch vice version
eval `grep 'VICE_VERSION_[A-Z]*=' configure.in`
if test x"$VICE_VERSION_BUILD" = "x" -o x"$VICE_VERSION_BUILD" = "x0" ; then
  VICE_VERSION=$VICE_VERSION_MAJOR"."$VICE_VERSION_MINOR
else
  VICE_VERSION=$VICE_VERSION_MAJOR"."$VICE_VERSION_MINOR"."$VICE_VERSION_BUILD
fi

# ----- check args -----
if [ "x$1" = "x" ]; then
  echo "Usage: $0 <arch> <ui-type> <dist-type> <extlib-dir> <build-dir> [sdk-ver]"
  echo "   arch        Build architecture       ub,i386,ppc"
  echo "   ui-type     User Interface Type      x11,gtk,cocoa"
  echo "   dist-type   Type of Distribution     dmg,dir"
  echo "   extlib-dir  External Libraries"
  echo "   build-dir   Where VICE is built"
  echo "   sdk-ver     Select SDK version       10.5"
  exit 1
fi
ARCH="$1"
if [ "$ARCH" != "ub" -a "$ARCH" != "i386" -a "$ARCH" != "ppc" ]; then
  echo "Wrong architecture given: use 'ub', 'i386', or 'ppc'!"
  exit 1
fi
UI_TYPE="$2"
if [ "$UI_TYPE" != "x11" -a "$UI_TYPE" != "gtk" -a "$UI_TYPE" != "cocoa" ]; then
  echo "Wrong UI Type given: use 'x11' or 'gtk', or 'cocoa'!"
  exit 1
fi
DIST_TYPE="$3"
if [ "$DIST_TYPE" != "dir" -a "$DIST_TYPE" != "dmg" ]; then
  echo "Wrong Build Target: use 'dir' or 'dmg'!"
  exit 1
fi
EXTLIB_DIR="`cd \"$4\" && pwd`"
if [ ! -d "$EXTLIB_DIR" ]; then
  echo "ExtLib Directory '$EXTLIB_DIR' does not exist!"
  exit 1
fi
BUILD_DIR="`cd \"$5\" && pwd`"
if [ "$BUILD_DIR" = "" ]; then
  echo "No Build Directory given!"
  exit 1
fi
SDK_VERSION="$6"
if [ "x$SDK_VERSION" = "x" ]; then
  SDK_VERSION="10.4"
fi
echo "  architecture: $ARCH"
echo "  ui type:      $UI_TYPE"
echo "  dist type:    $DIST_TYPE"
echo "  ext lib dir:  $EXTLIB_DIR"
echo "  build dir:    $BUILD_DIR"
echo "  sdk version:  $SDK_VERSION"
echo

# ----- determine build options -----
echo "----- Determine Build Options -----"

# check if a library is available for the selected architecture
check_lib () {
  local libName="$1"
  echo -n "checking for library '$libName' ... "
  # check for i386 lib
  if [ "$ARCH" = "ub" -o "$ARCH" = "i386" ]; then
    local I386_PATH="$EXTLIB_DIR/i386/lib/$libName"
    if [ ! -f "$I386_PATH" ]; then
      echo " i386 build missing ($I386_PATH)"
      false
      return
    fi
  fi
  # check for ppc lib
  if [ "$ARCH" = "ub" -o "$ARCH" = "ppc" ]; then
    local PPC_PATH="$EXTLIB_DIR/ppc/lib/$libName"
    if [ ! -f "$PPC_PATH" ]; then
      echo " ppc build missing! ($PPC_PATH)"
      false
      return
    fi
  fi
  echo " available for $ARCH"
  true
}

# default configure flags
CONFIGURE_FLAGS="--disable-nls"

# check for gtk+ libs
if [ "$UI_TYPE" = "gtk" ]; then
  check_lib "libgtk-x11-2.0.dylib"
  if [ "$?" != "0" ]; then
    echo "gtk requested, but no gtk+ libs found!"
    exit 1
  fi
  CONFIGURE_FLAGS="--enable-gnomeui $CONFIGURE_FLAGS"
  CONFIGURE_OPTS="Gtk+"
elif [ "$UI_TYPE" = "cocoa" ]; then
  CONFIGURE_FLAGS="--with-cocoa $CONFIGURE_FLAGS"
  CONFIGURE_OPTS="Cocoa"
fi

# check for hidutil
check_lib "libHIDUtilities.a"
if [ "$?" = "0" ]; then
  echo "+++ With Joystick Support +++"
  CONFIGURE_OPTS="Joystick $CONFIGURE_OPTS"
fi

# check for libpcap and libnet
check_lib "libpcap.a"
if [ "$?" = "0" ]; then
  check_lib "libnet.a"
  if [ "$?" = "0" ]; then
    CONFIGURE_FLAGS="--enable-ethernet $CONFIGURE_FLAGS"
    CONFIGURE_OPTS="Ethernet $CONFIGURE_OPTS"
    echo "+++ With Ethernet Support +++"
  fi
fi

# check for ffmpeg and lame
#check_lib "libavcodec.dylib"
#if [ "$?" = "0" ]; then
#  check_lib "libmp3lame.dylib"
#  if [ "$?" = "0" ]; then
#    CONFIGURE_FLAGS="--enable-ffmpeg $CONFIGURE_FLAGS"
#    CONFIGURE_OPTS="FFMPEG $CONFIGURE_OPTS"
#    echo "+++ With FFMPEG + Lame Support +++"
#  fi
#fi

# ----- setup build dir -----
BUILD_DIR="$BUILD_DIR/$UI_TYPE-$SDK_VERSION"
if [ ! -d "$BUILD_DIR" ]; then
  mkdir -p "$BUILD_DIR"
fi
if [ ! -d "$BUILD_DIR" ]; then
  echo "ERROR: Creating build directory '$BUILD_DIR' failed!"
  exit 1
fi
if [ "$ARCH" = "ub" ]; then
  mkdir -p "$BUILD_DIR/ppc"
  mkdir -p "$BUILD_DIR/i386"
elif [ "$ARCH" = "i386" ]; then
  mkdir -p "$BUILD_DIR/i386"
else
  mkdir -p "$BUILD_DIR/ppc"
fi

# ----- Copy Gtk Config Files -----
if [ "$UI_TYPE" = "gtk" ]; then
  echo "----- Copy GTK Config Files -----"
  ANY_ARCH="$ARCH"
  if [ "$ANY_ARCH" = "ub" ]; then
    ANY_ARCH="i386"
  fi
  # copy fontconfig 
  MYFONTCONFIG="$BUILD_DIR/$ARCH/etc/fonts"
  if [ ! -d "$MYFONTCONFIG" ]; then
    FONTCONFIG="$EXTLIB_DIR/$ANY_ARCH/etc/fonts"
    if [ ! -d "$FONTCONFIG" ]; then
      echo "FontConfig dir not found in '$FONTCONFIG'"
      exit 1
    fi
    mkdir -p "$MYFONTCONFIG"
    (cd "$FONTCONFIG" && tar cf - *) | (cd "$MYFONTCONFIG" && tar xvf -)
  fi
fi

# ----- Compile VICE -----
export PATH=/usr/X11R6/bin:$PATH
COMMON_CFLAGS="-O3"

# extra flags
if [ "$UI_TYPE" != "cocoa" ]; then
  LDFLAGS_EXTRA="-dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
fi

build_vice () {
  local BUILD_ARCH="$1"
  local BUILD_SDK="$2"
  local BUILD_SDK_VERSION="$3"
  local VICE_SRC="`pwd`"
  local BUILD_ARCH2="powerpc"
  if [ "$BUILD_ARCH" = "i386" ]; then
    BUILD_ARCH2="i386"
  fi

  echo "----- Bulding VICE ($BUILD_ARCH) in $BUILD_DIR/$BUILD_ARCH/src -----"

  # already here?
  if [ -f "$BUILD_DIR/$BUILD_ARCH/src/x64" ]; then
    echo "  hmm... VICE already built for $BUILD_ARCH?! skipping..."
    return
  fi

  # main VICE compile call
  pushd "$VICE_SRC"
  cd "$BUILD_DIR/$BUILD_ARCH"
  set -x
  env \
    PATH="$EXTLIB_DIR/$BUILD_ARCH/bin:$PATH" \
    CPPFLAGS="-I$EXTLIB_DIR/$BUILD_ARCH/include" \
    CFLAGS="$COMMON_CFLAGS" \
    OBJCFLAGS="$COMMON_CFLAGS" \
    LDFLAGS="-L$EXTLIB_DIR/$BUILD_ARCH/lib $LDFLAGS_EXTRA" \
    CC="gcc -arch $BUILD_ARCH -isysroot $BUILD_SDK -mmacosx-version-min=$BUILD_SDK_VERSION" \
    CXX="g++ -arch $BUILD_ARCH -isysroot $BUILD_SDK -mmacosx-version-min=$BUILD_SDK_VERSION" \
    LD="gcc -arch $BUILD_ARCH -isysroot $BUILD_SDK -mmacosx-version-min=$BUILD_SDK_VERSION" \
    $VICE_SRC/configure --host=$BUILD_ARCH2-apple-darwin $CONFIGURE_FLAGS \
      --x-includes=$BUILD_SDK/usr/X11R6/include --x-libraries=$BUILD_SDK/usr/X11R6/lib
  set +x
  make 2>&1 | tee build.log 
  echo "--- Warnings ---" 
  fgrep warning: build.log
  popd

  # check if all went well
  if [ ! -f "$BUILD_DIR/$BUILD_ARCH/src/x64" ]; then
    echo "FATAL: no x64 in $BUILD_DIR/$BUILD_ARCH/src found!"
    exit 1
  fi
}

copy_dylib_rec () {
  local FILE="$1"
  local GOTLIB="$2"
  
  # get external libs
  echo 1>&2 "  checking binary '$FILE' for dylibs"
  local EXTLIB="`otool -L \"$FILE\" | grep dylib | grep -v /usr | grep -v /System | grep -v "$FILE" | cut -f1 -d ' '`"
  for lib in $EXTLIB ; do
    echo "$GOTLIB" | grep -q "$lib"
    if [ $? = 1 ]; then
      #echo 1>&2 "     lib...$lib"
      GOTLIB="$lib $GOTLIB"
      GOTLIB="`copy_dylib_rec \"$lib\" \"$GOTLIB\"`"
    fi
  done
  echo "$GOTLIB"
}

# copy the required dynamic libs
copy_dylib () {
  local BUILD_ARCH="$1"

  # get external libs
  echo "-- copy dylibs for $BUILD_ARCH --"
  EXTLIB="`copy_dylib_rec \"$BUILD_DIR/$BUILD_ARCH/src/x64\"`"

  # make sure lib dir exists
  local LIBDIR="$BUILD_DIR/$BUILD_ARCH/lib"
  if [ ! -d "$LIBDIR" ]; then
    mkdir -p "$LIBDIR"
  fi
  
  # copy lib if not already there
  for lib in $EXTLIB ; do
    LIBNAME="`basename \"$lib\"`"
    if [ ! -e "$LIBDIR/$LIBNAME" ]; then
      echo "  copying required dylib '$LIBNAME'"
      cp "$lib" "$LIBDIR/$LIBNAME"
    else
      echo "  required dylib '$LIBNAME' already here"
    fi
  done
}

fix_ref () {
  local exe="$1"
  local base="`basename \"$exe\"`"
  local LIBS=`otool -L "$exe" | grep .dylib | grep -v /usr/lib | awk '{ print $1 }'`
  echo -n "  fixing lib ref in '$1'"
  for lib in $LIBS ; do
    baselib="`basename \"$lib\"`"
    newlib="@executable_path/$baselib"
    install_name_tool -change "$lib" "$newlib" "$exe"
    echo -n "."
  done
  cutbase="`basename \"$base\" .dylib`"
  if [ "$cutbase" != "$base" ]; then
    # adapt install name in lib
    libname="@executable_path/$base"
    install_name_tool -id "$libname" "$exe"
    echo -n "I"
  fi
  echo
}

# setup SDK paths
echo "----- Choosing SDKs -----"
SDK_BASE=/Developer/SDKs
case "$SDK_VERSION" in
10.3+4)
  PPC_SDK=$SDK_BASE/MacOSX10.3.9.sdk
  I386_SDK=$SDK_BASE/MacOSX10.4u.sdk
  PPC_SDK_VERSION=10.3
  I386_SDK_VERSION=10.4
  ;;
10.4)
  PPC_SDK=$SDK_BASE/MacOSX10.4u.sdk
  I386_SDK=$SDK_BASE/MacOSX10.4u.sdk
  PPC_SDK_VERSION=10.4
  I386_SDK_VERSION=10.4
  ;;
10.5)
  PPC_SDK=$SDK_BASE/MacOSX10.5.sdk
  I386_SDK=$SDK_BASE/MacOSX10.5.sdk
  PPC_SDK_VERSION=10.5
  I386_SDK_VERSION=10.5
  ;;
*)
  echo "Invalid SDK given: $SDK_VERSION"
  exit 1
  ;;
esac
echo "ppc:  $PPC_SDK_VERSION $PPC_SDK"
if [ ! -d "$PPC_SDK" ]; then
  echo "ERROR: ppc SDK not found!"
  exit 1
fi
echo "i386: $I386_SDK_VERSION $I386_SDK"
if [ ! -d "$I386_SDK" ]; then
  echo "ERROR: i386 SDK not found!"
  exit 1
fi

# build vice
if [ "$ARCH" = "ub" ]; then
  build_vice i386 $I386_SDK $I386_SDK_VERSION
  build_vice ppc $PPC_SDK $PPC_SDK_VERSION
  copy_dylib i386
  copy_dylib ppc
elif [ "$ARCH" = "i386" ]; then
  build_vice i386 $I386_SDK $I386_SDK_VERSION
  copy_dylib i386
else
  build_vice ppc $PPC_SDK $PPC_SDK_VERSION
  copy_dylib ppc
fi

APPS="c1541 cartconv petcat x128 x64 x64dtv xcbm2 xpet xplus4 xvic"

# ----- Fixin Lib Refs -----
if [ "$UI_TYPE" = "cocoa" ]; then
  echo "----- Fixing Library References -----"
  # in all apps
  for app in $APPS ; do
    if [ "$ARCH" = "ub" ]; then
      fix_ref "$BUILD_DIR/i386/src/$app"
      fix_ref "$BUILD_DIR/ppc/src/$app"
    else
      fix_ref "$BUILD_DIR/$ARCH/src/$app"
    fi
  done
  # and all libs
  for lib in $EXTLIB ; do
    LIBNAME="`basename \"$lib\"`"
    if [ "$ARCH" = "ub" ]; then
      fix_ref "$BUILD_DIR/i386/lib/$LIBNAME"
      fix_ref "$BUILD_DIR/ppc/lib/$LIBNAME"
    else
      fix_ref "$BUILD_DIR/$ARCH/lib/$LIBNAME"
    fi
  done
fi

# ----- Create Universal Binary -----
if [ "$ARCH" = "ub" ]; then
  echo "----- Combining Binaries into Universal Binaries -----"
  mkdir -p "$BUILD_DIR/ub/src"  
  if [ -f "$BUILD_DIR/ub/src/x64" ]; then
    echo "  hmm... Already combined?! skipping..."
  else
    for app in $APPS ; do
      echo "  combining '$app'"
      lipo -create -output "$BUILD_DIR/ub/src/$app" \
        "$BUILD_DIR/ppc/src/$app" "$BUILD_DIR/i386/src/$app"
    done
    if [ ! -f "$BUILD_DIR/ub/src/x64" ]; then
      echo "FATAL: no universal x64 found!"
      exit 1
    fi
  fi
  
  # combine dynamic libs
  mkdir -p "$BUILD_DIR/ub/lib"
  for lib in $EXTLIB ; do
    LIBNAME="`basename \"$lib\"`"
    if [ ! -e "$BUILD_DIR/ub/lib/$LIBNAME" ]; then
      echo "  combining dylib '$LIBNAME'"
      lipo -create -output "$BUILD_DIR/ub/lib/$LIBNAME" \
        "$BUILD_DIR/ppc/lib/$LIBNAME" "$BUILD_DIR/i386/lib/$LIBNAME"
    else
      echo "  already combined dylib '$LIBNAME'"
    fi
  done
fi

# ----- Bundle Applications and Distribution -----
echo "----- Distribution Creation -----"
VICE_SRC="`pwd`"
ZIP="zip"
if [ "$DIST_TYPE" = "dir" ]; then
  ZIP="nozip"
fi
(cd "$BUILD_DIR/$ARCH" && \
$SHELL $VICE_SRC/src/arch/unix/macosx/make-bindist.sh $VICE_SRC strip $VICE_VERSION $ZIP $UI_TYPE $SDK_VERSION)

echo "----- Ready: architecture: $ARCH, ui-type: $UI_TYPE, dist-type: $DIST_TYPE -----"
echo "VICE was configured with: $CONFIGURE_OPTS"
exit 0
