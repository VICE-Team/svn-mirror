#!/bin/bash
#
# Github actions helper script: compare generated dist tarballs
#
# Author:   Bas Wassink
#
# Requires artifacts from the check-make-dist.yml job in its working directory.
# Extracts the VICE distribution tarballs configured using the different UI
# options (headless, sdl2, gtk3) and compares the file listing of each tarball
# with the others.
#
# Reports "Differences found: [YES|NO]" on stdout, exit status will be 0 when no
# differences where found, 1 when file listings differ and 2 on error.


# Get list of dist names
ls -1 vice-*.tar.gz

dists=$(ls -1 vice-*.tar.gz | sed -n 's/^vice-.*-\(.*\)\.tar.gz$/\1/p')
echo "dists = $dist"
# Dump tarball contents to text files
for dist in $dists; do

    echo -n "Get list of files in $dist tarball: "
    tar --list -zf vice-*-$dist.tar.gz/vice-*.tar.gz | sort > "$dist-files.txt"
    if [ $? -eq 0 ]; then
        echo "OK"
    else
        echo "FAIL"
        exit 2  # script failure
    fi
done

# Run diff on the different combinations of tarballs
echo "Running diff on tarball file lists:"
differs="NO"
for dist in $dists; do
    for other in $dists; do
        if [ "$dist" != "$other" ]; then
            echo ".. $dist and $other:"
            diff $dist-files.txt $other-files.txt
            # diff exit status is 0 when no differences, 1 when differences and
            # 2 on error
            if [ $? -eq 1 ]; then
                differs="YES"
            fi
        fi
    done
done
# Report status:
echo "Differences found: $differs"
if [ "$differs" = "YES" ]; then
    exit 1  # dist tarball file lists differ
else
    exit 0  # all dist tarball file lists are equal
fi
