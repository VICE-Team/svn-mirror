#!/bin/sh

if [ "$1" = "cru" ]; then
  shift
  ar cr $*
else
  ar $*
fi
