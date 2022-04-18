#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_PATH=./src/

cd $SCRIPT_PATH/../../

FILES=`find $CHECK_PATH -type f \( \( -name \*.c \! -name mon_lex.c \! -name mon_parse.c \) -o \( -name \*.h \! -name box_drawing.h \! -name p64.h \) -o -name \*.cc -o -name \*.m \) -print | xargs grep -l $'\t'`

cd $OLDCWD

if [ x"$FILES"x != xx ]; then
    echo "error: found TABs in the following files:"
    for n in $FILES; do
        echo $n
    done
    exit -1
fi
