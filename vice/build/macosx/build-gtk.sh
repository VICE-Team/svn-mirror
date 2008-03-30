#!/bin/bash
#
# ----- build-gtk.sh -----
# build the gtk+ toolkit on macs for the gtk+ version of VICE
#
# Usage:     build-mac-gtk.sh <build-dir> <arch:ppc|i386>
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#
# call this function twice: one for each architecture

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"
. "$SCRIPT_DIR/build-inc.sh"

# parse args
parse_args "$1" "$2"

# create target dirs
make_dirs bin lib include man share

# setup compiler environment
set_compiler_env

echo "===== gtk+ build for $ARCH ====="

# ----- Tool Libs -----
# gettext
configure_make_install gettext-0.16.tar.gz gettext-0.16 lib/libintl.a install \
                       "http://www.gnu.org/software/gettext/" \
                       "--disable-java --disable-csharp --disable-libasprintf"

# pkg-config
configure_make_install pkg-config-0.21.tar.gz pkg-config-0.21 bin/pkg-config install \
                       "http://pkgconfig.freedesktop.org/wiki/"

# jpeg
configure_make_install jpegsrc.v6b.tar.gz jpeg-6b lib/libjpeg.a install-lib \
                       "http://www.ijg.org/files/"
# png
configure_make_install libpng-1.0.20.tar.bz2 libpng-1.0.20 lib/libpng10.a install \
                       "http://www.libpng.org/pub/png/libpng.html" \
                       "--disable-shared"
# tiff
configure_make_install tiff-3.8.2.tar.gz tiff-3.8.2 lib/libtiff.a install \
                       "http://www.libtiff.org/" \
                       "--disable-shared --disable-cxx"

# freetype
configure_make_install freetype-2.1.10.tar.bz2 freetype-2.1.10 lib/libfreetype.a install \
                       "http://www.freetype.org/"
# libxml2
configure_make_install libxml2-2.6.27.tar.gz libxml2-2.6.27 lib/libxml2.a install \
                       "http://xmlsoft.org/" \
                       "--without-python"
# fontconfig
configure_make_install fontconfig-2.4.1.tar.gz fontconfig-2.4.1 lib/libfontconfig.a install \
                       "http://www.fontconfig.org/wiki/"

# ----- Gtk+ Libs -----
# glib
configure_make_install glib-2.12.4.tar.bz2 glib-2.12.4 lib/libglib-2.0.dylib install \
                       "http://www.gtk.org/"
# cairo
configure_make_install cairo-1.2.6.tar.gz cairo-1.2.6 lib/libcairo.dylib install \
                       "http://cairographics.org/"
# pango
configure_make_install pango-1.14.7.tar.bz2 pango-1.14.7 lib/libpango-1.0.dylib install \
                       "http://www.gtk.org/" \
                       "--with-included-modules=yes --with-dynamic-modules=no"
# atk
configure_make_install atk-1.9.0.tar.bz2 atk-1.9.0 lib/libatk-1.0.dylib install \
                       "http://www.gtk.org/"
# gtk+
export ac_cv_path_CUPS_CONFIG=no # cups does not cross-compile
configure_make_install gtk+-2.10.6.tar.gz gtk+-2.10.6 lib/libgtk-x11-2.0.dylib install \
                       "http://www.gtk.org/" \
                       "--disable-modules --with-included-loaders=yes"

echo "===== gtk+ ready for $ARCH ====="

                       