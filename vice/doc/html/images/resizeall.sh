#! /bin/bash

SIZE=500
#SHARP=-unsharp 0.25x0.25+8+0.065
#SHARP=-sharpen 0x0.8
#SHARP2=-sharpen 0x0.8

for N in `ls -1 *layout*.png | grep -v "\-small.png"`; do
    B=`basename -s .png $N`-small.png
    echo "$N to $B"
    convert $N $SHARP -thumbnail $SIZE $SHARP2 $B
done
for N in `ls -1 *keymap*.png | grep -v "\-small.png"`; do
    B=`basename -s .png $N`-small.png
    echo "$N to $B"
    convert $N $SHARP -thumbnail $SIZE $SHARP2 $B
done
