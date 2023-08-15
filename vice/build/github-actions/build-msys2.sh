#!/usr/bin/env bash
#
# Usage: build-msys2.sh <UI> [SVN rXXXXX override, or 'release']

set -o errexit
set -o nounset
source $(dirname $0)/build-shared.sh
cd "$(dirname $0)"/../..

# Set proper configure options
set_configure_options "$1"


# Skip autogen.sh when building release from tarball
if [ "$2" = "release" ]; then
    ./configure $ARGS || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
else
    ./autogen.sh
    ./configure $ARGS SVN_REVISION_OVERRIDE=$(echo "$2" | sed 's/^r//') || ( echo -e "\n**** CONFIGURE FAILED ****\n" ; cat config.log ; exit 1 )
fi

make -j $(( $NUMBER_OF_PROCESSORS )) -s
make bindistzip
make bindist7zip
