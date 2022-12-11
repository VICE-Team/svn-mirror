#!/bin/bash
# Requires bash 4.x or later
#
# This script expects to be run inside the svn-mirror/ directory
#
# Usage:    build-deb.sh <UI>
#
# TODO: Generate DEBIAN/copyright file
# TODO: Provide md5sums?


# Mapping of emulators to their icon names in data/common/
# There is no separate icon for xcbm5x0, so we use the xcbm2 icon
declare -A ICONS=(
    [vsid]="SID"
    [x128]="C128"
    [x64dtv]="DTV"
    [x64sc]="C64"
    [xcbm2]="CBM2"
    [xpet]="PET"
    [xplus4]="Plus4"
    [xscpu64]="SCPU"
    [xvic]="VIC20"
)

# Check command line for UI argument
if [ $# -ne 1 ]; then
    echo "$(basename $0): Please specify UI to build: 'gtk3' or 'sdl2', aborting."
    exit 1
fi
case "$1" in
    [gG][tT][kK]3)
        UI="gtk3"
        ;;
    [sS][dD][lL]2)
        UI="sdl2"
        ;;
    *)
        echo "$(basename $0): Unknown UI: $1, aborting."
        exit 1
        ;;
esac

# Get SVN revision and VICE version string
SVN_REVISION=$(echo ${GITHUB_REF} | sed 's/.*\///')
VICE_VERSION=$(grep '\<VERSION' vice/src/config.h | sed -n 's/^.*"\(.*\)".*$/\1/p')

# Create directory for the deb package and subdirectories inside the deb
DEB_DIR="${UI}vice_${VICE_VERSION}_${SVN_REVISION}"
mkdir -p ${DEB_DIR}/DEBIAN
mkdir -p ${DEB_DIR}/usr/share/doc/vice
if [ "$UI" = "gtk3" ]; then
    mkdir -p ${DEB_DIR}/usr/share/applications
    mkdir -p ${DEB_DIR}/usr/share/icons/hicolor/scalable/apps
fi

# Copy control file, setting the correct VICE version in the "Version:" field
cat vice/build/debian/control.${UI} | \
    sed "s/__VICE_VERSION__/${VICE_VERSION}/" \
    > ${DEB_DIR}/DEBIAN/control
# Create copyright file, taking contributor info from src/infocontrib.h
cat vice/build/debian/copyright.header > ${DEB_DIR}/DEBIAN/copyright
cat vice/src/infocontrib.h | vice/build/github-actions/contributors.awk \
    >> ${DEB_DIR}/DEBIAN/copyright

# Copy the files installed with `make install-strip`
cp -R ${HOME}/build/* ${DEB_DIR}/
# Copy the documentation
cp vice/doc/vice.pdf ${DEB_DIR}/usr/share/doc/vice/
# Copy .desktop files
if [ "$UI" = "gtk3" ]; then
    cp vice/build/debian/*.desktop ${DEB_DIR}/usr/share/applications
fi
# Copy icon files
if [ "$UI" = "gtk3" ]; then
    for emu in ${!ICONS[@]}
    do
        orig="vice/data/common/${ICONS[$emu]}_1024.svg"
        dest="${DEB_DIR}/usr/share/icons/hicolor/scalable/apps/${emu}.svg"
        cp ${orig} ${dest}
    done
fi

# Now build the .deb
fakeroot dpkg-deb --build ${DEB_DIR}

# Export some variables to the runner for the upload step
echo "deb_name=${DEB_DIR}.deb" >> $GITHUB_OUTPUT
echo "deb_path=$(realpath ${DEB_DIR}.deb)" >> $GITHUB_OUTPUT
