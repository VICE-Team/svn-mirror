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
    OTHER_UI="SDL2"
    ;;

SDL2)
    ARGS="--enable-sdlui2 $ARGS"
    OTHER_UI="GTK3"
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
    OUTPUT="../gh-pages/analysis/$UI/$REVISION_STRING"
    mkdir "$OUTPUT"

    # Delete more than older reports so they don't grow indefinitely
    for report in $(ls "$OUTPUT/../" | grep '^r\d\+$' | sort -nr | sed '1,250d')
    do
        echo "Deleting old report $report"
        rm -rd "$OUTPUT/../$report"
    done

    ./autogen.sh
    scan-build ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
    scan-build -o "$OUTPUT" make -j $(sysctl -n hw.ncpu)

    # scan-build -o still creates a silly folder name
    mv $(dirname $(find "$OUTPUT/scan-build-"* -name index.html))/* "$OUTPUT/"
    rm -rf "$OUTPUT/scan-build-"*

    # Update the page title with SVN rev and link to the other UI
    TITLE="VICE $UI $REVISION_STRING Static Analysis Result"
    HEADING="$TITLE (<a href=\"../../$OTHER_UI/$REVISION_STRING\">Switch to $OTHER_UI</a>)"
    sed \
        -i '' \
        -e "s,<title>.*</title>,<title>$TITLE</title>," \
        -e "s,<h1>.*</h1>,<h1>$HEADING</h1>," \
        "$OUTPUT/index.html"

    # Now, generate the UI index page linking to each report
    cat << HEREDOC | sed 's/^        //' > "$OUTPUT/../index.html" 
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/water.css@2/out/water.css">
            <meta charset="utf-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>VICE $UI Static Analysis Results</title>
        </head>
        <body>
            <header>
                <h1>VICE $UI Static Analysis Results</h1>
            </header>
            <main>
                <p>
        $(
            for report in $(ls "$OUTPUT/../" | grep '^r\d\+$' | sort -nr)
            do
                echo "            <a href=\"$report\">$report &rarr;</a><br>"
            done 
        )
                </p>
            </main>
        </body>
        </html>
HEREDOC
    ;;
*)
    echo "Bad Build Type: $BUILD_TYPE"
    exit 1
    ;;
esac
