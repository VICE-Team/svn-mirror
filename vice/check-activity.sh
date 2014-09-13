#! /bin/bash

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

# create a list of all SVN contributors
function findcontributors
{
    if [ ! -f .svnlog ]; then
        updatelog
    fi
    cat .svnlog | grep '^r[0-9]' | awk '{print $3}' | sort | uniq > .contributors
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
    cat .svnlog | grep '^r[0-9]' | sed 's:^r::g' | grep "$1" | sort -nr | head -n1
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

################################################################################
function usage
{
    echo "usage: check-activity.sh <option>"
    echo "where option is one of:"
    echo "update             - update local copy of SVN log"
    echo "contributors       - show alltime SVN contributor list"
    echo "lastactivity       - show last activity of all contributors"
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
        showcontributors ;;
    lastactivity)
        showlastactivity ;;
    cleanup)
        cleanup ;;
    *)
        usage ;;
esac
