#!/bin/sh

echo fixing $1, please wait...
dd if=$1 of=$1.cut bs=1 skip=9
printf '\xce\xfa\xed\xfe\x07\x00\x00\x00\x03' >$1.head
cat $1.head $1.cut >$1.ok
mv $1.ok $1
chmod +x $1
rm $1.head $1.cut
echo $1 has been fixed.
