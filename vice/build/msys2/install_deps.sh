#!/bin/bash

pacman --sync --noconfirm --needed base-devel
pacboy --noconfirm --needed sync zip: subversion:
pacman --sync --noconfirm --needed  \
    mingw-w64-x86_64-pkg-config     \
    mingw-w64-x86_64-gcc            \
    mingw-w64-x86_64-ntldd          \
    mingw-w64-x86_64-gtk3           \
    mingw-w64-x86_64-glew           \
    mingw-w64-x86_64-giflib         \
    mingw-w64-x86_64-lame           \
    mingw-w64-x86_64-libvorbis      \
    mingw-w64-x86_64-flac           \
    mingw-w64-x86_64-mpg123         \
    mingw-w64-x86_64-icoutils
