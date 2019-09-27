# Creating a Gtk3VICE-Windows cross builder using Debian with Fedora packages

**NOTE**: Very much a work in progress, you might risk your mental health =)


## Introduction

The idea is to create a cross-compiler on Debian Stretch that can build Gtk3 Windows binaries, so we can use pokefinder to generate nightly Windows builds.
Unfortunately Debian doesn't provide any mingw64 packages for building Gtk3 applications,so that's where the Fedora packages come in: I've succesfully created a Gtk3 Windows cross-compiler on Fedora 30.

So the idea is this: use Debian's packages whenever possible and fill in the blanks with Fedora packages, either using the `alien` tool, or just brute-force copying files from the Fedora packages until 'it' works.
That's the idea anyway, the result will probably be hell, so be prepared for some 'nasty language' while I'm working on this document.


### Setting up the environment

We need at least a Debian Stretch box, I'll be using a VirtualBox VM so I can take snapshots to easily roll back an mistakes I might make. I might also add a fresh Fedora VM at some point when my current Fedora VM doesn't cut it (ie checking mingw64 package dependencies).

#### Setting up the Debian VM

Download <https://cdimage.debian.org/cdimage/archive/9.11.0/amd64/iso-cd/debian-9.11.0-amd64-netinst.iso>, we'll be using that to install a minimal, headless Debian.

##### Debian VM settings

This should be basic stuff, but here's short list of settings I used:

* General
    * Basic
        * Type: Linux
        * Version: Debian (64-bit)
    * Advanced
        * Shared clipboard: Bidirectional (no idea if this works with non-X11)
        * Drag'n'Drop: Bidirectional (no ideaif this works with non-X11)
* System
    * Motherboard
        * Base memory: 2GB (adjust to host specs, my host has 8GB)
        * Boot order: Optical,HDD (uncheck Floppy and Network)
        * Chipset: ICH9
        * Pointing device: USB Tablet
    * Processor
        * Processor(s): 2 (adjust to host, my host has 4 cores)
* Display
    * Screen
        * Video memory: 128MB
* Storage
    * Storage devices: attach the ISO to IDE


##### Installing Debian on the VM

Do a basic install of Debian with only 'ssh server' and 'standard system utilities' selected, make sure not to select any Desktop stuff.

I'll be using 'vice' for the username, the password for the user and also for the root password, which is handy but not very secure. For internet-facing stuff, better use a password like vice123.

Reboot.

Once rebooted, log in as 'vice', and do

```
$ su
$ apt update
$ apt upgrade
```

Now we'll install a bunch of native development packages:
```sh
$ su
$ apt install autoconf automake build-essential byacc curl flex gettext git \
        subversion xa65
# install the text editor of your choice:
$ apt install vim
# optional:
$ apt remove emacs
$ ln -s `which vim` /usr/bin/emacs
```

Set the default editor for Subversion and Git commits, by adding this to `~/.profile`:
```
export SVN_EDITOR=vim
export GIT_EDITOR=vim
```

