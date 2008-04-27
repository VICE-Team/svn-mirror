# build-inc.sh
#
# include file for other build-* scripts
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#

parse_args () {
  # get arguments
  BASE_DIR="$1"
  ARCH="$2"
  if [ "$ARCH" = "" ]; then
    echo "Usage: $0 <build-dir> <arch:ppc|i386>"
    exit 1
  fi
  if [ "$BASE_DIR" = ""  ]; then
    echo "Please give base dir!"
    exit 1
  fi
  if [ ! -d "$BASE_DIR" ]; then
    echo "Missing base dir!"
    exit 1
  fi
  # normalize base dir
  BASE_DIR="`cd \"$BASE_DIR\" && pwd`"

  # check arguments
  if [ "$ARCH" = "ppc" ]; then
    INSTALL_DIR="$BASE_DIR/ppc"
  elif [ "$ARCH" = "i386" ]; then
    INSTALL_DIR="$BASE_DIR/i386"
  else
    echo "Unknown ARCH: $ARCH (ppc or i386)"
    exit 1
  fi

  # setup base dir
  if [ ! -d "$INSTALL_DIR" ]; then
    echo "  creating install dir $INSTALL_DIR"
    mkdir -p "$INSTALL_DIR"
  fi
}

# configure/compile/install a autoconf'ed distribution
#
# SRC         source archive
# DIR         source directory
# CHECK_FILE  if this file exists then assume build is already done
# INSTALL     rule to install
# URL         where to download archive if its missing
# CONFIG_OPT  extra switches for configure
#
configure_make_install () {
  SRC="$1"
  DIR="$2"
  CHECK_FILE="$3"
  INSTALL="$4"
  URL="$5"
  CONFIG_OPT="$6"

  echo "----- $DIR ($ARCH) -----"
  
  # check if lib is available
  if [ -e "$INSTALL_DIR/$CHECK_FILE" ]; then
    echo "  Already installed. ($CHECK_FILE available)"
  else
    
    # check for source archive
    if [ "x$SRC" = "x" ]; then
      if [ ! -d "$DIR" ]; then
        echo "FATAL: source dir '$DIR' is missing!"
        echo "       please setup first (from e.g. $URL)"
        exit 1
      fi
    else
      if [ ! -e "$SRC" ]; then
        echo "FATAL: source archive '$SRC' missing in curent directory!"
        echo "       please download first (from e.g. $URL)"
        exit 1
      fi
    fi
    
    # check if source is already unpacked
    if [ ! -d "$DIR" ]; then
      echo "  Unpacking source for $DIR"
      echo "$SRC" | grep .bz2 > /dev/null
      if [ $? != 0 ]; then 
        tar xfz "$SRC"
      else
        tar xfj "$SRC"
      fi
    fi
    if [ ! -d "$DIR" ]; then
      echo "FATAL: source not unpacked to $DIR"
      exit 1
    fi
    
    # prepare BUILD
    if [ "$COMPILE_IN_SOURCE" != "" ]; then
      BUILD_DIR="$DIR"
    else
      BUILD_DIR="BUILD"
      if [ -d "$BUILD_DIR" ]; then
        echo "FATAL: build directory '$BUILD_DIR' already here!"
        exit 1
      fi
      mkdir BUILD
    fi
    
    # patch source?
    if [ "$PATCH" != "" ]; then
      (cd "$DIR" && eval "$PATCH")
    fi
    
    # build
    echo "  configure options: $CONFIG_OPT"
    (cd "$BUILD_DIR" && eval "../$DIR/configure --prefix=\"$INSTALL_DIR\" $CONFIG_OPT $EXTRA_OPT")
    (cd "$BUILD_DIR" && make)
    if [ "$?" != "0" ]; then
      echo "FATAL: make failed!"
      exit 1
    fi
    echo "make $INSTALL"
    (cd "$BUILD_DIR" && make $INSTALL)
    
    # check for lib
    if [ ! -e "$INSTALL_DIR/$CHECK_FILE" ]; then
      echo "FATAL: $CHECK_FILE not found!"
      exit 1
    fi
    
    # clean up
    if [ "$COMPILE_IN_SOURCE" = "" ]; then
      rm -rf BUILD
    fi
    if [ "x$SRC" != "x" ]; then
      rm -rf "$DIR"
    fi
    
    echo "----- ready with $2 -----" 
  fi
}

# create directories in install dir
make_dirs () {
  for DIR in "$@" ; do
    if [ ! -d "$INSTALL_DIR/$DIR" ]; then
      echo "  creating directory $DIR"
      mkdir -p "$INSTALL_DIR/$DIR"
    fi
  done
}

set_compiler_env () {
  # setup SDK paths
  local PPC_SDK=/Developer/SDKs/MacOSX10.3.9.sdk
  local I386_SDK=/Developer/SDKs/MacOSX10.4u.sdk
  local PPC_SDK_VERSION=10.3
  local I386_SDK_VERSION=10.4
  
  # choose SDK
  if [ "$ARCH" = "ppc" ]; then
    SDK="$PPC_SDK"
    SDK_VERSION="$PPC_SDK_VERSION"
  else
    SDK="$I386_SDK"
    SDK_VERSION="$I386_SDK_VERSION"
  fi
    
  # set common flags
  export CPPFLAGS="-I$INSTALL_DIR/include"
  export LDFLAGS="-L$INSTALL_DIR/lib"
  export PATH="$INSTALL_DIR/bin:$PATH"

  # do cross-compile?
  #if [ "`uname -p`" != "$ARCH" ]; then
  #  EXTRA_OPT="--host=$ARCH-apple-darwin`uname -r`"
  #fi
  
  export COMPILE_TAG="-arch $ARCH -isysroot $SDK -mmacosx-version-min=$SDK_VERSION"
  export CC="gcc $COMPILE_TAG"
  export CXX="g++ $COMPILE_TAG"
  export LD="gcc $COMPILE_TAG"
}
