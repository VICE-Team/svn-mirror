#!/bin/sh

splitcpuos()
{
  cpu=$1
  shift
  man=$1
  shift
  if test x"$2" != "x"; then
    os=$1
    shift
    for i in $*
    do
      os="$os-$i"
    done
  else
    if test x"$1" != "x"; then
      os=$1
    else
      os=$man
    fi
  fi
}

srcdir=""
shared=no
static=no
makecommand=""
extra_generic_enables=""
extra_ffmpeg_enables=""
extra_x264_enables=""
host=""
cpu=""
os=""
compiler=""

for i in $*
do
  case "$i" in
    --srcdir*)
      srcdir=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
    --enable-make-command*)
      makecommand=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
    --enable-compiler*)
      compiler=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
    --enable-shared-ffmpeg)
      shared=yes
      ;;
    --enable-static-ffmpeg)
      static=yes
      ;;
    --enable-w32threads)
      extra_ffmpeg_enables="$extra_ffmpeg_enables $i"
      extra_x264_enables="$extra_x264_enables --enable-win32thread"
      ;;
    --enable-full-host*)
      host=`echo $i | sed -e 's/^[^=]*=//g'`
      splitcpuos `echo $host | sed 's/-/ /g'`
      ;;
    --host*)
      hostprefix=`echo $i | sed -e 's/^[^=]*=//g'`
      ;;
esac
done

NEW_SHELL=""

if test x"$BASH" = "x"; then
  for i in "/bin/bash /usr/bin/bash /usr/local/bin/bash"
  do
    if test -e "$i"; then
      NEW_SHELL=$i
    fi
  done
else
  NEW_SHELL=${SHELL}
fi

curdir=`pwd`

if [ ! -d "../liblame" ]; then
  mkdir ../liblame
fi

cd ../liblame
cur=`pwd`
if test x"$shared" = "xyes"; then
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../liblame/configure -v --enable-shared --disable-frontend --prefix=$cur/../libffmpeg $extra_generic_enables --host=$host"
  else
    config_line="$srcdir/../liblame/configure -v --enable-shared --disable-frontend --prefix=$cur/../libffmpeg $extra_generic_enables"
  fi
else
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../liblame/configure -v --disable-shared --enable-static --disable-frontend --prefix=$cur/../libffmpeg $extra_generic_enables --host=$host"
  else
    config_line="$srcdir/../liblame/configure -v --disable-shared --enable-static --disable-frontend --prefix=$cur/../libffmpeg $extra_generic_enables"
  fi
fi
cat <<__END
Running configure in liblame with $config_line
__END

${NEW_SHELL} $config_line
$makecommand install

if [ ! -d "$cur/../libffmpeg/lib" ]; then
  mkdir -p $cur/../libffmpeg/lib
fi

if [ -f "$cur/../libffmpeg/lib64/libmp3lame.a" ]; then
  cp $cur/../libffmpeg/lib64/libmp3lame.a $cur/../libffmpeg/lib/libmp3lame.a
fi

if [ ! -d "../libx264" ]; then
  mkdir ../libx264
fi

cd ../libx264
cur=`pwd`
if test x"$shared" = "xyes"; then
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../libx264/configure --enable-shared --enable-static --prefix=$cur/../libffmpeg $extra_generic_enables $extra_x264_enables --host=$host --cross-prefix=$hostprefix-"
  else
    config_line="$srcdir/../libx264/configure --enable-shared --enable-static --prefix=$cur/../libffmpeg $extra_generic_enables $extra_x264_enables --compiler=${compiler}"
  fi
else
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../libx264/configure --enable-static --prefix=$cur/../libffmpeg --host=$host --cross-prefix=$hostprefix-"
  else
    config_line="$srcdir/../libx264/configure --enable-static --prefix=$cur/../libffmpeg --compiler=${compiler}"
  fi
fi

cat <<__END
Running configure in libx264 with $config_line
__END

${NEW_SHELL} $config_line
$makecommand install

if [ -f "$cur/../libffmpeg/lib64/libx264.a" ]; then
  cp $cur/../libffmpeg/lib64/libx264.a $cur/../libffmpeg/lib/libx264.a
fi

if [ ! -d "../libffmpeg" ]; then
  mkdir ../libffmpeg
fi

cd ../libffmpeg
cur=`pwd`
if test x"$shared" = "xyes"; then
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --enable-shared --disable-static --disable-programs --enable-gpl $extra_ffmpeg_enables $extra_generic_enables --arch=$cpu --target-os=$os --cross-prefix=$hostprefix-"
  else
    config_line="$srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --enable-shared --disable-static --disable-programs --enable-gpl $extra_ffmpeg_enables $extra_generic_enables --cc=${compiler}"
  fi
else
  if test x"$hostprefix" != "x"; then
    config_line="$srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --enable-static --disable-programs --enable-gpl $extra_ffmpeg_enables $extra_generic_enables  --arch=$cpu --target-os=$os --cross-prefix=$hostprefix-"
  else
    config_line="$srcdir/../libffmpeg/configure --enable-libmp3lame --enable-libx264 --enable-static --disable-programs --enable-gpl $extra_ffmpeg_enables $extra_generic_enables --cc=${compiler}"
  fi
fi

cat <<__END
Running configure in libffmpeg with $config_line --extra-ldflags="-Llib -Llib64" --extra-cflags="-Iinclude"
__END

${NEW_SHELL} $config_line --extra-cflags="-Iinclude" --extra-ldflags="-Llib -Llib64"
