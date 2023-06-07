#!/bin/bash

#
# findhacks.sh - find 'hacks' in source files
#
# Written by
#  groepaz <groepaz@gmx.net>
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

# list of all VALID arch-dependent global defines
# if any of these is found in the common part of the source, it should ideally
# be removed.
# these symbols are defined by configure
ARCHDEFS+=" USE_SDLUI"
ARCHDEFS+=" USE_SDL2UI"
ARCHDEFS+=" USE_GTK3UI"
ARCHDEFS+=" USE_HEADLESSUI"

ARCHDEFS+=" WINDOWS_COMPILE"
ARCHDEFS+=" WIN64_COMPILE"
ARCHDEFS+=" MACOS_COMPILE"
ARCHDEFS+=" LINUX_COMPILE"
ARCHDEFS+=" BEOS_COMPILE"
ARCHDEFS+=" HAIKU_COMPILE"
ARCHDEFS+=" UNIX_COMPILE"
ARCHDEFS+=" BSD_COMPILE"
ARCHDEFS+=" FREEBSD_COMPILE"
ARCHDEFS+=" DRAGONFLYBSD_COMPILE"
ARCHDEFS+=" NETBSD_COMPILE"
ARCHDEFS+=" OPENBSD_COMPILE"

# todo: seperated check for CPU defs
ARCHDEFS+=" __i386__"
ARCHDEFS+=" __i486__"
ARCHDEFS+=" __i586__"
ARCHDEFS+=" __i686__"
ARCHDEFS+=" __x86_64__"
ARCHDEFS+=" __amd64__"
ARCHDEFS+=" __PPC__"
ARCHDEFS+=" __ppc"
ARCHDEFS+=" __powerpc__"
ARCHDEFS+=" __m68020__"
ARCHDEFS+=" __m68030__"
ARCHDEFS+=" __m68040__"
ARCHDEFS+=" __m68060__"

ARCHDEFS+=" WORDS_BIGENDIAN"

# list of OBSOLETE global arch-dependent defines. whenever one gets removed or
# even just renamed, add it here
# if any of these is found _anywhere_ in the source, it should be removed
OBSOLETEARCHDEFS+=" SDL_UI_SUPPORT"
OBSOLETEARCHDEFS+=" SDL_COMPILE"
OBSOLETEARCHDEFS+=" SDL2_COMPILE"
OBSOLETEARCHDEFS+=" USE_SDLUI2"
OBSOLETEARCHDEFS+=" HEADLESS_COMPILE"
OBSOLETEARCHDEFS+=" MSDOS"
OBSOLETEARCHDEFS+=" __MSDOS__"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_WINDOWS"
OBSOLETEARCHDEFS+=" WIN32"
OBSOLETEARCHDEFS+=" WIN32_COMPILE"
OBSOLETEARCHDEFS+=" _WIN64"
OBSOLETEARCHDEFS+=" WINCE"
OBSOLETEARCHDEFS+=" __XBOX__"
OBSOLETEARCHDEFS+=" GP2X"
OBSOLETEARCHDEFS+=" GP2X_SDL"
OBSOLETEARCHDEFS+=" WIZ"
OBSOLETEARCHDEFS+=" riscos"
OBSOLETEARCHDEFS+=" __riscos"
OBSOLETEARCHDEFS+=" __riscos__"
OBSOLETEARCHDEFS+=" __RISCOS__"
OBSOLETEARCHDEFS+=" DARWIN_COMPILE"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_MACOS"
OBSOLETEARCHDEFS+=" MACOSX_SUPPORT"
OBSOLETEARCHDEFS+=" __APPLE__"
OBSOLETEARCHDEFS+=" MACOSX_COCOA"
OBSOLETEARCHDEFS+=" UNIX_MACOSX_COMPILE"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_LINUX"
OBSOLETEARCHDEFS+=" __linux__"
OBSOLETEARCHDEFS+=" __linux"
OBSOLETEARCHDEFS+=" USE_NATIVE_GTK3"
OBSOLETEARCHDEFS+=" NATIVE_GTK3_COMPILE"
OBSOLETEARCHDEFS+=" USE_GNOMEUI"
OBSOLETEARCHDEFS+=" USE_XAWUI"
OBSOLETEARCHDEFS+=" USE_XF86_EXTENSIONS"
OBSOLETEARCHDEFS+=" USE_XF86_VIDMODE_EXT"
OBSOLETEARCHDEFS+=" USE_XAW3D"
OBSOLETEARCHDEFS+=" USE_MITSHM"
OBSOLETEARCHDEFS+=" HAVE_XRANDR"
OBSOLETEARCHDEFS+=" XSync"
OBSOLETEARCHDEFS+=" UseXSync"
OBSOLETEARCHDEFS+=" USE_BEOS_UI"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BEOS"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BEOS_R5"
OBSOLETEARCHDEFS+=" __BEOS__"
OBSOLETEARCHDEFS+=" __HAIKU__"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_HAIKU"
OBSOLETEARCHDEFS+=" DINGOO_NATIVE"
OBSOLETEARCHDEFS+=" DINGUX_SDL"
OBSOLETEARCHDEFS+=" __MACH__"
OBSOLETEARCHDEFS+=" __DragonFly__"
OBSOLETEARCHDEFS+=" __DragonflyBSD__"
OBSOLETEARCHDEFS+=" __bsdi__"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BSD"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BSD_DRAGON"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BSD_FREE"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BSD_NET"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_BSD_OPEN"
OBSOLETEARCHDEFS+=" __FreeBSD__"
OBSOLETEARCHDEFS+=" __NetBSD__"
OBSOLETEARCHDEFS+=" __OpenBSD__"
OBSOLETEARCHDEFS+=" OPENSTEP_COMPILE"
OBSOLETEARCHDEFS+=" RHAPSODY_COMPILE"
OBSOLETEARCHDEFS+=" NEXTSTEP_COMPILE"
OBSOLETEARCHDEFS+=" __INTERIX"
OBSOLETEARCHDEFS+=" __sortix__"
OBSOLETEARCHDEFS+=" SKYOS"
OBSOLETEARCHDEFS+=" MINIXVMD"
OBSOLETEARCHDEFS+=" MINIX_SUPPORT"
OBSOLETEARCHDEFS+=" __NeXT__"
OBSOLETEARCHDEFS+=" __QNX__"
OBSOLETEARCHDEFS+=" __QNXNTO__"
OBSOLETEARCHDEFS+=" __OS2__"
OBSOLETEARCHDEFS+=" OS2_COMPILE"
OBSOLETEARCHDEFS+=" OS2"
OBSOLETEARCHDEFS+=" _AIX"
OBSOLETEARCHDEFS+=" __sgi"
OBSOLETEARCHDEFS+=" sgi"
OBSOLETEARCHDEFS+=" __hpux"
OBSOLETEARCHDEFS+=" _hpux"
OBSOLETEARCHDEFS+=" sun"
OBSOLETEARCHDEFS+=" __sun"
OBSOLETEARCHDEFS+=" SVR4"
OBSOLETEARCHDEFS+=" __SVR4"
OBSOLETEARCHDEFS+=" __svr4__"
OBSOLETEARCHDEFS+=" __osf__"
OBSOLETEARCHDEFS+=" __osf"
OBSOLETEARCHDEFS+=" __sparc64__"
OBSOLETEARCHDEFS+=" sparc64"
OBSOLETEARCHDEFS+=" __sparc__"
OBSOLETEARCHDEFS+=" sparc"
OBSOLETEARCHDEFS+=" VMS"
OBSOLETEARCHDEFS+=" ANDROID_COMPILE"
OBSOLETEARCHDEFS+=" __ANDROID__"
OBSOLETEARCHDEFS+=" AMIGA_SUPPORT"
OBSOLETEARCHDEFS+=" AMIGA_AROS"
OBSOLETEARCHDEFS+=" AMIGA_M68K"
OBSOLETEARCHDEFS+=" AMIGA_MORPHOS"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_AMIGA"
OBSOLETEARCHDEFS+=" HAVE_DEVICES_AHI_H"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_AMIGA"
OBSOLETEARCHDEFS+=" ARCHDEP_OS_UNIX"

# list of all valid compiler specific global defines
CCARCHDEFS+=" __GNUC__"
CCARCHDEFS+=" __STDC__"
CCARCHDEFS+=" _POSIX_SOURCE"
CCARCHDEFS+=" _INCLUDE_POSIX_SOURCE"
CCARCHDEFS+=" _MSC_VER"
CCARCHDEFS+=" WINVER"
CCARCHDEFS+=" WATCOM_COMPILE"
CCARCHDEFS+=" __WATCOMC__"
CCARCHDEFS+=" __VBCC__"
CCARCHDEFS+=" __ICC"
CCARCHDEFS+=" __DMC__"
CCARCHDEFS+=" __IBMC__"
CCARCHDEFS+=" llvm"

# list of OBSOLETE resources. whenever a resource gets removed or even just
# renamed, it should get added here (comment them out if no more are left)
#OBSOLETERESOURCES+=" RomsetKernalName"
#OBSOLETERESOURCES+=" RomsetBasicName"
#OBSOLETERESOURCES+=" RomsetBasic64Name"
#OBSOLETERESOURCES+=" RomsetBasicHiName"
#OBSOLETERESOURCES+=" RomsetBasicLoName"
#OBSOLETERESOURCES+=" RomsetBasicName"
#OBSOLETERESOURCES+=" RomsetCart1Name"
#OBSOLETERESOURCES+=" RomsetCart2Name"
#OBSOLETERESOURCES+=" RomsetCart4Name"
#OBSOLETERESOURCES+=" RomsetCart6Name"
#OBSOLETERESOURCES+=" RomsetChargenDEName"
#OBSOLETERESOURCES+=" RomsetChargenFRName"
#OBSOLETERESOURCES+=" RomsetChargenIntName"
#OBSOLETERESOURCES+=" RomsetChargenName"
#OBSOLETERESOURCES+=" RomsetChargenSEName"
#OBSOLETERESOURCES+=" RomsetDosName1001"
#OBSOLETERESOURCES+=" RomsetDosName1540"
#OBSOLETERESOURCES+=" RomsetDosName1541"
#OBSOLETERESOURCES+=" RomsetDosName1541ii"
#OBSOLETERESOURCES+=" RomsetDosName1551"
#OBSOLETERESOURCES+=" RomsetDosName1570"
#OBSOLETERESOURCES+=" RomsetDosName1571"
#OBSOLETERESOURCES+=" RomsetDosName1571cr"
#OBSOLETERESOURCES+=" RomsetDosName1581"
#OBSOLETERESOURCES+=" RomsetDosName2000"
#OBSOLETERESOURCES+=" RomsetDosName2031"
#OBSOLETERESOURCES+=" RomsetDosName2040"
#OBSOLETERESOURCES+=" RomsetDosName3040"
#OBSOLETERESOURCES+=" RomsetDosName4000"
#OBSOLETERESOURCES+=" RomsetDosName4040"
#OBSOLETERESOURCES+=" RomsetEditorName"
#OBSOLETERESOURCES+=" RomsetFunctionHighName"
#OBSOLETERESOURCES+=" RomsetFunctionLowName"
#OBSOLETERESOURCES+=" RomsetH6809RomAName"
#OBSOLETERESOURCES+=" RomsetH6809RomBName"
#OBSOLETERESOURCES+=" RomsetH6809RomCName"
#OBSOLETERESOURCES+=" RomsetH6809RomDName"
#OBSOLETERESOURCES+=" RomsetH6809RomEName"
#OBSOLETERESOURCES+=" RomsetH6809RomFName"
#OBSOLETERESOURCES+=" RomsetKernal64Name"
#OBSOLETERESOURCES+=" RomsetKernalDEName"
#OBSOLETERESOURCES+=" RomsetKernalFIName"
#OBSOLETERESOURCES+=" RomsetKernalFRName"
#OBSOLETERESOURCES+=" RomsetKernalITName"
#OBSOLETERESOURCES+=" RomsetKernalIntName"
#OBSOLETERESOURCES+=" RomsetKernalNOName"
#OBSOLETERESOURCES+=" RomsetKernalName"
#OBSOLETERESOURCES+=" RomsetKernalSEName"
#OBSOLETERESOURCES+=" RomsetRomModule9Name"
#OBSOLETERESOURCES+=" RomsetRomModuleAName"
#OBSOLETERESOURCES+=" RomsetRomModuleBName"

OBSOLETERESOURCES+=" UseVicII"
OBSOLETERESOURCES+=" SidParSIDport"
OBSOLETERESOURCES+=" REUfirstUnusedRegister"

OBSOLETERESOURCES+=" PALEmulation"
OBSOLETERESOURCES+=" VICIIScale2x"
OBSOLETERESOURCES+=" VICScale2x"
OBSOLETERESOURCES+=" TEDScale2x"
OBSOLETERESOURCES+=" ColorSaturation"
OBSOLETERESOURCES+=" ColorContrast"
OBSOLETERESOURCES+=" ColorBrightness"
OBSOLETERESOURCES+=" ColorGamma"
OBSOLETERESOURCES+=" ColorTint"
OBSOLETERESOURCES+=" PALScanLineShade"
OBSOLETERESOURCES+=" PALBlur"
OBSOLETERESOURCES+=" PALOddLinePhase"
OBSOLETERESOURCES+=" PALOddLineOffset"

################################################################################

# find archdep ifdefs in portable code
function findifdefs
{
    echo "checking define: \"$1\""
    find -wholename './config.h' -prune -o -wholename './vicefeatures.c' -prune -o -wholename './joystickdrv' -prune -o -wholename './iodrv' -prune -o -wholename './socketdrv' -prune -o -wholename './mididrv' -prune -o -wholename './hwsiddrv' -prune -o -wholename './sounddrv' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './platform' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep -n '# *if' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | grep -v "^./src/lib/" | grep --color "$1"
    find -wholename './config.h' -prune -o -wholename './vicefeatures.c' -prune -o -wholename './joystickdrv' -prune -o -wholename './iodrv' -prune -o -wholename './socketdrv' -prune -o -wholename './mididrv' -prune -o -wholename './hwsiddrv' -prune -o -wholename './sounddrv' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './platform' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep -n '# *define' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | grep -v "^./src/lib/" | grep --color "$1"
    echo " "
}

# find archdep ifdefs in portable code
function finddefsfiles
{
    FILES+=`find -wholename './config.h' -prune -o -wholename './joystickdrv' -prune -o -wholename './iodrv' -prune -o -wholename './socketdrv' -prune -o -wholename './mididrv' -prune -o -wholename './hwsiddrv' -prune -o -wholename './sounddrv' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './platform' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep '# *if' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | sed 's/\(.*[ch]:\).*/\1/' | grep -v "^./src/lib/" | grep -v "^./src/arch/"`
    FILES+=`find -wholename './config.h' -prune -o -wholename './joystickdrv' -prune -o -wholename './iodrv' -prune -o -wholename './socketdrv' -prune -o -wholename './mididrv' -prune -o -wholename './hwsiddrv' -prune -o -wholename './sounddrv' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './platform' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep '# *define' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | sed 's/\(.*[ch]:\).*/\1/' | grep -v "^./src/lib/" | grep -v "^./src/arch/"`
}

# find obsolete/compiler ifdefs in all code
function findifdefsfulltree
{
    echo "checking define: \"$1\""
    find  -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep -n '# *if' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | grep -v "^./src/lib/"  | grep --color "$1"
    find  -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep -n '# *elif' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | grep -v "^./src/lib/"  | grep --color "$1"
    find  -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep -n '# *define' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | grep -v "^./src/lib/"  | grep --color "$1"
    grep -Hn --color "$1[^a-zA-Z_]" ../configure.ac
    grep -Hn --color "$1$" ../configure.ac
    find .. -name "Makefile.am" -print | xargs grep -Hn --color "$1[^a-zA-Z_]"
    find .. -name "Makefile.am" -print | xargs grep -Hn --color "$1$"
    echo " "
}

# find obsolete/compiler ifdefs in all code
function finddefsfilesfulltree
{
    FILES+=`find -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep '# *if' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | sed 's/\(.*[ch]:\).*/\1/'  | grep -v "^./src/lib/" `
    FILES+=`find -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep '# *elif' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | sed 's/\(.*[ch]:\).*/\1/'  | grep -v "^./src/lib/" `
    FILES+=`find -wholename './lib' -prune -o -name '*.[ch]' -print -o -name '*.cc' -print -o -name '*.hh' -print | xargs grep '# *define' | sed 's:\(.*\)$:\1^:g' | grep "$1[ )^]" | sed 's:\(.*\)^$:\1:g' | sed 's/\(.*[ch]:\).*/\1/'  | grep -v "^./src/lib/" `
    FILES+=`grep -l "$1[^a-zA-Z_]" ../configure.ac`":"
    FILES+=`grep -l "$1$" ../configure.ac`":"
    FILES+=`find .. -name "Makefile.am" -print | xargs grep -l "$1[^a-zA-Z_]" | sed 's:\(.*\)$:\1\::g'`
    FILES+=`find .. -name "Makefile.am" -print | xargs grep -l "$1$" | sed 's:\(.*\)$:\1\::g'`
}

# find non latin chars
function findnonlatin
{
    echo "-------------------------------------------------------------------------"
    echo "- files with non ASCII characters in them. usually this should only be"
    echo "- the case for files that have translation-related string tables in them."
    echo "-"
    echo "- expected files with different encoding are: infocontrib.h"
    echo "-"
    echo "- all other files should be ASCII"
    echo "-"
    echo "checking character encoding"
    find -wholename './lib' -prune -o -name "*.[ch]" -exec file {} \; | grep -v "ASCII text" | grep -v "CSV text"
    echo " "
}

function findres
{
    echo "checking resource: \"$1\""
    find -name '*.[ch]' -print | xargs grep --color -in '"'$1'"'
}

################################################################################

function findprintfs
{
echo "-------------------------------------------------------------------------"
echo "- fprintf to stdout/stderr in portable emulator code (should perhaps go to log)"
echo "-"
find -wholename './tools' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './bin2c.c' -prune -o -wholename './buildtools/bin2c.c' -prune -o -wholename './buildtools/palette2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -name '*.[ch]' -print | xargs grep -n 'printf' | sed 's:/\* .* \*/::g' | grep --color 'fprintf *( *std'
echo "-------------------------------------------------------------------------"
echo "- printf in portable emulator code (should perhaps go to log)"
echo "-"
find -wholename './tools' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './bin2c.c' -prune -o -wholename './buildtools/bin2c.c' -prune -o -wholename './buildtools/dat2h.c' -prune -o -wholename './buildtools/palette2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './buildtools/geninfocontrib.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -name '*.[ch]' -print | xargs grep -n ' printf' | grep -v '^.*:#define DBG' | grep -v '^.*:#define DEBUG' | sed 's:/\* .* \*/::g' | grep --color 'printf'
find -wholename './tools' -prune -o -wholename './lib' -prune -o -wholename './arch' -prune -o -wholename './bin2c.c' -prune -o -wholename './buildtools/bin2c.c' -prune -o -wholename './buildtools/dat2h.c' -prune -o -wholename './buildtools/palette2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './buildtools/geninfocontrib.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -name '*.[ch]' -print | xargs grep -n '^printf' | grep -v '^.*:#define DBG' | grep -v '^.*:#define DEBUG' | sed 's:/\* .* \*/::g' | grep --color 'printf'

echo "-------------------------------------------------------------------------"
echo "- fprintf to stdout/stderr in archdep code (should go to log if debug output)"
echo "-"
find -wholename './arch/win32/utils' -prune -o -wholename './bin2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -wholename "./arch/*" -a  -name '*.[ch]' -print | xargs grep -n 'printf' | sed 's:/\* .* \*/::g' | grep --color 'fprintf *( *std'
echo "-------------------------------------------------------------------------"
echo "- printf in archdep code (should go to log if debug output)"
echo "-"
find -wholename './arch/win32/utils' -prune -o -wholename './arch/win32/vs_tmpl/mkmsvc.c' -prune -o -wholename './bin2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -wholename "./arch/*" -a -name '*.[ch]' -print | xargs grep -n ' printf' | grep -v '^.*:#define DBG' | grep -v '^.*:#define DEBUG' | sed 's:/\* .* \*/::g' | grep --color 'printf'
find -wholename './arch/win32/utils' -prune -o -wholename './arch/win32/vs_tmpl/mkmsvc.c' -prune -o -wholename './bin2c.c' -prune -o -wholename './cartconv.c' -prune -o -wholename './petcat.c' -prune -o -wholename './c1541.c' -prune -o -wholename "./arch/*" -a -name '*.[ch]' -print | xargs grep -n '^printf' | grep -v '^.*:#define DBG' | grep -v '^.*:#define DEBUG' | sed 's:/\* .* \*/::g' | grep --color 'printf'
}

function finddefs
{
FILES=""

echo "-------------------------------------------------------------------------"
echo "- archdep defines found in portable code (eliminate if possible)"
echo "- these are USE_xxxUI and xxx_COMPILE symbols defined by configure"
echo "-" $ARCHDEFS
echo " "

for I in $ARCHDEFS; do
    findifdefs $I
done

echo "-------------------------------------------------------------------------"
echo "- list of all files containing archdep defines in portable code:"

for I in $ARCHDEFS; do
    finddefsfiles $I
done

echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u
#echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u | wc -l
}

function findccdefs
{
FILES=""

echo "-------------------------------------------------------------------------"
echo "- compiler specific defines (these should be avoided!):"
echo "-" $CCARCHDEFS
echo "-"
echo "- we expect these in vice.h, besides that there shouldnt be any - except"
echo "- in generated code (mon_parse.c, mon_lex.c) and 'external' code like"
echo "- novte, mingw32-pcap."
echo "-"
echo " "

for I in $CCARCHDEFS; do
    findifdefsfulltree $I
done

echo "-------------------------------------------------------------------------"
echo "- list of all files containing compiler specific defines:"

for I in $CCARCHDEFS; do
    finddefsfilesfulltree $I
done

echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u
#echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u | wc -l
}

function findobsolete
{
FILES=""

echo "-------------------------------------------------------------------------"
echo "- obsolete defines (these should be removed!):"
echo "-" $OBSOLETEARCHDEFS
echo " "

for I in $OBSOLETEARCHDEFS; do
    findifdefsfulltree $I
done

echo "-------------------------------------------------------------------------"
echo "- list of all files containing obsolete defines:"

for I in $OBSOLETEARCHDEFS; do
    finddefsfilesfulltree $I
done

echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u
#echo $FILES | tr ':' '\n' | sed 's:^ ::' | sort -u | wc -l
}

function findresources
{
echo "-------------------------------------------------------------------------"
echo "- checking various obsolete resources (which might have either been"
echo "- removed or renamed)."
echo "-"
for I in $OBSOLETERESOURCES; do
    findres $I
done
}

################################################################################
function usage
{
    echo "usage: findhacks.sh <option>"
    echo "where option is one of:"
    echo "encoding  - find non ASCII characters"
    echo "archdep   - find arch dependant ifdefs in portable code"
    echo "ccarchdep - find compiler specific ifdefs"
    echo "obsolete  - find obsolete ifdefs in all code"
    echo "printf    - find printfs (which perhaps should go to the log instead)"
    echo "res       - find obsolete resources"
    echo "all       - all of the above"
}
################################################################################

OLDCWD=`pwd`
cd `dirname $0`

case $1 in
    encoding)
        findnonlatin ;;
    archdep)
        finddefs ;;
    ccarchdep)
        findccdefs ;;
    obsolete)
        findobsolete ;;
    printf)
        findprintfs ;;
    res)
        findresources ;;
    all)
        findnonlatin
        finddefs
        findccdefs
        findobsolete
        findprintfs
        findresources ;;
    *)
        usage ;;
esac

cd $OLDCWD
