#!/bin/sh
#
# check_obsolete.sh - script for checking for unused/obsolete translation elements.
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# check_obsolete.sh
#

echo "checking for obsolete translation items, this can take a while..."

files=""

# build a list of .c files
cfiles=`find ../src -name "*.c"`

# build a list of .h files
hfiles=`find ../src -name "*.h"`

# get rid of all the crap
for i in $cfiles
do
  case $i in
    *.po.c)
      ;;
    *unix*)
      ;;
    *res*.rc*)
      ;;
    *intl_text.c)
      ;;
    *translate_text.c)
      ;;
    *)
      files="$files $i"
      ;;
  esac
done

# get rid of all the crap
for i in $hfiles
do
  case $i in
    *unix*)
      ;;
    *intl*.h)
      ;;
    *translate*.h)
      ;;
    *)
      files="$files $i"
  esac
done

# generate a file with possible translation ids
rm -f tmp_g.txt
for i in $files
do
  grep >>tmp_g.txt -e IDGS_* -e IDCLS_* -e IDMS_* -e IDMES_* -e IDS_* $i
done

# search through the file for every id and report missing ones
while read data
do
  case ${data%%_*} in
    ID*)
      result=`grep $data tmp_g.txt`
      if test x"$result" = "x"; then
        echo "$data is not used"
      fi
      ;;
  esac
done

rm -f tmp_g.txt
