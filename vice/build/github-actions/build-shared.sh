#!/usr/bin/env bash
#
# Shared functions and variables for the Github Actions


# Shared command line options
ARGS="
    --disable-arch \
    --disable-pdf-docs \
    --enable-catweasel \
    --enable-cpuhistory \
    --enable-ethernet \
    --enable-midi \
    --enable-parsid \
    --with-flac \
    --with-gif \
    --with-lame \
    --with-libcurl \
    --with-libieee1284 \
    --with-mpg123 \
    --with-png \
    --with-portaudio \
    --with-unzip-bin \
    --with-vorbis \
    "

# Set configure options, check UI, set OTHER_UI
#
# params:   $1  UI identifier (GTK3, SDL2, SDL1)
#
set_configure_options()
{
    case "$1" in
        GTK3)
            ARGS="--enable-gtk3ui $ARGS"
            OTHER_UI="SDL2"
            ;;
        SDL1)
            ARGS="--enable-sdl1ui $ARGS"
            OTHER_UI="GTK3"
            ;;
        SDL2)
            ARGS="--enable-sdl2ui $ARGS"
            OTHER_UI="GTK3"
            ;;
        *)
            echo "Bad UI: $1"
            exit 1
            ;;
    esac

    # add option checking
    ARGS="--enable-option-checking=fatal $ARGS"
}
