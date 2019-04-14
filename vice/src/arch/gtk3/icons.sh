#!/bin/sh

#grep -R --include "*.c" '\$VICEICON' .

for f in `find . -name '*.c'`;
do
    # echo "$f"
    cat "$f" | awk -f icons.awk
done
