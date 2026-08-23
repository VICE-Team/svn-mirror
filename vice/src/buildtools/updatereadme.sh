#!/bin/bash
# NOTE: Sometimes run via $(SHELL), which may be a non-bash shell

###############################################################################
# updatereadme.sh   - update VICE version / date in the README file
###############################################################################

#VERBOSE=1

# GNU sed and BSD sed disagree about -i (suffix attached vs separate argument),
# hence the wrapper function: in a plain string the quotes are stripped at
# assignment and sed would use the '' as the backup suffix, leaving
# "<file>''" droppings behind.
#
# Both branches use -E, because "\+" is a GNU-only BRE extension that BSD sed
# would read as a literal '+' and silently substitute nothing. The patterns
# are therefore ERE with a plain '+', and the greps sharing them need -E too,
# where '(' and ')' must be escaped (literal in a BRE, group in an ERE).
SEDTESTFILE=`mktemp`
if sed -i 'p' "$SEDTESTFILE" 2>/dev/null
then
    # GNU sed
    sed_i() { LC_ALL=C sed -E -i "$@"; }
else
    # BSD sed
    sed_i() { LC_ALL=C sed -E -i '' "$@"; }
fi
rm -f "$SEDTESTFILE"

README=README
CONFIG=configure.ac
VICEDATE=src/vicedate.h

if [ "x$1" = "x" ]; then
echo Filename for readme not defined, using: $README
else
README=$1
fi

if [ "x$2" = "x" ]; then
echo Filename for configure.ac not defined, using: $CONFIG
else
CONFIG=$2
fi

if [ "x$3" = "x" ]; then
echo Filename for vicedate.h not defined, using: $VICEDATE
else
VICEDATE=$3
fi

if [ "x$VERBOSE" = "x1" ]; then
echo readme:$README
echo config:$CONFIG
echo vicedate:$VICEDATE
fi

VMAJOR=`grep "m4_define.*(.*vice_version_major" $CONFIG | sed "s:m4_define.*(.*vice_version_major.*, \([0-9]*\)):\1:g"`
VMINOR=`grep "m4_define.*(.*vice_version_minor" $CONFIG | sed "s:m4_define.*(.*vice_version_minor.*, \([0-9]*\)):\1:g"`
VBUILD=`grep "m4_define.*(.*vice_version_build" $CONFIG | sed "s:m4_define.*(.*vice_version_build.*, \([0-9]*\)):\1:g"`
VDEV=`grep "m4_define.*(.*vice_version_label" $CONFIG | sed "s:m4_define.*(.*vice_version_label.*, \([a-z]*\)):\1:g"`

#VMINOR=0
#VMINOR=14
#VBUILD=0
#VBUILD=14

if [ "x$VERBOSE" = "x1" ]; then
echo major: $VMAJOR
echo minor: $VMINOR
echo build: $VBUILD
echo dev: $VBUILD
fi

if [ "x$VDEV" = "xdev" ]; then
    echo "release date/version in README is not updated in dev versions"
    exit 0
fi

MONTH=`grep "VICEDATE_MONTH_SHORT " $VICEDATE | cut -d '"' -f 2`
YEAR=`grep "VICEDATE_YEAR " $VICEDATE | cut -d " " -f 3`

if [ "x$VERBOSE" = "x1" ]; then
echo month: $MONTH
echo year: $YEAR
fi

# The top line of README
# "  VICE  3.6.2                                                        Jan 2022"
TOPLINE=`grep -E " +VICE +[0-9]+\.[0-9]+[\.]*[0-9]* +[A-Z][a-z][a-z] 20[0-9][0-9]" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo topline old: $TOPLINE
fi

if [ "x$TOPLINE" = "x" ]; then
    echo "WARNING: top line of README not found, version/date NOT updated."
else
    TOPLINE="  VICE  $VMAJOR.$VMINOR"
    if [ "$VBUILD" = "0" ]; then
    TOPLINE="$TOPLINE  "
    else
    TOPLINE="$TOPLINE.$VBUILD"
    fi
    TOPLINE="$TOPLINE                                                        "
    TOPLINE="$TOPLINE$MONTH $YEAR"
    sed_i -e "s: +VICE +[0-9]+\.[0-9]+[\.]*[0-9]* +[A-Z][a-z][a-z] 20[0-9][0-9]:$TOPLINE:g" $README
fi

TOPLINE=`grep -E " +VICE +[0-9]+\.[0-9]+[\.]*[0-9]* +[A-Z][a-z][a-z] 20[0-9][0-9]" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo topline new: $TOPLINE
fi

# In the second paragraph of the README ("This is version 3.6 of VICE")

LINE=`grep -E "This is version [0-9]+\.[0-9]+[\.]*[0-9]* +of VICE" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo line old: $LINE
fi

if [ "x$LINE" = "x" ]; then
    echo "WARNING: second patch line of README not found, version NOT updated."
else
    LINE="This is version $VMAJOR.$VMINOR"
    if [ "$VBUILD" != "0" ]; then
        LINE="$LINE.$VBUILD"
    fi
    LINE="$LINE of VICE"
    sed_i -e "s:This is version [0-9]+\.[0-9]+[\.]*[0-9]* +of VICE:$LINE:g" $README
fi

LINE=`grep -E "This is version [0-9]+\.[0-9]+[\.]*[0-9]* +of VICE" < $README`
if [ "x$VERBOSE" = "x1" ]; then
echo line new: $LINE
fi
