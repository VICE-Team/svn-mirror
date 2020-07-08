#!/bin/bash

rm -rf docker-build
mkdir docker-build
cd docker-build
../vice/configure -v --host=x86_64-w64-mingw32 --enable-native-gtk3ui --enable-cpuhistory --enable-arch=no 2>&1 |tee conf.log
make -j12 2>&1 | tee make.log
make bindistzip 2>&1| tee -a make.log
