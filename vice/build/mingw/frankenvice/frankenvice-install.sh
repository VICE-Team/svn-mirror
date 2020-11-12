#!/bin/bash
# vim: et ts=4 sw=4 sts=4 fdm=marker:
#
# Frankenvice install script
#
# @author   Bas Wassink <b.wassink@ziggo.nl>
#
# FIXME:    Before this script can be run, there's a few issues to be fixed,
#           and I'm not sure these should be handled by this script:
#
#           - Fix the alien Perl code to handle the latests RPMs.
#           - Fix the pkg-config wrapper so it won't bitch about installing
#             dpkg-dev (which won't help at all, still an issue on Debian 10.3)
#           - Create symlinks to glib tools
#           - Create gschema files, these do not get generated for some reason,
#             so manual labour is required:
#             $ su -
#             $ cd /usr/x86_64-w64-mingw32/share/glib-2.0/schemas
#             $ glib-compile-schemas .
#             (this requires proper symlinks to the glib tools)



# User directory to do the work
FRANKENVICE_ROOT="$HOME/frankenvice"
# Downloads directory from the RPM packages, and perhaps others
FRANKENVICE_DOWNLOADS="$FRANKENVICE_ROOT/downloads"
# DEB packages dir
FRANKENVICE_DEB="$FRANKENVICE_ROOT/deb"

RPM_REPO="https://rpmfind.net/linux/fedora/linux/development/rawhide/Everything/x86_64/os/Packages/m"
RPM_PACKAGES="rpm-packages.txt"

# command line args
ARG_SKIP_DOWNLOAD="no"
ARG_SKIP_CONVERSION="no"
ARG_SKIP_INSTALL="no"



# Check if we are root
#
# @return   0 if root, 1 otherwise
check_root()
{
    if [ "$EUID" -ne 0 ]; then
        return 1
    fi
    return 0
}


# Display help text
display_help()
{
    echo "Usage `basename $0` [options]"
    cat <<"EOF"
Install the packages required to create a Gtk3 Windows cross-compiler on a
Debian Linux box by using Fedora mingw64 packages which Debian is missing.

This script needs to run as a normal user.

Command line options:

    -h/--help           show this text and exit
    --clean             delete all .RPM and .DEB packages before downloading,
                        converting and installing packages.

    The following options are meant for debugging, or when for some reason the
    previous step failed:

    --skip-download     skip downloading RPMs
    --skip-conversion   skip converting RPMs to DEBs
    --skip-install      skip-installing DEBs
EOF
    exit 0
}


# Set up directories
#
# Create directories in the normal user's HOME dir to use for this script.
#
setup_directories()
{
    if [ ! -e "$FRANKENVICE_ROOT" ]; then
        echo ".. creating frankenvice root dir:     $FRANKENVICE_ROOT"
        mkdir "$FRANKENVICE_ROOT"
    fi
    if [ ! -e "$FRANKENVICE_DOWNLOADS" ]; then
        echo ".. creating frankenvice download dir: $FRANKENVICE_DOWNLOADS"
        mkdir "$FRANKENVICE_DOWNLOADS"
    fi
    if [ ! -e "$FRANKENVICE_DEB" ]; then
        echo ".. creating frankenvice deb dir:      $FRANKENVICE_DEB"
        mkdir "$FRANKENVICE_DEB"
    fi
}

# Download RPM's required for mingw64 Gtk3 cross-compiling
#
# Note: downloads a few too many packages, specifically the C++ versions of
#       some packages we use.
#
download_rpms()
{
    packages="`dirname $0`/$RPM_PACKAGES"
    for f in `cat "$packages"`; do
        echo ".. downloading '$f'"
        wget -r -l1 -q --no-parent --no-directories -A "${f}*.rpm" \
            -R "*-static-*" -R "*-tools-*" \
            --directory-prefix="$FRANKENVICE_DOWNLOADS" "$RPM_REPO"

    done
}


# Remove unwanted RPM packages (C++ etc)
#
remove_unwanted_packages()
{
    echo ".. removing C++ packages:"
    rm -f "$FRANKENVICE_DOWNLOADS/mingw64-*mm-*.rpm"
    echo ".. OK."
}



# Convert RPMs to DEBs
#
# Try to convert the RPM files to DEB files using the vice-rpm-to-deb script.
#
convert_rpms()
{
    old_dir=`pwd`
    cd "$FRANKENVICE_DEB"

    for f in `ls "$FRANKENVICE_DOWNLOADS"`; do
        echo "..converting $f"
        # this is messed up:
        sudo $HOME/`dirname $0`/vice-rpm-to-deb.sh "$FRANKENVICE_DOWNLOADS/$f" 2>/dev/null
    done

    cd "$old_dir"
}


# Install DEBs
#
# Try to install the converted DEBs with dpkg
#
install_debs()
{
    old_dir=`pwd`
    cd "$FRANKENVICE_DEB"

    for f in *.deb; do
        echo ".. installing $f:"
        sudo dpkg -i "$f"
    done
}


# Delete all .deb and .rpm files
clean_files()
{
    rm -f $FRANKENVICE_DEB/*
    rm -f $FRANKENVICE_DOWNLOADS/*
}


# Entry point
#
check_root
if [ "$?" -eq "0" ]; then
    echo "This script needs to be started as a normal user, aborting."
    exit 1
fi
# echo "OK, normal user"


# parse command line args
while [ "$#" -gt 0 ]; do
    case "$1" in
        -h|--help) display_help;;
        --clean) shift; clean_files;;
        --skip-download) shift; ARG_SKIP_DOWNLOAD="yes";;
        --skip-conversion) shift; ARG_SKIP_CONVERSION="yes";;
        --skip-install) shift; ARG_SKIP_INSTALL="yes";;
        -*) echo "Unknown option $1, aborting"; exit 1;;
    esac
done

echo "skip downloads  = $ARG_SKIP_DOWNLOAD"
echo "skip conversion = $ARG_SKIP_CONVERSION"
echo "skip install    = $ARG_SKIP_INSTALL"


echo "Setting up directories:"
setup_directories
echo "OK."


if [ "$ARG_SKIP_DOWNLOAD" = "no" ]; then

    echo "FIXME: Wiping downloads to avoid downloading and installing multiple versions of"
    echo "the same package. Perhaps add a --update switch or so?"
    rm -rfd "$FRANKENVICE_DOWNLOADS/*"
    echo "Downloading mingw64 RPM packages."
    download_rpms
    echo "Removing unwanted packages."
    remove_unwanted_packages
fi

if [ "$ARG_SKIP_CONVERSION" = "no" ]; then
    echo "Converting RPMs to DEB:"
    convert_rpms
fi

if [ "$ARG_SKIP_INSTALL" = "no" ]; then
    echo "Installing DEBs:"
    install_debs
fi
