#!/usr/bin/env python3

import subprocess

def create_trunk_tree_map():
    result = subprocess.run(['git', 'log', '--pretty=raw', 'svn/trunk'], check=True, capture_output=True)

    # Map from tree hashes to their commit under master/trunk
    trunk_map = {}

    current_commit = None
    commit_count = 0
    duplicates = 0

    for line in result.stdout.decode('latin-1').split('\n'):
        tokens = line.split()
        # Can't actually check the tokens themselves here; what if a line in
        # a commit message starts with 'commit' or 'tree'?
        if len(tokens) == 2 and line.startswith("commit "):
            current_commit = tokens[1]
            commit_count += 1
        if len(tokens) == 2 and line.startswith("tree "):
            if current_commit is None:
                raise ValueError("Tree commit found without commit")
            if tokens[1] in trunk_map:
                duplicates += 1
            trunk_map[tokens[1]] = current_commit
            current_commit = None

    # print(f"{len(trunk_map)} unique tree hashes found in {commit_count} commits. {duplicates} repeated hashes.")
    vice_map = {}
    # The code below is way, way too slow to run every time.
    # for tree_hash in trunk_map:
    #     commit_hash = trunk_map[tree_hash]
    #     tree_contents = [line.split() for line in subprocess.run(['git', 'cat-file', '-p', tree_hash], check=True, capture_output=True).stdout.decode('latin-1').split('\n')]
    #     if len(tree_contents) > 10:
    #         print(f"Strange result for {tree_hash}:\n{tree_contents}")
    #         continue
    #     for subdir in tree_contents:
    #         if len(subdir) == 4 and subdir[1] == 'tree' and subdir[3] == 'vice':
    #             vice_map[subdir[2]] = commit_hash
    return trunk_map, vice_map

def get_tree_hash_for_commit(commit):
    result = subprocess.run(['git', 'show', '-s', '--pretty=raw', commit], check=True, capture_output=True)
    for line in result.stdout.decode('latin-1').split('\n'):
        if line.startswith("tree "):
            tokens = line.split()
            return tokens[1]
    return None

def create_tag_tree_map():
    # Step 1: Collect 'tag' branches
    branches = subprocess.run(['git', 'branch', '-a'], check=True, capture_output=True).stdout.decode('ascii').split('\n')
    worklist = []
    true_tags = {}
    src_tags = {}
    for branch in branches:
        tokens = branch.split()
        if len(tokens) == 0:
            continue
        tag = tokens[-1]
        if not tag.startswith("remotes/svn/tags/"):
            continue
        tag_hash = get_tree_hash_for_commit(tag[8:])
        tag = tag[len("remotes/svn/tags/"):]
        worklist.append((tag, tag_hash, "svn/tags"))
    # Step 2: Recurse through them to find the ones that actually have VICE in them
    while len(worklist) > 0:
        tag, tag_hash, history = worklist.pop()
        tree_contents = [line.split() for line in subprocess.run(['git', 'cat-file', '-p', tag_hash], check=True, capture_output=True).stdout.decode('latin-1').split('\n')]
        ok = False
        for subdir in tree_contents:
            if len(subdir) == 4 and subdir[1] == 'tree' and subdir[3] == 'vice':
                ok = True
                # print(f"{history}/{tag} is a real VICE version, hash {tag_hash}")
                if tag in true_tags or tag in src_tags:
                    print(f"- {tag} already in use")
                    true_tags[f"{history}/{tag}"] = tag_hash
                else:
                    true_tags[tag] = tag_hash
                break
            if len(subdir) == 4 and subdir[1] == 'blob' and subdir[3] == 'README':
                ok = True
                # print(f"{history}/{tag} is a partial VICE version, vice-dir hash {tag_hash}")
                if tag in true_tags or tag in src_tags:
                    print(f"- {tag} already in use")
                    src_tags[f"{history}/{tag}"] = tag_hash
                else:
                    src_tags[tag] = tag_hash
                break
        if ok:
            continue
        for subdir in tree_contents:
            if len(subdir) == 4 and subdir[1] == 'tree':
                worklist.append((subdir[3], subdir[2], f"{history}/{tag}"))
    return true_tags, src_tags

print("Creating tag map...", end='', flush=True)
tags, vice_tags = create_tag_tree_map()
print(f"{len(tags)} true tags found, {len(vice_tags)} vice subtags found.\nCreating trunk map...", end='', flush=True)
trunk_hashes, vice_hashes = create_trunk_tree_map()
# print(f"{len(trunk_hashes)} unique hashes found, with {len(vice_hashes)} unique source trees.")
print(f"{len(trunk_hashes)} unique hashes found.")

good = []
bad = []
for tag in tags:
    tree_hash = tags[tag]
    if tree_hash not in trunk_hashes:
        bad.append(tag)
    else:
        good.append(tag)
good.sort()
bad.sort()

print(f"{len(good)} tags have direct trunk representation")
for i in range(len(good)):
    print(f"{i+1:4}. {good[i]}")

print(f"{len(bad)} tags don't")
for i in range(len(bad)):
    print(f"{i+1:4}. {bad[i]}")

