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
parse_args "$@"

# setup compiler environment
set_compiler_env

echo "===== pcaplibnet build $BUILD_TAG ====="

COMPILE_IN_SOURCE=1
configure_make_install libpcap-1.0.0.tar.gz libpcap-1.0.0 lib/libpcap.a install \
                       "http://www.tcpdump.org/"

PATCH="mv src/libnet_init.c src/libnet_init.c.org ; \
sed -e 's/getuid() && geteuid()/0/' < src/libnet_init.c.org > src/libnet_init.c ; \
mv src/libnet_build_gre.c src/libnet_build_gre.c.org ; \
sed -e 's/^inline//g' < src/libnet_build_gre.c.org > src/libnet_build_gre.c"
configure_make_install libnet-1.1.2.1.tar.gz libnet lib/libnet.a install \
                       "http://ftp.debian.org/debian/pool/main/libn/libnet0/libnet0_1.0.2a.orig.tar.gz"

echo "===== pcaplibnet $BUILD_TAG ====="
echo "(Read 'libpcap-1.0.0/README.macosx' to find out how to install pcap on Mac OS X correctly)"
