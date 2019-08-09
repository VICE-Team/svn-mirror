# Creating a Windows cross-build system using Fedora Linux

**WARNING**: Work in progress, do not use these instructions yet!


## Test machine setup details

I'll be using a Fedora 30 64-bit VM using VirtualBox (6.0.x). Fedora provides all packages required to cross-compile Gtk applications, unlike Debian. Hopefully we can use this as a guide for a Debian cross-build system by using `alien` for any packages Debian doesn't provide.

The install image I'll be using is the netinstall x86\_64 image:
<https://download.fedoraproject.org/pub/fedora/linux/releases/30/Workstation/x86_64/iso/Fedora-Workstation-netinst-x86_64-30-1.2.iso>

### VM install details

In VBox select 'New machine' and create a Fedora 64-bit VM.

I've changed the following settings from the defaults:

* HDD: 64GB, growing
* 2GB memory, 2 CPU cores (adjust to host capabilities)
* Enable bidirectional for shared clipboard and drag'n'drop (no idea if it'll work)
* Disable 'floppy' in the boot order
* Change chipset to ICH9
* Video memory: 128MB, or whatever the max is
* Graphics controller: VMSVGA, enable 3D
  (I haven't actually gotten VICE's OpenGL to actually work with any driver)
* Network: Bridged

#### Install from ISO

In Vbox attach the ISO and start the VM.

As soon as the Grub shell appears, hit 'Tab' and add 'inst.text' to the boot line and hit 'Enter', this will run the installer in text mode,
Select '2' when given a choice between VNC and text mode.

There should be menu:

* 1) I stuck to EN-US
* 2) Timezone (optional), I used 1,1,1 (Europe/Amsterdam)
* 3) Installation source: already set to 'closest mirror', should be okay
* 4) Software selection, this is important for later.
    Select '2', (Minimal), then 'c' continue
    Then select '8' (C Development Tools and Libraries) and perhaps '38' (Text based internet)
* 5) Install dest: VBox HDD: 2 - use all space
* 6) Network: (optional, defaults to DHCP)
* 7) Set root password to 'vicerulez' or so, 'vice' is too short
* 8) Create normal user
    - 3 - vice
    - 4 - use password
    - 5 - set password (vicerulez)
    - 6 - make Admin
    - 7 - groups: add 'wheel'


Press 'b' to begin install and wait.

Reboot after install (don't forget to remove ISO)

#### Basic setup of the VM

Now login as 'vice'

Optional: set passwords

```
$ su
$ passwd
 # enter 'vice' as password
$ passwd vice
 # enter 'vice' as password
```
Fedora will bitch, but it works.






