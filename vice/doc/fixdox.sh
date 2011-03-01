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
# remove START-INFO-DIR-ENTRY and END-INFO-DIR-ENTRY lines
  while read data
  do
    if test x"$data" != "xSTART-INFO-DIR-ENTRY" -a x"$data" != "xEND-INFO-DIR-ENTRY"; then
      echo "$data"
    else
      if test x"$data" = "xSTART-INFO-DIR-ENTRY"; then
# fix the title
        read data
        header=""
        for i in $data
        do
          if test x"$header" != "x"; then
             header="$header $i"
          fi
          if test x"$header" = "x" -a x"$i" = "x(vice)."; then
             header=" "
          fi
        done
        echo $header
      fi
    fi
  done
}

fixchm()
{
  echo not implemented yet
}

fixhlp()
{
  echo not implemented yet
}

fixguide()
{
  echo not implemented yet
}

fixpdf()
{
  echo not implemented yet
}

fixipf()
{
  echo not implemented yet
}

if test x"$FORMAT" = "xtxt"; then
  fixtxt
fi

if test x"$FORMAT" = "xchm"; then
  fixchm
fi

if test x"$FORMAT" = "xhlp"; then
  fixhlp
fi

if test x"$FORMAT" = "xguide"; then
  fixguide
fi

if test x"$FORMAT" = "xpdf"; then
  fixpdf
fi

if test x"$FORMAT" = "xipf"; then
  fixipf
fi
