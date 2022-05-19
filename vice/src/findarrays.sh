#! /bin/bash

#
# findarrays.sh - find excessively large arrays in the binary files
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

BINNAME=x64sc

function findallarrays
{
    nm -a -C -S --size-sort $BINNAME | sed 's/^[0-9a-f]* //g' | grep -v '^[0-9a-f]* [tT] '
    BYTES=0
    for N in `nm --radix=d -a -C -S --size-sort $BINNAME | sed 's/^[0-9a-f]* //g' | grep -v '^[0-9a-f]* [tT] ' | sed 's/^\([0-9a-f]*\) .*/\1/g' | sed 's/^0*//'`; do
#        echo adding: $N
        BYTES=$(($BYTES + $N))
#        echo       : $BYTES
    done
    echo "total bss+data: $BYTES bytes"
    BYTES=0
    for N in `nm --radix=d -a -C -S --size-sort $BINNAME | sed 's/^[0-9a-f]* //g' | sed 's/^\([0-9a-f]*\) .*/\1/g' | sed 's/^0*//'`; do
#        echo adding: $N
        BYTES=$(($BYTES + $N))
#        echo       : $BYTES
    done
    echo "total +code:    $BYTES bytes"
}

################################################################################
function usage
{
    echo "usage: findarrays.sh <option> <binary>"
    echo "where option is one of:"
    echo "list <binname>    - find all (large) arrays in <binname>"
    echo "all <binname>     - all of the above"
    echo "if <binname> is not given, x64sc will be used"
}
################################################################################

OLDCWD=`pwd`
cd `dirname $0`

if [ x"$2"x != xx ]; then
    if [ -f "$2" ]; then
        BINNAME=$2;
    else
        echo "File "$2" does not exist, using "$BINNAME"."
    fi
fi

case $1 in
    list)
        findallarrays ;;
    all)
        findallarrays ;;
    *)
        usage ;;
esac

cd $OLDCWD
