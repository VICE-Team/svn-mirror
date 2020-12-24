#!/usr/bin/env bash
set -o errexit
cd "$(dirname $0)"/../..

#
# Build and install xa65. When this stops working, check
# https://www.floodgap.com/retrotech/xa/dists/ for a newer version.
#

XA_VERSION=2.3.11

if [ ! -e /usr/local/bin/xa65.exe ]
then
    pushd /usr/local
    mkdir -p src
    cd src
    wget https://www.floodgap.com/retrotech/xa/dists/xa-${XA_VERSION}.tar.gz
    tar -xzf xa-${XA_VERSION}.tar.gz
    cd xa-${XA_VERSION}
    make mingw install
    cp /usr/local/bin/xa.exe /usr/local/bin/xa65.exe
    popd
fi

./autogen.sh
./configure \
    --disable-arch \
    --disable-pdf-docs \
    --enable-ethernet \
    --with-jpeg \
    --with-png \
    --with-gif \
    --enable-lame \
    --enable-midi \
    --enable-cpuhistory \
    || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
make -j $(( $NUMBER_OF_PROCESSORS * 2 )) -s
make bindistzip
