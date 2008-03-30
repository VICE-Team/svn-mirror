#!/bin/sh

if [ "$1" = "cru" ]; then
  shift
  mwldppc -xml -o $*
else
  ar $*
fi
