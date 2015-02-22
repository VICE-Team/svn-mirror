#!/bin/sh

srcdir=""
shared=no
static=no
makecommand=""

for i in $*
do
  case "$i" in
    --srcdir*)
      srcdir=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
    --enable-make-command*)
      makecommand=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
    --enable-shared-ffmpeg)
      shared=yes
      ;;
    --enable-static-ffmpeg)
      static=yes
      ;;
  esac
done

curdir=`pwd`

if [ ! -d "../liblame" ]; then
  mkdir ../liblame
fi

cd ../liblame
cur=`pwd`
if test x"$shared" = "xyes"; then
  $srcdir/../liblame/configure -v --enable-shared --disable-frontend --prefix=$cur/../libffmpeg
  $makecommand install
else
  $srcdir/../liblame/configure -v --disable-shared --enable-static --disable-frontend --prefix=$cur/../libffmpeg
  $makecommand install
fi

if [ ! -d "../libx264" ]; then
  mkdir ../libx264
fi

cd ../libx264
cur=`pwd`
if test x"$shared" = "xyes"; then
  $srcdir/../libx264/configure --enable-shared --enable-static --prefix=$cur/../libffmpeg
  $makecommand install
else
  $srcdir/../libx264/configure --disable-shared --enable-static --prefix=$cur/../libffmpeg
  $makecommand install
fi

if [ ! -d "../libffmpeg" ]; then
  mkdir ../libffmpeg
fi

cd ../libffmpeg
cur=`pwd`
if test x"$shared" = "xyes"; then
  $srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --enable-shared --disable-static --disable-programs --enable-gpl --extra-cflags="-Iinclude" --extra-ldflags="-Llib"
else
  $srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --disable-shared --enable-static --disable-programs --enable-gpl --extra-cflags="-Iinclude" --extra-ldflags="-Llib"
fi
