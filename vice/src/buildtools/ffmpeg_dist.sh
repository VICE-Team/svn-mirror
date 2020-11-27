#!/bin/sh

#
# ffmpeg_dist.sh - make ffmpeg part of the dist when configured without static or shared ffmpeg
#
# Written by
#  Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#  02111-1307  USA.
#
# Usage: ffmpeg_dist.sh <srcdir> <distdir>
#                       $1       $2

SRCDIR=$1
DISTDIR=$2

mkdir $DISTDIR/ffmpeg
cp $SRCDIR/ffmpeg/*.{gnu,am,in} $DISTDIR/ffmpeg

mkdir $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/*.{mak,md} $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/*.{c,h} $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/COPYING.* $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/RELEASE* $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/Changelog $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/configure $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/CREDITS $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/MAINTAINERS $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/Makefile $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/VERSION $DISTDIR/libffmpeg
cp $SRCDIR/libffmpeg/version.sh $DISTDIR/libffmpeg

mkdir $DISTDIR/libffmpeg/compat
cp $SRCDIR/libffmpeg/compat/*.{c,h} $DISTDIR/libffmpeg/compat

cp -r $SRCDIR/libffmpeg/compat/aix $DISTDIR/libffmpeg/compat/aix
cp -r $SRCDIR/libffmpeg/compat/avisynth $DISTDIR/libffmpeg/compat/avisynth
cp -r $SRCDIR/libffmpeg/compat/dos $DISTDIR/libffmpeg/compat/dos
cp -r $SRCDIR/libffmpeg/compat/float $DISTDIR/libffmpeg/compat/float
cp -r $SRCDIR/libffmpeg/compat/msvcrt $DISTDIR/libffmpeg/compat/msvcrt
cp -r $SRCDIR/libffmpeg/compat/plan9 $DISTDIR/libffmpeg/compat/plan9
cp -r $SRCDIR/libffmpeg/compat/tms470 $DISTDIR/libffmpeg/compat/tms470
cp -r $SRCDIR/libffmpeg/compat/windows $DISTDIR/libffmpeg/compat/windows

cp -r $SRCDIR/libffmpeg/doc $DISTDIR/libffmpeg/doc

mkdir $DISTDIR/libffmpeg/libavcodec
cp $SRCDIR/libffmpeg/libavcodec/*.{c,h,cpp,rc,v} $DISTDIR/libffmpeg/libavcodec
cp $SRCDIR/libffmpeg/libavcodec/Makefile $DISTDIR/libffmpeg/libavcodec

mkdir $DISTDIR/libffmpeg/libavcodec/aarch64
cp $SRCDIR/libffmpeg/libavcodec/aarch64/*.{c,h,S} $DISTDIR/libffmpeg/libavcodec/aarch64
cp $SRCDIR/libffmpeg/libavcodec/aarch64/Makefile $DISTDIR/libffmpeg/libavcodec/aarch64

mkdir $DISTDIR/libffmpeg/libavcodec/alpha
cp $SRCDIR/libffmpeg/libavcodec/alpha/*.{c,h,S} $DISTDIR/libffmpeg/libavcodec/alpha
cp $SRCDIR/libffmpeg/libavcodec/alpha/Makefile $DISTDIR/libffmpeg/libavcodec/alpha

mkdir $DISTDIR/libffmpeg/libavcodec/arm
cp $SRCDIR/libffmpeg/libavcodec/arm/*.{c,h,S} $DISTDIR/libffmpeg/libavcodec/arm
cp $SRCDIR/libffmpeg/libavcodec/arm/Makefile $DISTDIR/libffmpeg/libavcodec/arm

cp -r $SRCDIR/libffmpeg/libavcodec/avr32 $DISTDIR/libffmpeg/libavcodec/avr32
cp -r $SRCDIR/libffmpeg/libavcodec/bfin $DISTDIR/libffmpeg/libavcodec/bfin
cp -r $SRCDIR/libffmpeg/libavcodec/sh4 $DISTDIR/libffmpeg/libavcodec/sh4
cp -r $SRCDIR/libffmpeg/libavcodec/sparc $DISTDIR/libffmpeg/libavcodec/sparc
cp -r $SRCDIR/libffmpeg/presets $DISTDIR/libffmpeg/presets
cp -r $SRCDIR/libffmpeg/tools $DISTDIR/libffmpeg/tools

mkdir $DISTDIR/libffmpeg/libavcodec/mips
cp $SRCDIR/libffmpeg/libavcodec/mips/*.{c,h} $DISTDIR/libffmpeg/libavcodec/mips
cp $SRCDIR/libffmpeg/libavcodec/mips/Makefile $DISTDIR/libffmpeg/libavcodec/mips

mkdir $DISTDIR/libffmpeg/libavcodec/neon
cp $SRCDIR/libffmpeg/libavcodec/neon/*.c $DISTDIR/libffmpeg/libavcodec/neon
cp $SRCDIR/libffmpeg/libavcodec/neon/Makefile $DISTDIR/libffmpeg/libavcodec/neon

mkdir $DISTDIR/libffmpeg/libavcodec/ppc
cp $SRCDIR/libffmpeg/libavcodec/ppc/*.{c,h,S} $DISTDIR/libffmpeg/libavcodec/ppc
cp $SRCDIR/libffmpeg/libavcodec/ppc/Makefile $DISTDIR/libffmpeg/libavcodec/ppc

mkdir $DISTDIR/libffmpeg/libavcodec/x86
cp $SRCDIR/libffmpeg/libavcodec/x86/*.{asm,c,h} $DISTDIR/libffmpeg/libavcodec/x86
cp $SRCDIR/libffmpeg/libavcodec/x86/Makefile $DISTDIR/libffmpeg/libavcodec/x86

mkdir $DISTDIR/libffmpeg/libavdevice
cp $SRCDIR/libffmpeg/libavdevice/*.{c,h,rc,m,cpp,v} $DISTDIR/libffmpeg/libavdevice
cp $SRCDIR/libffmpeg/libavdevice/Makefile $DISTDIR/libffmpeg/libavdevice

mkdir $DISTDIR/libffmpeg/libavfilter
cp $SRCDIR/libffmpeg/libavfilter/*.{c,h,inc,rc,v} $DISTDIR/libffmpeg/libavfilter
cp $SRCDIR/libffmpeg/libavfilter/Makefile $DISTDIR/libffmpeg/libavfilter

cp -r $SRCDIR/libffmpeg/libavfilter/libmpcodecs $DISTDIR/libffmpeg/libavfilter/libmpcodecs

mkdir $DISTDIR/libffmpeg/libavfilter/x86
cp $SRCDIR/libffmpeg/libavfilter/x86/*.{c,asm} $DISTDIR/libffmpeg/libavfilter/x86
cp $SRCDIR/libffmpeg/libavfilter/x86/Makefile $DISTDIR/libffmpeg/libavfilter/x86

mkdir $DISTDIR/libffmpeg/libavformat
cp $SRCDIR/libffmpeg/libavformat/*.{c,h,rc,v} $DISTDIR/libffmpeg/libavformat
cp $SRCDIR/libffmpeg/libavformat/Makefile $DISTDIR/libffmpeg/libavformat

mkdir $DISTDIR/libffmpeg/libavresample
cp $SRCDIR/libffmpeg/libavresample/*.{c,h,rc,v} $DISTDIR/libffmpeg/libavresample
cp $SRCDIR/libffmpeg/libavresample/Makefile $DISTDIR/libffmpeg/libavresample

mkdir $DISTDIR/libffmpeg/libavresample/aarch64
cp $SRCDIR/libffmpeg/libavresample/aarch64/*.{c,h,S} $DISTDIR/libffmpeg/libavresample/aarch64
cp $SRCDIR/libffmpeg/libavresample/aarch64/Makefile $DISTDIR/libffmpeg/libavresample/aarch64

mkdir $DISTDIR/libffmpeg/libavresample/arm
cp $SRCDIR/libffmpeg/libavresample/arm/*.{c,S} $DISTDIR/libffmpeg/libavresample/arm
cp $SRCDIR/libffmpeg/libavresample/arm/Makefile $DISTDIR/libffmpeg/libavresample/arm

mkdir $DISTDIR/libffmpeg/libavresample/x86
cp $SRCDIR/libffmpeg/libavresample/x86/*.{c,asm} $DISTDIR/libffmpeg/libavresample/x86
cp $SRCDIR/libffmpeg/libavresample/x86/Makefile $DISTDIR/libffmpeg/libavresample/x86

mkdir $DISTDIR/libffmpeg/libavutil
cp $SRCDIR/libffmpeg/libavutil/*.{c,h,rc,v} $DISTDIR/libffmpeg/libavutil
cp $SRCDIR/libffmpeg/libavutil/Makefile $DISTDIR/libffmpeg/libavutil

mkdir $DISTDIR/libffmpeg/libavutil/aarch64
cp $SRCDIR/libffmpeg/libavutil/aarch64/*.{c,h,S} $DISTDIR/libffmpeg/libavutil/aarch64
cp $SRCDIR/libffmpeg/libavutil/aarch64/Makefile $DISTDIR/libffmpeg/libavutil/aarch64

mkdir $DISTDIR/libffmpeg/libavutil/arm
cp $SRCDIR/libffmpeg/libavutil/arm/*.{c,h,S} $DISTDIR/libffmpeg/libavutil/arm
cp $SRCDIR/libffmpeg/libavutil/arm/Makefile $DISTDIR/libffmpeg/libavutil/arm

cp -r $SRCDIR/libffmpeg/libavutil/avr32 $DISTDIR/libffmpeg/libavutil/avr32
cp -r $SRCDIR/libffmpeg/libavutil/bfin $DISTDIR/libffmpeg/libavutil/bfin
cp -r $SRCDIR/libffmpeg/libavutil/sh4 $DISTDIR/libffmpeg/libavutil/sh4
cp -r $SRCDIR/libffmpeg/libavutil/tomi $DISTDIR/libffmpeg/libavutil/tomi

mkdir $DISTDIR/libffmpeg/libavutil/mips
cp $SRCDIR/libffmpeg/libavutil/mips/*.{c,h} $DISTDIR/libffmpeg/libavutil/mips
cp $SRCDIR/libffmpeg/libavutil/mips/Makefile $DISTDIR/libffmpeg/libavutil/mips

mkdir $DISTDIR/libffmpeg/libavutil/ppc
cp $SRCDIR/libffmpeg/libavutil/ppc/*.{c,h} $DISTDIR/libffmpeg/libavutil/ppc
cp $SRCDIR/libffmpeg/libavutil/ppc/Makefile $DISTDIR/libffmpeg/libavutil/ppc

mkdir $DISTDIR/libffmpeg/libavutil/x86
cp $SRCDIR/libffmpeg/libavutil/x86/*.{c,h,asm} $DISTDIR/libffmpeg/libavutil/x86
cp $SRCDIR/libffmpeg/libavutil/x86/Makefile $DISTDIR/libffmpeg/libavutil/x86

mkdir $DISTDIR/libffmpeg/libpostproc
cp $SRCDIR/libffmpeg/libpostproc/*.{c,h,v,rc} $DISTDIR/libffmpeg/libpostproc
cp $SRCDIR/libffmpeg/libpostproc/Makefile $DISTDIR/libffmpeg/libpostproc

mkdir $DISTDIR/libffmpeg/libswresample
cp $SRCDIR/libffmpeg/libswresample/*.{c,h,v,rc} $DISTDIR/libffmpeg/libswresample
cp $SRCDIR/libffmpeg/libswresample/Makefile $DISTDIR/libffmpeg/libswresample

mkdir $DISTDIR/libffmpeg/libswresample/aarch64
cp $SRCDIR/libffmpeg/libswresample/aarch64/*.{c,S} $DISTDIR/libffmpeg/libswresample/aarch64
cp $SRCDIR/libffmpeg/libswresample/aarch64/Makefile $DISTDIR/libffmpeg/libswresample/aarch64

mkdir $DISTDIR/libffmpeg/libswresample/arm
cp $SRCDIR/libffmpeg/libswresample/arm/*.{c,S} $DISTDIR/libffmpeg/libswresample/arm
cp $SRCDIR/libffmpeg/libswresample/arm/Makefile $DISTDIR/libffmpeg/libswresample/arm

mkdir $DISTDIR/libffmpeg/libswresample/x86
cp $SRCDIR/libffmpeg/libswresample/x86/*.{c,asm} $DISTDIR/libffmpeg/libswresample/x86
cp $SRCDIR/libffmpeg/libswresample/x86/Makefile $DISTDIR/libffmpeg/libswresample/x86

mkdir $DISTDIR/libffmpeg/libswscale
cp $SRCDIR/libffmpeg/libswscale/*.{c,h,v,rc} $DISTDIR/libffmpeg/libswscale
cp $SRCDIR/libffmpeg/libswscale/Makefile $DISTDIR/libffmpeg/libswscale

mkdir $DISTDIR/libffmpeg/libswscale/arm
cp $SRCDIR/libffmpeg/libswscale/arm/*.{c,S} $DISTDIR/libffmpeg/libswscale/arm
cp $SRCDIR/libffmpeg/libswscale/arm/Makefile $DISTDIR/libffmpeg/libswscale/arm

mkdir $DISTDIR/libffmpeg/libswscale/ppc
cp $SRCDIR/libffmpeg/libswscale/ppc/*.{c,h} $DISTDIR/libffmpeg/libswscale/ppc
cp $SRCDIR/libffmpeg/libswscale/ppc/Makefile $DISTDIR/libffmpeg/libswscale/ppc

mkdir $DISTDIR/libffmpeg/libswscale/x86
cp $SRCDIR/libffmpeg/libswscale/x86/*.{c,asm} $DISTDIR/libffmpeg/libswscale/x86
cp $SRCDIR/libffmpeg/libswscale/x86/Makefile $DISTDIR/libffmpeg/libswscale/x86

mkdir $DISTDIR/liblame
cp $SRCDIR/liblame/*.{m4,h,in,wav,mp3,unix,am*,sh,bat,spec,conf*,guess,rpath,sub} $DISTDIR/liblame
cp $SRCDIR/liblame/Makefile.* $DISTDIR/liblame
cp $SRCDIR/liblame/depcomp $DISTDIR/liblame
cp $SRCDIR/liblame/API $DISTDIR/liblame
cp $SRCDIR/liblame/ChangeLog $DISTDIR/liblame
cp $SRCDIR/liblame/configure $DISTDIR/liblame
cp $SRCDIR/liblame/COPYING $DISTDIR/liblame
cp $SRCDIR/liblame/DEFINES $DISTDIR/liblame
cp $SRCDIR/liblame/HACKING $DISTDIR/liblame
cp $SRCDIR/liblame/INSTALL $DISTDIR/liblame
cp $SRCDIR/liblame/install-sh $DISTDIR/liblame
cp $SRCDIR/liblame/LICENSE $DISTDIR/liblame
cp $SRCDIR/liblame/missing $DISTDIR/liblame
cp $SRCDIR/liblame/README* $DISTDIR/liblame
cp $SRCDIR/liblame/STYLEGUIDE $DISTDIR/liblame
cp $SRCDIR/liblame/TODO $DISTDIR/liblame
cp $SRCDIR/liblame/USAGE $DISTDIR/liblame

mkdir $DISTDIR/liblame/ACM
cp $SRCDIR/liblame/ACM/*.{cpp,h,rc,ico,xml,def,inf,txt} $DISTDIR/liblame/ACM
cp $SRCDIR/liblame/ACM/Makefile.* $DISTDIR/liblame/ACM
cp $SRCDIR/liblame/ACM/TODO $DISTDIR/liblame/ACM

mkdir $DISTDIR/liblame/ACM/ADbg
cp $SRCDIR/liblame/ACM/ADbg/*.{cpp,h} $DISTDIR/liblame/ACM/ADbg
cp $SRCDIR/liblame/ACM/ADbg/Makefile.* $DISTDIR/liblame/ACM/ADbg

mkdir $DISTDIR/liblame/ACM/ddk
cp $SRCDIR/liblame/ACM/ddk/*.h $DISTDIR/liblame/ACM/ddk
cp $SRCDIR/liblame/ACM/ddk/Makefile.* $DISTDIR/liblame/ACM/ddk

mkdir $DISTDIR/liblame/ACM/tinyxml
cp $SRCDIR/liblame/ACM/tinyxml/*.{txt,bat,cpp,h} $DISTDIR/liblame/ACM/tinyxml
cp $SRCDIR/liblame/ACM/tinyxml/Makefile.* $DISTDIR/liblame/ACM/tinyxml
cp $SRCDIR/liblame/ACM/tinyxml/dox $DISTDIR/liblame/ACM/tinyxml
cp $SRCDIR/liblame/ACM/tinyxml/makedistlinux $DISTDIR/liblame/ACM/tinyxml

mkdir $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/*.{install,docs,am,in,manpages} $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/changelog $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/compat $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/control $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/copyright $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/rules $DISTDIR/liblame/debian
cp $SRCDIR/liblame/debian/watch $DISTDIR/liblame/debian

mkdir $DISTDIR/liblame/Dll
cp $SRCDIR/liblame/Dll/*.{c,def,h,cpp,htm,pas} $DISTDIR/liblame/Dll
cp $SRCDIR/liblame/Dll/Makefile.* $DISTDIR/liblame/Dll
cp $SRCDIR/liblame/Dll/README $DISTDIR/liblame/Dll

mkdir $DISTDIR/liblame/doc
cp $SRCDIR/liblame/doc/*.{am,in} $DISTDIR/liblame/doc

mkdir $DISTDIR/liblame/doc/html
cp $SRCDIR/liblame/doc/html/*.{html,am,in} $DISTDIR/liblame/doc/html

mkdir $DISTDIR/liblame/doc/man
cp $SRCDIR/liblame/doc/man/*.{1,am,in} $DISTDIR/liblame/doc/man

mkdir $DISTDIR/liblame/dshow
cp $SRCDIR/liblame/dshow/*.{cpp,h,ico,am,in,def,rc,CPP,H} $DISTDIR/liblame/dshow
cp $SRCDIR/liblame/dshow/README $DISTDIR/liblame/dshow

mkdir $DISTDIR/liblame/frontend
cp $SRCDIR/liblame/frontend/*.{c,h,am,in} $DISTDIR/liblame/frontend
cp $SRCDIR/liblame/frontend/depcomp $DISTDIR/liblame/frontend

mkdir $DISTDIR/liblame/include
cp $SRCDIR/liblame/include/*.{def,h,sym,am,in} $DISTDIR/liblame/include

mkdir $DISTDIR/liblame/libmp3lame
cp $SRCDIR/liblame/libmp3lame/*.{c,h,rc,ico,am,in} $DISTDIR/liblame/libmp3lame
cp $SRCDIR/liblame/libmp3lame/depcomp $DISTDIR/liblame/libmp3lame

mkdir $DISTDIR/liblame/libmp3lame/i386
cp $SRCDIR/liblame/libmp3lame/i386/*.{nas,h,am,in} $DISTDIR/liblame/libmp3lame/i386

mkdir $DISTDIR/liblame/libmp3lame/vector
cp $SRCDIR/liblame/libmp3lame/vector/*.{c,h,am,in} $DISTDIR/liblame/libmp3lame/vector

mkdir $DISTDIR/liblame/mac
cp $SRCDIR/liblame/mac/*.{mcp,pch,c,h,am,in} $DISTDIR/liblame/mac

mkdir $DISTDIR/liblame/macosx
cp $SRCDIR/liblame/macosx/*.{plist,pch,am,in} $DISTDIR/liblame/macosx

mkdir $DISTDIR/liblame/macosx/English.lproj
cp $SRCDIR/liblame/macosx/English.lproj/*.{strings,am,in} $DISTDIR/liblame/macosx/English.lproj

mkdir $DISTDIR/liblame/macosx/LAME.xcodeproj
cp $SRCDIR/liblame/macosx/LAME.xcodeproj/*.{pbxproj,am,in} $DISTDIR/liblame/macosx/LAME.xcodeproj

mkdir $DISTDIR/liblame/misc
cp $SRCDIR/liblame/misc/*.{c,vbs,bat,html,pl,sh,am,in} $DISTDIR/liblame/misc
cp $SRCDIR/liblame/misc/auenc $DISTDIR/liblame/misc
cp $SRCDIR/liblame/misc/depcomp $DISTDIR/liblame/misc
cp $SRCDIR/liblame/misc/mlame $DISTDIR/liblame/misc

mkdir $DISTDIR/liblame/mpglib
cp $SRCDIR/liblame/mpglib/*.{c,h,am,in} $DISTDIR/liblame/mpglib
cp $SRCDIR/liblame/mpglib/AUTHORS $DISTDIR/liblame/mpglib
cp $SRCDIR/liblame/mpglib/README $DISTDIR/liblame/mpglib
cp $SRCDIR/liblame/mpglib/depcomp $DISTDIR/liblame/mpglib

mkdir $DISTDIR/liblame/vc_solution
cp $SRCDIR/liblame/vc_solution/*.{vsprops,am,in,sln,vcproj,rules} $DISTDIR/liblame/vc_solution

mkdir $DISTDIR/libx264
cp $SRCDIR/libx264/*.{c,h,guess,sub,sh,rc} $DISTDIR/libx264
cp $SRCDIR/libx264/AUTHORS $DISTDIR/libx264
cp $SRCDIR/libx264/configure $DISTDIR/libx264
cp $SRCDIR/libx264/COPYING $DISTDIR/libx264
cp $SRCDIR/libx264/Makefile $DISTDIR/libx264

mkdir $DISTDIR/libx264/common
cp $SRCDIR/libx264/common/*.{c,h} $DISTDIR/libx264/common

mkdir $DISTDIR/libx264/common/aarch64
cp $SRCDIR/libx264/common/aarch64/*.{S,c,h} $DISTDIR/libx264/common/aarch64

mkdir $DISTDIR/libx264/common/arm
cp $SRCDIR/libx264/common/arm/*.{S,c,h} $DISTDIR/libx264/common/arm

mkdir $DISTDIR/libx264/common/opencl
cp $SRCDIR/libx264/common/opencl/*.{cl,h} $DISTDIR/libx264/common/opencl

mkdir $DISTDIR/libx264/common/ppc
cp $SRCDIR/libx264/common/ppc/*.{c,h} $DISTDIR/libx264/common/ppc

mkdir $DISTDIR/libx264/common/x86
cp $SRCDIR/libx264/common/x86/*.{asm,c,h} $DISTDIR/libx264/common/x86

cp -r $SRCDIR/libx264/doc $DISTDIR/libx264/doc
cp -r $SRCDIR/libx264/dos $DISTDIR/libx264/dos

mkdir $DISTDIR/libx264/encoder
cp $SRCDIR/libx264/encoder/*.{c,h} $DISTDIR/libx264/encoder

mkdir $DISTDIR/libx264/extras
cp $SRCDIR/libx264/extras/*.{c,h,pl} $DISTDIR/libx264/extras

mkdir $DISTDIR/libx264/extras/windowsPorts
cp $SRCDIR/libx264/extras/windowsPorts/*.h $DISTDIR/libx264/extras/windowsPorts

mkdir $DISTDIR/libx264/filters
cp $SRCDIR/libx264/filters/*.{c,h} $DISTDIR/libx264/filters

mkdir $DISTDIR/libx264/filters/video
cp $SRCDIR/libx264/filters/video/*.{c,h} $DISTDIR/libx264/filters/video

mkdir $DISTDIR/libx264/input
cp $SRCDIR/libx264/input/*.{c,h} $DISTDIR/libx264/input

mkdir $DISTDIR/libx264/output
cp $SRCDIR/libx264/output/*.{c,h} $DISTDIR/libx264/output

cp -r $SRCDIR/libx264/tools $DISTDIR/libx264/tools
