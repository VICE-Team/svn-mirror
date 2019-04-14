# This script is to be used from icons.sh
#
# @author   Bas Wassink <b.wassink@ziggo.nl>


BEGIN {
    # TODO: initialize a hash map or something similar

    # set field separator to space (probably not required)
    FS=" "
}

# this runs for every line passed
{
    # Match '$VICEICON' [filename] [list-of-sizes]
    result = match($0,
              /\$VICEICON\s+(.[a-zA-Z0-9_-]+)\s+?([0-9]+|svg)?\s+?([0-9]+|svg)?/,
              arr);
    if (result !=0)
        # arr[1] is the filename
        print arr[1], arr[2], arr[3]
}

END {
    # TODO: Export a list of icon-name = [size1, size2, .. sizeN] to be used by
    #       make bindist
    }

