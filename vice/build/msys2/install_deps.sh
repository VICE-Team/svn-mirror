#!/bin/bash

pacman --sync --noconfirm --needed base-devel mingw-w64-x86_64-toolchain
pacboy --noconfirm --needed sync zip: subversion:
pacman --sync --noconfirm --needed  \
    mingw-w64-x86_64-pkg-config     \
    mingw-w64-x86_64-ntldd          \
    mingw-w64-x86_64-gtk3           \
    mingw-w64-x86_64-glew           \
    mingw-w64-x86_64-giflib         \
    mingw-w64-x86_64-lame           \
    mingw-w64-x86_64-libvorbis      \
    mingw-w64-x86_64-flac           \
    mingw-w64-x86_64-mpg123         \
    mingw-w64-x86_64-icoutils

XA_VERSION=2.3.11

if [ ! -e /usr/local/bin/xa65.exe ]
then
    cd /usr/local
    mkdir -p src
    cd src
    wget http://www.floodgap.com/retrotech/xa/dists/xa-${XA_VERSION}.tar.gz
    tar -xzf xa-${XA_VERSION}.tar.gz
    cd xa-${XA_VERSION}
    make mingw install
    cp /usr/local/bin/xa.exe /usr/local/bin/xa65.exe
fi
