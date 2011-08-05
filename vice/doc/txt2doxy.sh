#! /bin/bash
#
# this script is used to fix up ascii text files for inclusion in the doxygen
# documentation. it replaces some characters by their proper html equivalent,
# inserts line breaks and tries to link urls.
#
# note: this only covers very few things at the moment and should probably get
#       enhanced =)
#
echo "<tt>"
cat $1 | \
    sed -s 's/&/\&amp;/g' |
    sed -s 's/>/\&gt;/g' |
    sed -s 's/</\&lt;/g' |
    sed -s 's:\(http\://[^ )]*\):\<a href=\"\1\"\>\1\</a\>:' |
    sed -s 's/$/<br>/g' |
    sed -s 's/\t/        /g' |
    sed -s 's/  /\&nbsp;\&nbsp;/g'
echo "</tt>"
