#!/usr/bin/env bash
set -o errexit
set -o nounset
cd "$(dirname $0)"/../..

UI="$1"
BUILD_TYPE="$2"

ARGS="\
    --disable-arch \
    --disable-pdf-docs \
    --enable-ethernet \
    --with-jpeg \
    --with-png \
    --with-gif \
    --with-vorbis \
    --with-flac \
    --enable-lame \
    --enable-midi \
    --enable-cpuhistory \
    --enable-external-ffmpeg \
    "

case "$UI" in
GTK3)
    ARGS="--enable-native-gtk3ui $ARGS"
    ;;

SDL2)
    ARGS="--enable-sdlui2 $ARGS"
    ;;

*)
    echo "Bad UI: $UI"
    exit 1
    ;;
esac

case "$BUILD_TYPE" in
bindist)
    ./autogen.sh
    ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
    make -j $(sysctl -n hw.ncpu) -s
    make bindistzip
    ;;

analyse)
    OUTPUT="../gh-pages/analysis/$UI"
    rm -rf $OUTPUT/*

    ./autogen.sh
    scan-build ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
    scan-build -o $OUTPUT make -j $(sysctl -n hw.ncpu)

    # scan-build -o still creates a silly folder name
    mv $(dirname $(find $OUTPUT/scan-build-* -name index.html))/* $OUTPUT/
    rm -rf $OUTPUT/scan-build-*
    ;;

*)
    echo "Bad Build Type: $BUILD_TYPE"
    exit 1
    ;;
esac
