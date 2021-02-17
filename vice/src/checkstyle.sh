#! /bin/bash

# this is a really ugly hacked up script to check some aspects of the
# mandatory coding style in VICE

# find c++ style comments
function checkcppcomments
{
    echo "finding c++ style comments:"
# first make a list of files, omitting those we dont want to check
    find -name '*.[ch]' | \
        grep -v "/arch/android/" | \
        grep -v "/arch/gtk3/novte/" | \
        grep -v "/lib/" | \
        grep -v '/resid/' | \
        grep -v '/resid-dtv/' > .checkcppcomments
# find "//", with some ugly hackery to omit URLs
    while IFS= read -r line
    do
        grep -Hn '//' "$line" | \
            sed -s 's/http:\/\//http:/g' | \
            sed -s 's/https:\/\//https:/g' | \
            sed -s 's/ip4:\/\//ip4:/g' | \
            sed -s 's/ip6:\/\//ip6:/g' | \
            sed -s 's/file:\/\//file:/g' | \
            grep --color '//'
    done < .checkcppcomments
    rm -f .checkcppcomments
}

# find TABs
function checktabs
{
    echo "finding TABs:"
# first make a list of files, omitting those we dont want to check
    find -name '*.[ch]' | \
        grep -v '/monitor/mon_parse.c' | \
        grep -v '/monitor/mon_lex.c' | \
        grep -v '/arch/android/AnVICE/' | \
        grep -v '/arch/mingw32-pcap/wpcap/' | \
        grep -v '/arch/gtk3/novte/box_drawing.h' | \
        grep -v "/lib/" > .checktabs
# find TABs
    while IFS= read -r line
    do
        grep -Hn $'\t' "$line"
    done < .checktabs
    rm -f .checktabs
}

checkcppcomments
checktabs
