#!/bin/bash
# Requires bash 4.x or later
#
# This script expects to be run inside the svn-mirror/ directory
#
#   Usage:  build-deb.sh <UI> ['release']
#
# When the string 'release' is given as the second argument a point release
# package will be built. Currently this just means leaving out the SVN revision
# in the package filename and the "Version" field in the control file.
#
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

# Point release flag
RELEASE=0

# Check command line for UI argument
if [ $# -lt 1 ]; then
    echo "$(basename $0): error: please specify UI to build: 'gtk3', 'sdl2' or 'headless'."
    exit 1
fi
case "$1" in
    [gG][tT][kK]3)
        UI="gtk3"
        ;;
    [sS][dD][lL]1)
        UI="sdl1"
        ;;
    [sS][dD][lL]2)
        UI="sdl2"
        ;;
    [Hh]eadless)
        UI="headless"
        ;;
    *)
        echo "$(basename $0): Unknown UI: $1, aborting."
        exit 1
        ;;
esac
if [ "$2" = "release" ]; then
    RELEASE=1
fi

# Get SVN revision and VICE version string
if [ $RELEASE -eq 0 ]; then
    SVN_REVISION=$(echo ${GITHUB_REF} | sed 's/.*\///')
fi
VICE_VERSION=$(grep '\<VERSION' vice/src/config.h | sed -n 's/^.*"\(.*\)".*$/\1/p')

# Create directory for the deb package and subdirectories inside the deb
if [ $RELEASE -eq 0 ]; then
    DEB_DIR="${UI}vice_${VICE_VERSION}+${SVN_REVISION}"
else
    DEB_DIR="${UI}vice_${VICE_VERSION}"
fi
mkdir -p ${DEB_DIR}/DEBIAN
mkdir -p ${DEB_DIR}/usr/share/doc/vice
if [ "$UI" = "gtk3" ]; then
    mkdir -p ${DEB_DIR}/usr/share/applications
    mkdir -p ${DEB_DIR}/usr/share/icons/hicolor/scalable/apps
fi

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

# Create copyright file, taking contributor info from src/infocontrib.h
cat vice/build/debian/copyright.header > ${DEB_DIR}/usr/share/doc/vice/copyright
cat vice/src/infocontrib.h | vice/build/github-actions/contributors.awk \
    >> ${DEB_DIR}/usr/share/doc/vice/copyright
cat vice/build/debian/copyright.footer >> ${DEB_DIR}/usr/share/doc/vice/copyright

# Get total size of installed files in KiB
INSTALLED_SIZE=$(du -ks ${DEB_DIR} | cut -f 1)

# Copy control file, setting the correct package version and installed size
if [ $RELEASE -eq 0 ]; then
    cat vice/build/debian/control.${UI} | \
        sed "s/__VICE_VERSION__/${VICE_VERSION}/ ; s/__SVN_REVISION__/${SVN_REVISION}/ ; s/__INSTALLED_SIZE__/${INSTALLED_SIZE}/" \
        > ${DEB_DIR}/DEBIAN/control
else
    # strip out the '+${SVNREVISION}' in point releases
    cat vice/build/debian/control.${UI} | \
        sed "s/__VICE_VERSION__/${VICE_VERSION}/ ; s/+__SVN_REVISION__// ; s/__INSTALLED_SIZE__/${INSTALLED_SIZE}/" \
        > ${DEB_DIR}/DEBIAN/control
fi

# Now build the .deb
fakeroot dpkg-deb -Zxz --build ${DEB_DIR}

# Export some variables to the runner for the upload step
echo "deb_name=${DEB_DIR}.deb" >> $GITHUB_OUTPUT
echo "deb_path=$(realpath ${DEB_DIR}.deb)" >> $GITHUB_OUTPUT
