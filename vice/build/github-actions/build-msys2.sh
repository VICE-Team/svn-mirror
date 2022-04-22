#!/usr/bin/env bash
#
# Usage: build-msys2.sh <UI> [SVN rXXXXX override, or 'release']

set -o errexit
set -o nounset
cd "$(dirname $0)"/../..

#
# Build and install xa65. When this stops working, check
# https://www.floodgap.com/retrotech/xa/dists/ to see why
#

if [ ! -e /usr/local/bin/xa65.exe ]
then
    pushd /usr/local
    mkdir -p src
    cd src
    XA_VERSION=$(wget --tries=1 -O - https://www.floodgap.com/retrotech/xa/dists/ 2>/dev/null | grep '"xa-[^"]*gz"' | sed -e 's,.*xa-,,' -e 's,.tar.gz.*,,' | sort -V | tail -n1)
    echo "got XA version: " $XA_VERSION
    if [ x"$XA_VERSION"x = x""x ]; then
        echo "WARNING: using backup location for XA"
        wget https://vice-emu.sourceforge.io/xa/xa65-xa-2.3.11.tar.gz
        tar -xzf xa65-xa-2.3.11.tar.gz
        cd xa65-xa-2.3.11/xa
    else
        wget https://www.floodgap.com/retrotech/xa/dists/xa-${XA_VERSION}.tar.gz
        tar -xzf xa-${XA_VERSION}.tar.gz
        cd xa-${XA_VERSION}
    fi
    make mingw install
    cp /usr/local/bin/xa.exe /usr/local/bin/xa65.exe
    popd
fi

ARGS="
    --disable-arch \
    --disable-pdf-docs \
    --with-png \
    --with-gif \
    --with-vorbis \
    --with-flac \
    --enable-ethernet \
    --enable-lame \
    --enable-midi \
    --enable-cpuhistory \
    --enable-ffmpeg \
    "

case "$1" in
GTK3)
    ARGS="--enable-gtk3ui $ARGS"
    ;;

SDL2)
    ARGS="--enable-sdlui2 $ARGS"
    ;;

*)
    echo "Bad UI: $1"
    exit 1
    ;;
esac

# Skip autogen.sh when building release from tarball
if [ "$2" = "release" ]; then
    ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
else
    ./autogen.sh
    ./configure $ARGS SVN_REVISION_OVERRIDE=$(echo "$2" | sed 's/^r//') || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
fi

make -j $(( $NUMBER_OF_PROCESSORS )) -s
make bindistzip
make bindist7zip
