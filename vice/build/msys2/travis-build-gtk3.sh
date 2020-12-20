#!/bin/bash
set -o errexit

cd "$(dirname $0)"/../..
./autogen.sh
./configure --disable-arch --disable-pdf-docs || ( cat config.log && false )
make -j -s
make bindistzip