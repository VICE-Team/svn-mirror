#!/usr/bin/env python3

# PROTOTYPE SCRIPT for mirroring the SVN repo locally and converting it
# into a git repo.
#
# In ordinary usage, you'll want to select a location outside the VICE
# checkout tree and pass that as the first argument to this script:
#
# ./make_vice_git.py ../../vice-repo-mirror
#
# This will create vice-repo-mirror if necessary, get it up to date with
# all current branches and tags, and then create a Git version of that
# in ../../vice-git
#
# vice-git should be deleted before each run of this, but it's best to
# keep vice-repo-mirror around.
#
# email addresses are synthesized as SF.net addresses and may not be
# real at this point, but it's the best we can do with CVS and SVN.
#
# TODO: Tag migration. Extracting testprogs and techdocs as their own
#       independent repos. Any kind of safety or usability testing.
#       It should not be necessary to delete vice-git if it hasn't
#       otherwise been touched, so that incremental updates are cheap.

import os
import os.path
import subprocess
import sys

SVN_REPO_PATH = None
SVN_REMOTE_PATH = "svn://svn.code.sf.net/p/vice-emu/code"
GIT_TARGET_PATH = None

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <SVN-REPO-DIR> [SVN-REMOTE-URL] [GIT-TARGET-DIR]")
    sys.exit(1)

SVN_REPO_PATH = os.path.realpath(sys.argv[1])
while SVN_REPO_PATH != "" and SVN_REPO_PATH != "/" and os.path.basename(SVN_REPO_PATH) == "":
    SVN_REPO_PATH = os.path.dirname(SVN_REPO_PATH)
if SVN_REPO_PATH == "" or SVN_REPO_PATH == "/":
    print("Error: Repo path may not be empty or root")
    sys.exit(1)

if len(sys.argv) > 2:
    SVN_REMOTE_PATH = sys.argv[2]
if len(sys.argv) > 3:
    GIT_TARGET_PATH = os.path.realpath(sys.argv[3])
else:
    GIT_TARGET_PATH = os.path.dirname(SVN_REPO_PATH) + "/vice-git"

if os.path.exists(GIT_TARGET_PATH):
    print(f"Error: {GIT_TARGET_PATH} already exists")
    sys.exit(1)

def find_program(prog):
    proc = subprocess.run(['which', prog], capture_output=True)
    if proc.returncode == 0:
        return proc.stdout.strip()
    return None

svn = find_program("svn")
svnadmin = find_program("svnadmin")
svnsync = find_program("svnsync")
gitsvn_ok = subprocess.run(['git', 'svn', '--version'], capture_output=True).returncode == 0

ok = True
if svn is not None:
    print(f"svn found at {svn.decode('utf-8')}")
else:
    print("svn NOT FOUND")
    ok = False
if svnadmin is not None:
    print(f"svnadmin found at {svnadmin.decode('utf-8')}")
else:
    print("svnadmin NOT FOUND")
    ok = False
if svnsync is not None:
    print(f"svnsync found at {svnsync.decode('utf-8')}")
else:
    print("svnsync NOT FOUND")
    ok = False
if gitsvn_ok:
    print("git-svn found")
else:
    print("git-svn NOT FOUND")
if not ok:
    print("Some prerequisites are not installed. Make sure SVN and git-svn\nare both installed and try again.")
    sys.exit(1)

repo_url = "file://" + SVN_REPO_PATH
if not os.path.isdir(SVN_REPO_PATH + "/hooks"):
    print(f"{SVN_REPO_PATH} not found, creating")
    subprocess.run([svnadmin, 'create', SVN_REPO_PATH], check=True)

    revprop_file = SVN_REPO_PATH + "/hooks/pre-revprop-change"
    if not os.path.isfile(revprop_file):
        print(f"Creating {revprop_file}")
        f = open(revprop_file, "w")
        f.write("#!/bin/sh\n\nexit 0\n")
        f.close()
        os.chmod(revprop_file, 0o755)

    print(f"Pointing {repo_url} at {SVN_REMOTE_PATH}")
    subprocess.run([svnsync, 'initialize', repo_url, SVN_REMOTE_PATH], check=True)

print(f"Syncing {repo_url} to {SVN_REMOTE_PATH}")
subprocess.run([svnsync, 'sync', repo_url], check=True)

print(f"Extracting author list")
changelog = subprocess.run([svn, 'log', '-q', repo_url], check=True, capture_output=True)
authors = {"(no author)": "(no author) <no_email>"}
for line in changelog.stdout.decode('utf-8').split('\n'):
    if '|' in line:
        author = line.split('|')[1].strip()
        if author not in authors:
            authors[author] = f"{author} <{author}@sf.net>"

print("Author transform list:")
f = open("authors-transform.txt", "w")
for x in authors:
    print(f"{x} = {authors[x]}")
    f.write(f"{x} = {authors[x]}\n")
f.close()

print(f"Creating git edition of SVN repo at {GIT_TARGET_PATH}")
subprocess.run(['git', 'svn', 'clone', repo_url, '--prefix=svn/', '--stdlayout', f'--rewrite-root={SVN_REMOTE_PATH}', '--authors-file', 'authors-transform.txt', GIT_TARGET_PATH], check=True)
