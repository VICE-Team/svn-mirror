#!/usr/bin/env bash
set -o errexit
set -o nounset
cd "$(dirname $0)"/../..

BUILD_TYPE="$1"
UI="$2"
REVISION_STRING="$3"

ARGS="\
    --disable-arch \
    --disable-pdf-docs \
    --enable-ethernet \
    --with-jpeg \
    --with-png \
    --with-gif \
    --with-vorbis \
    --with-flac \
    --enable-lame \
    --enable-midi \
    --enable-cpuhistory \
    --enable-external-ffmpeg \
    "

case "$UI" in
GTK3)
    ARGS="--enable-native-gtk3ui $ARGS"
    ;;

SDL2)
    ARGS="--enable-sdlui2 $ARGS"
    ;;

*)
    echo "Bad UI: $UI"
    exit 1
    ;;
esac

case "$BUILD_TYPE" in
bindist)
    ./autogen.sh
    ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
    make -j $(sysctl -n hw.ncpu) -s
    make bindistzip
    ;;

analyse)
    rm -rf ../gh-pages/analysis/$UI/*

    OUTPUT=../gh-pages/analysis/$UI/$REVISION_STRING
    mkdir $OUTPUT

    #  ./autogen.sh
    #  scan-build ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
    #  scan-build -o "$OUTPUT" make -j $(sysctl -n hw.ncpu)
    
    echo "$UI/$REVISION_STRING" > "$OUTPUT/index.html"

    # scan-build -o still creates a silly folder name
    #  mv $(dirname $(find "$OUTPUT/scan-build-"* -name index.html))/* "$OUTPUT/"
    #  rm -rf "$OUTPUT/scan-build-"*

    # Inject the revision number into the page
    sed \
        -i '' \
        -e "s,</title>, ($REVISION_STRING)</title>," \
        -e "s,</h1>, ($REVISION_STRING)</h1>," \
        "$OUTPUT/index.html"

    # TODO: delete more than x older reports so they don't grow indefinitely

    # Now, generate the UI index page linking to each report
    cat << "    HEREDOC" | sed 's/^        //' > "$OUTPUT/../index.html" 
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <link rel="stylesheet" href="https://unpkg.com/mvp.css">
            <meta charset="utf-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>VICE $UI Static Analysis Results</title>
        </head>
        <body>
            <header>
                <h1>VICE $UI Static Analysis Results</h1>
                <p>
                $(
                    for report in $(ls "$OUTPUT/../r"* | sort -vr)
                    do
                        echo "            <a href="$report"><b>$report &rarr;</b></a><br>"
                    done 
                )
                </p>
            </header>
        </body>
        </html>
    HEREDOC
    ;;
*)
    echo "Bad Build Type: $BUILD_TYPE"
    exit 1
    ;;
esac
