#!/bin/bash
#
# ----- build-pcaplibnet.sh -----
# build the pcaplibnet on macs for VICE ethernet (TFE/RRNET) support
#
# Usage:     build-pcaplibnet.sh <build-dir> <arch:ppc|i386>
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"
. "$SCRIPT_DIR/build-inc.sh"

# parse args
parse_args "$1" "$2"

# setup compiler environment
set_compiler_env

echo "===== pcaplibnet build for $ARCH ====="

configure_make_install libpcap-0.9.5.tar.gz libpcap-0.9.5 lib/libpcap.a install \
                       "http://www.tcpdump.org/"

# need to patch libnet
PATCH="cp /usr/share/libtool/config.guess . ; cp /usr/share/libtool/config.sub . ; \
cp include/libnet/libnet-headers.h tmp ; \
sed -e 's/#if (__linux__)/#if (__linux__) || (defined(__APPLE__)\&\&defined(__i386__))/' < tmp > include/libnet/libnet-headers.h ; \
cp Makefile.in tmp ;
sed -e '/^.*\$(INSTALL_DATA) \$(MAN).*$/d' < tmp > Makefile.in"
COMPILE_IN_SOURCE=1

configure_make_install libnet0_1.0.2a.orig.tar.gz libnet-1.0.2a.orig lib/libnet.a install \
                       "http://ftp.debian.org/debian/pool/main/libn/libnet0/libnet0_1.0.2a.orig.tar.gz"

echo "===== pcaplibnet ready for $ARCH ====="
echo "(Read 'libpcap-0.9.5/README.macosx' to find out how to install pcap on Mac OS X correctly)"
