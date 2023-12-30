#!/usr/bin/env python3


# This script interrogates the results of make_vice_git.py and reconstructs
# a sensible set of release and branch tags for it. It runs in four phases:
#
# 1. Creates a comprehensive index of every directory of every commit that
#    looks like it's a copy of VICE.
# 2. Cross-checks them against each other to work out which directories of
#    which commits are actually commits that exist in trunk (and which are
#    thus the 'true' source of those copies). Permit non-release tags to be
#    matched against commits in branches as well.
# 3. For the official releases that do not actually correspond to any commit
#    in trunk (there are many, mostly a legacy of cvs2svn), create new commits
#    that correspond to each such release and arrange them in a branch of
#    their own. For non-release tags that do not correspond to any commit in
#    trunk or any branch, turn them into top-level branches. For branches that
#    hold multiple VICE installs within them, break them out into multiple Git
#    branches.
# 4. Assign Git tags to every commit that corresponds to an old SVN tag. Fix
#    the branch references by removing branches and tags that do not match
#    the intended directory structure, preserving their contents in the
#    branches created in step 3.
#
# TODO: This script is incomplete. Phase 1 is largely done, but needs to be
#       made incremental (it takes forever). Phase 2 is partially done. Phases
#       3 and 4 are yet to be written and, since they alter the repository,
#       cannot sanely be made incremental.


import json
import os
import subprocess
import sys

verbose = False

# ---------- UTILITY FUNCTIONS ----------


# Interrogates the git repository in the current working directory to extract
# all branch names with the given prefix.
def collect_branches(prefix):
    branches = subprocess.run(['git', 'branch', '-a'], check=True, capture_output=True).stdout.decode('ascii').split('\n')
    result = []
    for branch in branches:
        tokens = branch.split()
        if len(tokens) == 0:
            continue
        tag = tokens[-1]
        if not tag.startswith(f"remotes/{prefix}"):
            continue
        tag_hash = get_tree_hash_for_commit(tag[8:])
        tag = tag[len("remotes/"):]
        result.append(tag)
    return result


# Returns a linear list of commit hashes for the given branch. The revisions
# are listed in the same order git log uses, so for SVN-like branches this
# history will be newest first. History halts when a commit in the terminator
# list is found. (This means branches don't all end up recapitulating all of
# trunk before the branch point.)
def create_branch_tree_history(branch, terminators=[]):
    result = subprocess.run(['git', 'log', '--pretty=raw', branch], check=True, capture_output=True)

    commits = []
    termset = set(terminators)

    for line in result.stdout.decode('latin-1').split('\n'):
        tokens = line.split()
        # Can't actually check the tokens themselves here; what if a line in
        # a commit message starts with 'commit'?
        if len(tokens) == 2 and line.startswith("commit "):
            commits.append(tokens[1])
            if tokens[1] in termset:
                break
    return commits


# Turns a commit hash into the hash that represents the actual code in that
# commit. This lets us detect if a subdirectory of one commit is a perfect
# copy of this one. (That happens a lot in our tag system.)
def get_tree_hash_for_commit(commit):
    result = subprocess.run(['git', 'show', '-s', '--pretty=raw', commit], check=True, capture_output=True)
    for line in result.stdout.decode('latin-1').split('\n'):
        if line.startswith("tree "):
            tokens = line.split()
            return tokens[1]
    return None


# Traverse the contents of a commit to find all subdirectories that look like
# copies of VICE. Returns a list of "full hashes" (directories that contain
# a vice/ subdirectory) and of "partial hashes" (the vice/ subdirectories
# themselves). Some tags and branches seem to have copied from vice/ instead
# of from trunk, so the partial hashes will let us find them.
def find_vice_installs_under_commit(commit):
    worklist = []
    full_hashes = {}
    partial_hashes = {}
    top_hash = get_tree_hash_for_commit(commit)
    worklist.append((top_hash, "/"))

    while len(worklist) > 0:
        tree_hash, cwd = worklist.pop()
        tree_contents = [line.split() for line in subprocess.run(['git', 'cat-file', '-p', tree_hash], check=True, capture_output=True).stdout.decode('latin-1').split('\n')]
        ok = False
        for subdir in tree_contents:
            if len(subdir) == 4 and subdir[1] == 'tree' and subdir[3] == 'vice':
                full_hashes[cwd] = tree_hash
                partial_hashes[f"{cwd}vice/"] = subdir[2]
                break
            if len(subdir) == 4 and subdir[1] == 'blob' and subdir[3] == 'README':
                ok = True
                partial_hashes[cwd] = tree_hash
                break
        if ok:
            continue
        for subdir in tree_contents:
            if len(subdir) == 4 and subdir[1] == 'tree' and subdir[3] != 'vice':
                worklist.append((subdir[2], f"{cwd}{subdir[3]}/"))
    return full_hashes, partial_hashes


# ---------- MAIN PROGRAM FUNCTIONS ----------


# Phase 1 interrogates the git repository to collect complete commit
# histories for every branch and tag, and then analyzes each commit to
# collect content-addressable hashes for every VICE version it can find.
# (Especially under the svn/tags hierarchy, many commits have multiple
# versions of VICE installed, so this is not a 1-1 mapping.)
#
# This is an extremely time-consuming process (about 20 minutes on my
# admittedly old-and-cheap dev machine), so the results of this are
# serialized as tag_monster_0.json at the end.
def perform_phase_1():
    repo_data = {}

    # Get the history of the svn/trunk branch. These commits are special
    # because if any other branch leads into trunk, that's the branch point
    # and they have to stop.
    print("Collecting commits for trunk...")
    trunk_commits = create_branch_tree_history('svn/trunk')
    repo_data['svn/trunk'] = trunk_commits

    # Get the history of every other branch. This includes all tags.
    fancy_text = sys.stdout.isatty()
    clear_str = "" 
    branches = [x for x in collect_branches('svn/') if x != 'svn/trunk']
    for i, branch in enumerate(branches):
        if fancy_text:
            new_msg = f"Collecting commits for branch '{branch}' ({i+1}/{len(branches)})..."
            print(f"\r{clear_str}\r{new_msg}", end='', flush=True)
            clear_str = " " * len(new_msg)
        repo_data[branch] = create_branch_tree_history(branch, trunk_commits)

    # Given the existence of branches, many commits are repeated. Remove
    # duplicates so later steps can operate on commits individually, without
    # having to care about where in the history they actually are.
    commits = set()
    for branch in repo_data:
        for commit in repo_data[branch]:
            commits.add(commit)
    print(f"\n{len(commits)} unique commits in repo.")

    # Search each commit for VICE installs and create a comprehensive catalog.
    vice_hashes = {}

    # If a previous run produced a tag_monster list, load it in to save us a
    # lot of tedious tree-hash searching
    try:
        previous_run = json.load(open("tag_monster_0.json"))
        vice_hashes = previous_run['commit_trees']
    except:
        # Cached results are missing or corrupt, actually do the work
        pass
    # Drop our reference to nay previous run; we don't need the old branch
    # history since we refreshed that ourselves.
    previous_run = None
    new_commits = set(x for x in commits if x not in vice_hashes)
    print(f"{len(new_commits)} new commits to search.")

    for i, commit in enumerate(new_commits):
        if fancy_text:
            print(f"\rSearching {commit} for VICE source trees ({i+1}/{len(new_commits)})...", end='', flush=True)
        full, partial = find_vice_installs_under_commit(commit)
        vice_hashes[commit] = {'full': full, 'partial': partial}
    if fancy_text:
        print("done.")

    # Write out the results to a file so we don't have to do all this again.
    o = open("tag_monster_0.json", "wt")
    result = {'branch_history': repo_data, 'commit_trees': vice_hashes}
    print(json.dumps(result, indent=4), file=o)
    o.close()

    return result


# Phase 2 takes the results of phase 1 and cross-indexes all tags and
# branches with trunk. It produces a list of candidate tags that can be
# realized with ordinary git tags targeting specific trunk commits.
# Strangenesses in the history will also be laid out.

def perform_phase_2(index):
    # Construct a map of tree-hash-to-trunk-commit
    print("Content-indexing trunk...")
    trunk_tree_hashes = {}
    trunk_partial_hashes = {}
    trunk_commits = set()
    merged_branches = {}
    copied_tags = {}
    for commit in index['branch_history']['svn/trunk']:
        trunk_commits.add(commit)
        hashes = index['commit_trees'][commit]['full']
        if '/' not in hashes:
            # Something from very early in the cvs2svn process, ignore it
            continue
        trunk_tree_hashes[hashes['/']] = commit
        hashes = index['commit_trees'][commit]['partial']
        trunk_partial_hashes[hashes['/vice/']] = commit

    # Construct per-subbranch histories of each branch and tag
    print("Content-indexing branches and sub-branches...")
    subhistories = {}
    for branch, commits in index['branch_history'].items():
        if branch == 'svn/trunk':
            continue
        for commit in commits:
            hashes = index['commit_trees'][commit]['full']
            for path, branch_hash in hashes.items():
                full_name = f"{branch}{path}"
                if full_name not in subhistories:
                    subhistories[full_name] = {'history': [branch_hash],
                            'svn_branch': branch,
                            'svn_path': path,
                            'partial': False}
                elif subhistories[full_name]['history'][-1] != branch_hash:
                    subhistories[full_name]['history'].append(branch_hash)
            partials = index['commit_trees'][commit]['partial']
            for path, branch_hash in partials.items():
                full_name = f"{branch}{path}"
                if full_name.endswith('/vice/'):
                    # This is part of a full branch
                    continue
                if full_name not in subhistories:
                    print(f"Branch {full_name} is partial", file=sys.stderr)
                    subhistories[full_name] = {'history': [branch_hash],
                            'svn_branch': branch,
                            'svn_path': path,
                            'partial': True}
                elif subhistories[full_name]['history'][-1] != branch_hash:
                    subhistories[full_name]['history'].append(branch_hash)

    # Match up the endpoints, and report anomalies
    for subbranch, branchdata in subhistories.items():
        first_hash = branchdata['history'][-1]
        last_hash = branchdata['history'][0]
        if branchdata['partial']:
            branchdata['branched_from'] = trunk_partial_hashes.get(first_hash)
            branchdata['merged_to'] = trunk_partial_hashes.get(last_hash)
        else:
            branchdata['branched_from'] = trunk_tree_hashes.get(first_hash)
            branchdata['merged_to'] = trunk_tree_hashes.get(last_hash)

    # From the standpoint of the git repository, each svn branch is just a
    # subdirectory of the "real" branch (which is our svn branch category.)
    # The history of each branch is thus a succession of tree hashes, not
    # commits. However, we need those commits in order to get authors, dates,
    # and commit messages, so we build up a list of those too.
    for subbranch, branchdata in subhistories.items():
        # Start at the end of the list and move BACK TO THE FUTURE, recording
        # commits as their trees first match our path's tree. This should
        # cover even cases where a change is backed out after being made.
        svn_history = index['branch_history'][branchdata['svn_branch']]
        svn_subbranch_path = branchdata['svn_path']
        tree_history = branchdata['history']
        tree_index = len(tree_history)-1
        commit_history = []
        for commit in reversed(svn_history):
            if tree_index < 0:
                break
            target = tree_history[tree_index]
            if branchdata['partial']:
                commit_trees = index['commit_trees'][commit]['partial']
            else:
                commit_trees = index['commit_trees'][commit]['full']
            if target == commit_trees.get(svn_subbranch_path, None):
                commit_history.append(commit)
                tree_index -= 1
        if tree_index >= 0:
            print(f"Subbranch f{subbranch} couldn't find commits for all history! {tree_index+1} commits remain.", file=sys.stderr)
        commit_history.reverse()
        branchdata['commit_history'] = commit_history

    # Collect all tags corresponding to releases, and then find the commit,
    # if any, they correspond to.
    version_names = []
    for branch in subhistories:
        # One weird case where v2.4.2 was released wrong
        if branch == 'svn/tags/v2.4/v2.4.1/trunk/':
            version_names.append(((2,4,2), branch))
            continue
        subbranch = branch.split('/')[-1]
        if subbranch == "":
            subbranch = branch.split('/')[-2]
        if subbranch[0] != 'v':
            continue
        try:
            versions = tuple([int(x) for x in subbranch[1:].split('.')])
            version_names.append((versions, branch))
        except ValueError:
            # Some branch off a specific version, doesn't count
            pass
    version_names.sort()

    # Go through version names in order, and find the latest commit in
    # each subhistory that has a corresponding trunk hash. If no such
    # commit exists, and there's only one to begin with, use that.
    # Otherwise, scream for manual overrides.
    release_history = []
    for version_code, branch in version_names:
        git_tag = "v" + ".".join([str(i) for i in version_code])
        commits = subhistories[branch]['history']
        ok = False
        for commit in commits:
            if commit in trunk_tree_hashes:
                release_history.append({'version': git_tag,
                                        'tree_hash': commit,
                                        'trunk_commit': trunk_tree_hashes[commit]})
                ok = True
                break
        if not ok:
            if len(commits) == 1:
                release_history.append({'version': git_tag,
                                        'tree_hash': commits[0],
                                        'trunk_commit': None})
                ok = True
        if not ok:
            print(f"Version {git_tag} needs special attention!")

    return {'branch_histories': subhistories,
            'release_history': release_history}

def parse_user(user):
    (token, rest) = user.split(' ', 1)
    rest = rest.strip()
    email_start = rest.index('<') + 1
    email_end = rest.rindex('>')
    name = rest[:email_start-1].strip()
    email = rest[email_start:email_end].strip()
    time = rest[email_end+1:].strip()
    return {'name': name, 'email': email, 'time': time}


def commit_info(commit):
    data = subprocess.run(['git', 'cat-file', '-p', commit], check=True, capture_output=True).stdout.decode('latin-1')
    (attrs, commit_msg) = data.split('\n\n', 1)
    result = {'commit_msg': commit_msg.strip()}
    for attr in attrs.split('\n'):
        tokens = attr.split()
        if len(tokens) == 0:
            continue
        elif tokens[0] == 'tree':
            result['tree'] = tokens[1]
        elif tokens[0] == 'parent':
            if 'parent' not in result:
                result['parent'] = []
            result['parent'].append(tokens[1])
        elif tokens[0] == 'author' or tokens[0] == 'committer':
            result[tokens[0]] = parse_user(attr)
        else:
            print(f"Unknown commit attribute: {attr}")
    return result

# Phase 3 takes the complete release and branch history and turns these
# into proper branches and tags in their own right. If a release is part
# of trunk already, it will just tag that commit directly. Pre-trunk
# releases will be stacked, in release order, in the "legacy_releases"
# branch and tagged there.
def perform_phase_3(index):
    prev_commit = None
    our_env = os.environ.copy()
    # TODO: These need to be real accounts/addresses for legacy releases
    our_env['GIT_AUTHOR_NAME'] = 'Tag Monster'
    our_env['GIT_AUTHOR_EMAIL'] = 'tagmonster@sf.net'
    our_env['GIT_COMMITTER_NAME'] = 'Tag Monster'
    our_env['GIT_COMMITTER_EMAIL'] = 'tagmonster@sf.net'

    for elt in index['release_history']:
        our_version = elt['version']
        if elt["trunk_commit"] is not None:
            our_commit = elt['trunk_commit']
        else:
            commit_msg = f"Legacy release {elt['version']}"
            cmd = ['git', 'commit-tree', elt['tree_hash']]
            if prev_commit is not None:
                cmd += ['-p', prev_commit]
            if verbose:
                print(" ".join(cmd))
            result = subprocess.run(cmd, input=commit_msg, capture_output=True, encoding="UTF-8", env=our_env, check=True)
            our_commit = result.stdout.strip()
            prev_commit = our_commit
        if verbose:
            print(f"git tag release/{our_version} {our_commit}")
        subprocess.run(['git', 'tag', f"release/{our_version}", our_commit], check=True)
    if verbose:
        print(f"git branch legacy_releases {prev_commit}")
    subprocess.run(['git', 'branch', 'legacy_releases', prev_commit], check=True)

    # Convert partial branches into full branches by synthesizing trees to
    # contain them. This is not a place of honor. No highly esteemed algorithm
    # is implemented here. What is here is dangerous and repulsive to us.
    for branch, branchdata in index['branch_histories'].items():
        if not branchdata['partial']:
            continue
        if verbose:
            print(f"Filling out partial branch {branch}")
        commits = branchdata['history']
        for i in range(len(commits)):
            if verbose:
                print("git read-tree --empty")
            subprocess.run(['git', 'read-tree', '--empty'], check=True)
            if verbose:
                print("git update-index --add --cacheinfo 100644 b80b8142b86d51f9a0322280585dafddc660002b svn-instructions.txt")
            subprocess.run(['git', 'update-index', '--add', '--cacheinfo', '100644', 'b80b8142b86d51f9a0322280585dafddc660002b', 'svn-instructions.txt'], check=True)
            if verbose:
                print(f"git read-tree -i --prefix=vice {commits[i]}")
            subprocess.run(['git', 'read-tree', '-i', '--prefix=vice', commits[i]], check=True)
            if verbose:
                print("git write-tree")
            full_hash = subprocess.run(['git', 'write-tree'], check=True, capture_output=True, encoding="UTF-8").stdout.strip()
            commits[i] = full_hash
        branchdata['partial'] = False
    # Dispose of the evidence that anything happened here, so later high-level
    # operations do not freak out
    if verbose:
        print("git restore --staged .")
    subprocess.run(['git', 'restore', '--staged', '.'])

    # Manually hook up some loose branches to the releases they forked from
    branch_subst = {'svn/amatthies/current/': 'release/v1.22.22',
            'svn/fabrizio/v2.1.3-remove_ui_h_and_videoarch_h_from_generic_code/': 'release/v2.1.3',
            'svn/fabrizio/v2.1-gtkfilefilters/': 'release/v2.1'}

    for branch, branchdata in index['branch_histories'].items():
        # We handled release tags seperately
        if branch.startswith('svn/tags/v') and branch[10].isdigit():
            continue
        # TODO: Handle all other tags as tags if possible
        # Comment out this check if we want to preserve the history of
        # merged branches
        if branchdata['merged_to'] is not None:
            print(f"{branch} was already merged into trunk", file=sys.stderr)
            continue
        prev_commit = branchdata['branched_from']
        if branch in branch_subst:
            prev_commit = subprocess.run(['git', 'merge-base', '--independent', branch_subst[branch]], capture_output=True, check=True, encoding="UTF-8").stdout.strip()

        for tree, commit in zip(reversed(branchdata['history']), reversed(branchdata['commit_history'])):
            info = commit_info(commit)
            # These defaults will control if values are missing
            # TODO: Better defaults
            our_env['GIT_AUTHOR_NAME'] = 'Tag Monster'
            our_env['GIT_AUTHOR_EMAIL'] = 'tagmonster@sf.net'
            if 'GIT_AUTHOR_DATE' in our_env:
                del our_env['GIT_AUTHOR_DATE']
            our_env['GIT_COMMITTER_NAME'] = 'Tag Monster'
            our_env['GIT_COMMITTER_EMAIL'] = 'tagmonster@sf.net'
            if 'GIT_COMMITTER_DATE' in our_env:
                del our_env['GIT_COMMITTER_DATE']
            if 'author' in info:
                our_env['GIT_AUTHOR_NAME'] = info['author']['name']
                our_env['GIT_AUTHOR_EMAIL'] = info['author']['email']
                our_env['GIT_AUTHOR_DATE'] = info['author']['time']
            if 'committer' in info:
                our_env['GIT_COMMITTER_NAME'] = info['committer']['name']
                our_env['GIT_COMMITTER_EMAIL'] = info['committer']['email']
                our_env['GIT_COMMITTER_DATE'] = info['committer']['time']
            commit_msg = info['commit_msg']
            cmd = ['git', 'commit-tree', tree]
            if prev_commit is not None:
                cmd += ['-p', prev_commit]
            else:
                # We'll handle this one later; a rename confused it so we'll rebase it at the end
                if branch != 'svn/marco/v2.1-turkish-danish-intl/':
                    print(f"Warning: Branch {branch} has no detectable start point", file=sys.stderr)
            if verbose:
                print(" ".join(cmd))
            result = subprocess.run(cmd, input=commit_msg, capture_output=True, encoding="UTF-8", env=our_env, check=True)
            our_commit = result.stdout.strip()
            prev_commit = our_commit
        if verbose:
            print(f"git branch {branch[4:-1]} {prev_commit}")
        subprocess.run(['git', 'branch', branch[4:-1], prev_commit], check=True)

    # Handle a weird branch where the history link was cut in SVN
    if verbose:
        print("git rebase --no-keep-empty marco/v2.1-turkish-intl marco/v2.1-turkish-danish-intl")
    subprocess.run(['git', 'rebase', '--no-keep-empty', 'marco/v2.1-turkish-intl', 'marco/v2.1-turkish-danish-intl'], check=True)
    if verbose:
        print("git switch master")
    subprocess.run(['git', 'switch', 'master'], check=True)

if __name__ == '__main__':
    index = perform_phase_1()
    index_2 = perform_phase_2(index)
    # This is fast enough that we don't need special staging data. Still,
    # JSON output is easier to look at.
    # print(json.dumps(index_2, indent=4))
    perform_phase_3(index_2)
