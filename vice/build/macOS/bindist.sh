#!/bin/bash
set -o errexit

function err {
    >&2 echo -e $@
    >&2 echo
    exit 1
}

if [ $# != 1 ]; then
    err "Usage:\n\t$(basename $0) <path to VICE source>"
fi

SOURCE="$1"

if [ ! -d "$SOURCE" ]; then
    err "$SOURCE is not a directory"
fi

if [ -z "$CODE_SIGN_ID" ]; then
    err "Missing env CODE_SIGN_ID. Set to something like 'Developer ID Application: <NAME> (<ID>)', try $ security find-identity -v -p codesigning"
fi
if [ -z "$APPLE_ID_EMAIL" ]; then
    err "Missing env APPLE_ID_EMAIL. Set to your apple ID email address"
fi
if [ -z "$NOTARISATION_PROVIDER" ]; then
    err "Missing env NOTARISATION_PROVIDER. Refer to https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution/customizing_the_notarization_workflow"
fi
if [ -z "$NOTARISATION_PASSWORD" ]; then
    err "Missing env NOTARISATION_PASSWORD. Refer to https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution/customizing_the_notarization_workflow"
fi

set -o nounset

cd "$SOURCE"
SOURCE="$(pwd)"

if [ ! -f configure ]; then
    ./autogen.sh
fi

BUILD_DIR=$(mktemp -d)
cd $BUILD_DIR
echo "Build Dir: $BUILD_DIR"

BUILD_FLAGS="\
    --with-jpeg \
    --with-png \
    --with-gif \
    --with-vorbis \
    --with-flac \
    --enable-ethernet \
    --enable-new8580filter \
    --enable-lame \
    --enable-midi \
    --enable-cpuhistory \
    --enable-external-ffmpeg \
    --enable-macos-target-sdk-version=11.0 \
    "

THREADS=$(sysctl -n hw.ncpu)

#
# Notarisation utility
#

function notarise {
    OUTPUT="$(mktemp)"
    xcrun altool --notarize-app \
                 --primary-bundle-id "$1" \
                 --username "$APPLE_ID_EMAIL" \
                 --password "$NOTARISATION_PASSWORD" \
                 --asc-provider "$NOTARISATION_PROVIDER" \
                 --file "$1" \
                 2>&1 | tee "$OUTPUT"
    UUID=$(awk '/RequestUUID/ { print $3 }' "$OUTPUT")

    if [ -z "$UUID" ]
    then
        echo "ERROR: Failed to capture RequestUUID from xcrun output".
        exit 1
    fi

    echo "Waiting for Status: success"

    while [ -z "$(grep "Status: success" "$OUTPUT")" ]
    do    
        sleep 10
        xcrun altool --notarization-info "$UUID" \
                     -u "$APPLE_ID_EMAIL" \
                     -p "$NOTARISATION_PASSWORD" \
                     2>&1 | tee "$OUTPUT"
    done

    xcrun stapler staple "$1"
}

#
# GTK3 build
#

mkdir gtk3
cd gtk3

"$SOURCE/configure" --enable-native-gtk3ui $BUILD_FLAGS

make -j $THREADS
DEPS_PREFIX=/opt/homebrew make bindistzip
notarise *.dmg
mv *.dmg "$SOURCE"
cd ..

#
# SDL2 build
#

mkdir sdl2
cd sdl2

"$SOURCE/configure" --enable-sdlui2 $BUILD_FLAGS

make -j $THREADS
DEPS_PREFIX=/opt/homebrew make bindistzip
notarise *.dmg
mv *.dmg "$SOURCE"
cd ..

echo
echo "Looks like it worked."

cd "$SOURCE"
rm -rf "$BUILD_DIR"
open .

