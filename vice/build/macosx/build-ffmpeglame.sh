#!/bin/bash
#
# ----- build-ffmpeglame.sh -----
# build the ffmpeg and mp3lame on macs for VICE
#
# Usage:     build-ffmpeglame.sh <build-dir> <arch:ppc|i386>
#
# written by Christian Vogelgsang <chris@vogelgsang.org>

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"
. "$SCRIPT_DIR/build-inc.sh"

# parse args
parse_args "$@"

# setup compiler environment
set_compiler_env

echo "===== ffmpeg+lame build $BUILD_TAG ====="

configure_make_install lame-3.97.tar.gz lame-3.97 lib/libmp3lame.dylib install \
                       "http://lame.sourceforge.net/index.php" \
                       "--disable-static"
                       
PATCH="touch SDL.h SDL_thread.h ; \
cd ../ffmpeg-svn/ && cp configure c.tmp && sed -e 's/cc=\"cc\"//' < c.tmp > configure"
configure_make_install "" ffmpeg-svn lib/libavcodec.dylib "install-libs install-headers" \
                       "svn checkout svn://svn.mplayerhq.hu/ffmpeg/trunk ffmpeg-svn" \
                       "--disable-ffmpeg --disable-ffserver --disable-ffplay \
                        --disable-static --enable-shared --enable-libmp3lame \
                        --cc=\"$GCC\" \
                        \"--extra-cflags=$CPPFLAGS $COMPILE_TAG\" \"--extra-ldflags=$LDFLAGS $COMPILE_TAG\" \
                        --arch=$ARCH --disable-altivec --disable-mmx" 

echo "===== ffmpeg+lame ready $BUILD_TAG ====="

