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


# Find trailing whitespace
#
# For now indicate with '~~~~' at the end
#
function checkwhitespace
{
    echo "finding trailing whitespace:"
# first make a list of files, omitting those we dont want to check
    find -name '*.[ch]' | \
        grep -v '/monitor/mon_parse.c' | \
        grep -v '/monitor/mon_lex.c' | \
        grep -v '/arch/android/AnVICE/' | \
        grep -v '/arch/mingw32-pcap/wpcap/' | \
        grep -v '/arch/gtk3/novte/box_drawing.h' | \
        grep -v "/lib/" > .checkws
# find trailing whitespace
    while IFS= read -r line
    do
        grep -Hn '\s\+$' "$line" | sed 's/\s\+$/~~~~/'
    done < .checkws
    rm -f .checkws
}

function usage
{
    echo "checkstyle.sh - find some codestyle issues"
    echo "usage: ./checkstyle.sh <options>"
    echo "options:"
    echo "comments      find dangling c++ comments"
    echo "tabs          find tabs"
    echo "whitespace    find trailing whitespace"
    echo "all           all of the above"
}

if [ "$#" -eq "0" ]; then
    usage
else
    for thisarg in "$@"
    do
        case "$thisarg" in
            comments)
                    checkcppcomments
                ;;
            tabs)
                    checktabs
                ;;
            whitespace)
                    checkwhitespace
                ;;
            all)
                    checkcppcomments
                    checktabs
                    checkwhitespace
                ;;
            *)
                usage
                ;;
        esac
    done
fi
