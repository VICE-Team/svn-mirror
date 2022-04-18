#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`
CHECK_PATH=./src/

cd $SCRIPT_PATH/../../

nl='
'
nl=$'\n'
FILES=`find $CHECK_PATH -type f \( -name \*.c -o -name \*.h -o -name \*.cc -o -name \*.m -o -name Makefile.am \) -print | while read f; do
    t=$(tail -c2 $f; printf x); r1="${nl}$";
    [[ ${t%x} =~ $r1 ]] || echo "$f"
done`

cd $OLDCWD

if [ x"$FILES"x != xx ]; then
    echo "error: found following files that have no newline at the end:"
    for n in $FILES; do
        echo $n
    done
    exit -1
fi
