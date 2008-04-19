#!/bin/sh
AR=ar RANLIB=true CC=cc bigsh configure --prefix=/usr/local --without-resid --disable-nls
make
make install
for b in x64 x128 xvic xplus4 xpet xcbm2 petcat c1541 cartconv
do chmem +10000000 /usr/local/bin/$b
done
