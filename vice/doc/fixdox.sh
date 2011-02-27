#!/bin/sh
# fixdox.sh - This script will fix up the texi file for proper
#             generation of the final document.
#
# input is stdin and output is stdout.
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# fixdox.sh <format-to-fix-for>
#           $1

FORMAT=$1

fixtxt()
{
}

fixchm()
{
}

fixhlp()
{
}

fixguide()
{
}

fixpdf()
{
}

fixipf()
{
}

if test x"$FORMAT" = "xtxt"; then
  fixtxt()
fi

if test x"$FORMAT" = "xchm"; then
  fixchm()
fi

if test x"$FORMAT" = "xhlp"; then
  fixhlp()
fi

if test x"$FORMAT" = "xguide"; then
  fixguide()
fi

if test x"$FORMAT" = "xpdf"; then
  fixpdf()
fi

if test x"$FORMAT" = "xipf"; then
  fixipf()
fi
