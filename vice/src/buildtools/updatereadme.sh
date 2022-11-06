#! /bin/bash

###############################################################################
# updatereadme.sh   - update VICE version / date in the README file
###############################################################################

#VERBOSE=1

README=README
CONFIG=configure.ac

if [ "x$1" = "x" ]; then
echo Filename for readme not defined, using: $README
else
README=$1
fi

if [ "x$1" = "x" ]; then
echo Filename for configure.ac not defined, using: $CONFIG
else
CONFIG=$2
fi

if [ "x$VERBOSE" = "x1" ]; then
echo readme:$README
echo config:$CONFIG
fi

VMAJOR=`grep "m4_define.*(.*vice_version_major" $CONFIG | sed "s:m4_define.*(.*vice_version_major.*, \([0-9]*\)):\1:g"`
VMINOR=`grep "m4_define.*(.*vice_version_minor" $CONFIG | sed "s:m4_define.*(.*vice_version_minor.*, \([0-9]*\)):\1:g"`
VBUILD=`grep "m4_define.*(.*vice_version_build" $CONFIG | sed "s:m4_define.*(.*vice_version_build.*, \([0-9]*\)):\1:g"`

#VMINOR=0
#VMINOR=14
#VBUILD=0
#VBUILD=14

if [ "x$VERBOSE" = "x1" ]; then
echo major: $VMAJOR
echo minor: $VMINOR
echo build: $VBUILD
fi

MONTH=`LANG=C date +%b`
YEAR=`LANG=C date +%Y`

if [ "x$VERBOSE" = "x1" ]; then
echo month $MONTH
echo year: $YEAR
fi

# The top line of README
# "  VICE  3.6.2                                                        Jan 2022"
TOPLINE=`grep " \+VICE \+[0-9]\+\.[0-9]\+[\.]*[0-9]* \+[A-Z][a-z][a-z] 20[0-9][0-9]" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo topline old: $TOPLINE
fi

if [ "x$TOPLINE" = "x" ]; then
    echo "WARNING: top line of README not found, version/date NOT updated."
else
    TOPLINE="  VICE  "
    TOPLINE+=$VMAJOR.$VMINOR
    if [ "$VBUILD" = "0" ]; then
    TOPLINE+="  "
    else
    TOPLINE+=.$VBUILD
    fi
    TOPLINE+="                                                        "
    TOPLINE+=$MONTH
    TOPLINE+=" "
    TOPLINE+=$YEAR
    sed -i -e "s: \+VICE \+[0-9]\+\.[0-9]\+[\.]*[0-9]* \+[A-Z][a-z][a-z] 20[0-9][0-9]:$TOPLINE:g" $README
fi

TOPLINE=`grep " \+VICE \+[0-9]\+\.[0-9]\+[\.]*[0-9]* \+[A-Z][a-z][a-z] 20[0-9][0-9]" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo topline new: $TOPLINE
fi

# In the second paragraph of the README ("This is version 3.6 of VICE")

LINE=`grep "This is version [0-9]\+\.[0-9]\+[\.]*[0-9]* \+of VICE" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo line old: $LINE
fi

if [ "x$LINE" = "x" ]; then
    echo "WARNING: second patch line of README not found, version NOT updated."
else
    LINE="This is version "
    LINE+=$VMAJOR.$VMINOR
    if [ "$VBUILD" = "0" ]; then
    LINE+=" "
    else
    LINE+=.$VBUILD
    LINE+=" "
    fi
    LINE+="of VICE"
    sed -i -e "s:This is version [0-9]\+\.[0-9]\+[\.]*[0-9]* \+of VICE:$LINE:g" $README
fi

LINE=`grep "This is version [0-9]\+\.[0-9]\+[\.]*[0-9]* \+of VICE" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo line new: $LINE
fi
