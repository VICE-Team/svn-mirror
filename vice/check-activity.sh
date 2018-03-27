#!/bin/bash

#
# check-activity.sh - check SVN activity
#
# Written by
#  groepaz <groepaz@gmx.net>
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#  02111-1307  USA.
#

function seperator
{
    echo "--------------------------------------------------------------------------"
}

function cleanup
{
    echo "deleting temporary files..."
    rm -f .svnlog .contributors
}

function updatelog
{
    cleanup
    echo "getting svn log from server... this may take a while"
    svn log -q > .svnlog
}

function catlog
{
    cat .svnlog | grep '^r[0-9]' | grep -v "trikalio" | sed 's:Loggedoubt:loggedoubt:g'
}

function checkrev
{
    if [ ! -f .svnlog ]; then
        updatelog
    fi
    CREV=`svnversion . | sed 's:M::g' | sed 's:S::g' | sed 's:P::g' | sed 's/.*://'`
    LREV=`catlog | sed 's:^r::g' | sort -nr | head -n1 | awk '{print $1}'`
    echo "CREV = $CREV"
    echo "LREV = $LREV"
    if [ "$CREV" -ne "$LREV" ]; then
        echo "WARNING: local log is not up to date, use '$0 update' to update it."
    fi
}

# create a list of all SVN contributors
function findcontributors
{
    if [ ! -f .svnlog ]; then
        updatelog
    fi
    catlog | awk '{print $3}' | sort | uniq > .contributors
}

function showcontributors
{
    findcontributors
    echo "showing all SVN contributors, sorted alphabetically:"
    seperator
    cat .contributors
}

# find last commit of a SVN contributor
function findlastcommit
{
    catlog | sed 's:^r::g' | grep "$1" | sort -nr | head -n1
}

# find last commit of all SVN contributors
function findlastcommits
{
    if [ ! -f .contributors ]; then
        findcontributors
    fi
    while read line; do
        findlastcommit $line
    done < .contributors
}

# show last activity of all contributors
function showlastactivity
{
    echo "showing last activity, sorted by date:"
    seperator
    findlastcommits | sort -nr | awk -F "|" '{printf "r%s %-20s %s\n", $1, $2, $3}'
}

function findcommitnumber
{
    echo `catlog | grep "$1" | wc -l` "$1"
}

function findcommitnumbers
{
    if [ ! -f .contributors ]; then
        findcontributors
    fi
    while read line; do
        findcommitnumber $line
    done < .contributors
}

function showtotalactivity
{
    echo "showing activity, sorted by number of commits:"
    seperator
    findcommitnumbers | sort -nr | awk '{printf "%6d %s\n", $1, $2}'
}

################################################################################
function usage
{
    echo "usage: check-activity.sh <option>"
    echo "where option is one of:"
    echo "update             - update local copy of SVN log"
    echo "contributors       - show alltime SVN contributor list"
    echo "lastactivity       - show last activity of all contributors"
    echo "totalactivity      - show number of all commits for all contributors"
    echo "cleanup            - remove temporary files"
    echo "WARNING: The script will not automatically update the local SVN log, to"
    echo "         do that use the 'update' option manually. This is to save"
    echo "         (a lot of) time on subsequent runs."
    exit
}
################################################################################

case $1 in
    update)
        updatelog ;;
    contributors)
        checkrev
        showcontributors ;;
    lastactivity)
        checkrev
        showlastactivity ;;
    totalactivity)
        checkrev
        showtotalactivity ;;
    cleanup)
        cleanup ;;
    *)
        usage ;;
esac
