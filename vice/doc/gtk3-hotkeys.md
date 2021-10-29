# Gtk3 UI hotkeys

> This file uses Github flavoured Markdown.
> To process this file, run `pandoc -f gfm -o gtk3-hotkeys.html gtk3-hotkeys.md`

The Gtk3 port of VICE allows setting custom keyboard shortcuts, which we refer
to as hotkeys. These hotkeys can be set either through the user interface or
by editing hotkeys files, which are stored either in the VICE data directory or
in the user's VICE configuration directory.

VICE's hotkeys files are read from VICE's data directory, which we'll refer to
as `$VICEDIR` and the (optional) user's hotkeys will be in the user's VICE
configuration directory, which we'll refer to as `$USERDIR`.

On Unix, `$VICEDIR` will point to `/usr/local/share/vice/`, when using the
default install prefix, and `$USERDIR` will point to `$HOME/.config/vice/`.
On Windows, `$VICEDIR` will point to the root directory of the bindist and
`$USERDIR` will point to `%APPDATA%\vice`, which usually is
`C:\Users\%USERNAME%\AppData\Roaming\vice`.

A custom path can be specified by using the command line interface:
`x64sc -hotkeyfile <some-file>`, or by using the UI (**TODO**).


## Syntax of the hotkeys files

The syntax of the hotkeys files is pretty straightforward, a file can contain
mappings, directives and comments. Leading and trailing whitespace is ignored
by the parser.


### Comments

Comments are started with either `;` or `#` and occupy the rest of the line,
they can also appear inline after a directive or mapping.

For example:
```
# This is a comment

monitor-open    <Alt>h      # This is an inline comment
```


### Directives

Directives are special commands for the parser. They start with **`!`** and are
case-insensitive.


#### !CLEAR

##### Syntax

> ```
> !clear
> ```

Clear all registered hotkeys. Best used as the first directive in the (main)
hotkeys file.


#### !DEBUG

##### Syntax

> ```
> !debug <enable|disable|on|off>
> ```

Enable or disable debugging messages via VICE's log system. Messages will be
prefixed with 'Hotkeys:'. Debugging is disabled by default.


#### !INCLUDE

##### Syntax

> ```
> !include <file>
> ```

Process `<file>` as if its contents were injected into the current file being
processed. This can be used recursively.
The `<file>` argument can be inside quotes (`"`) to be able to use paths or
filenames with spaces in them, and quotes inside quotes can be used by escaping
them with `\`, for example: `!include "foo \"bar\".vhk"`.

##### Special variables

As mentioned before, a few special variables exist to use in the argument to
the `!include` directive: `$VICEDIR` and `$USERDIR`. These can be used to avoid
hardcoded, absolute paths.

For example:
```
!include "$VICEDIR/common/gtk3-hotkeys-drive.vhk"
```
will include `/usr/local/share/vice/common/gtk3-hotkeys-drive.vhk`, assuming
the default install prefix for VICE was used.

#### !UNDEF

##### Syntax

> ```
> !undef [<modifier>..]<keyname>
> ```

Remove a hotkey from whatever action it is mapped to.

For example:
```
!undef      <Alt>r      # Unmap Alt+r from 'restore display'
reset-soft  <Alt>r      # Map Alt+r to soft reset
```


### Hotkey mappings

#### Syntax

> ```
> <action-name>   [<modifier>...]<keyname>
> ```

Create a mapping of a hotkey to an action, where `<action-name>` is a string
refering to an operation triggered by a menu item -- such as toggling Warp Mode,
or attaching a disk to a drive -- followed by a keyname, optionally prefixed
with one or more modifiers.

For example:
```
monitor-open    <Alt>m
settings-open   KP_Divide       # map '/' on the keypad to the settings dialog
edit-paste      <Control><Alt>Insert
```

The key names are case-sensitive and map directly to the symbolic constants
GDK uses, but without the leading `GDK_KEY_` component. In the above example
the string 'Insert' would map to `GDK_KEY_Insert`.

For a list of available symbolic key names, see the `gdkkeysyms.h` header of
the GDK development headers. On a Debian system the file is located at
`/usr/include/gtk-3.0/gdk/gdkkeysyms.h`. It can also be viewed online at
https://gitlab.gnome.org/GNOME/gtk/blob/master/gdk/gdkkeysyms.h


##### List of modifiers

| Windows/Unix    | MacOS          | GDK symbolic constant |
| :-------------- | :------------- | :-------------------- |
| `<Alt>`         | `<Option>`     | `GDK_MOD1_MASK`       |
| `<Control>`     | `<Command>`    | `GDK_CONTROL_MASK`    |
|                 | `<Hyper>`      | `GDK_HYPER_MASK`      |
| `<Shift>`       | `<Shift>`      | `GDK_SHIFT_MASK`      |
| `<Super>`       | `<Super>`      | `GDK_SUPER_MASK`      |


> Please be aware that some modifier+key combinations are either mapped to the
> emulated machine's keyboard -- such as `<Control>1` being mapped to CBM+1 when
> using a positional keymap -- or to the operating system/window manager.
> Mappings using `<Alt>` or `<Alt><Shift>` are usually fine.



##### List of action names

> This list is generated running `./uiactions.py markdown`.

| name                            | description                                         | x64 | x64sc | x64dtv | xscpu64 | x128 | xvic | xplus4 | xpet | xcbm5x0 | xcbm2 |
| :------------------------------ | :-------------------------------------------------- | --- | ----- | ------ | ------- | ---- | ---- | ------ | ---- | ------- | ----- |
| `advance-frame`                 | Advance emulation one frame                         | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `cart-attach`                   | Attach cartridge                                    | yes | yes   | no     | yes     | no   | yes  | yes    | no   | no      | yes   |
| `cart-detach`                   | Detach cartridge                                    | yes | yes   | no     | yes     | no   | yes  | yes    | no   | no      | yes   |
| `cart-freeze`                   | Press cartridge freeze button                       | yes | yes   | no     | yes     | no   | yes  | yes    | no   | no      | yes   |
| `debug-autoplayback-frames`     | Set autoplayback frames                             | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-core-dump-toggle`        | Toggle saving core dump                             | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-cpu-toggle`        | Toggle CPU trace                                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-drive-10-toggle`   | Toggle Drive 10 CPU trace                           | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-drive-11-toggle`   | Toggle Drive 11 CPU trace                           | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-drive-8-toggle`    | Toggle Drive 8 CPU trace                            | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-drive-9-toggle`    | Toggle Drive 9 CPU trace                            | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-iec-toggle`        | Toggle IEC bus trace                                | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-ieee488-toggle`    | Toggle IEEE-488 bus trace                           | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `debug-trace-mode`              | Select CPU/Drive trac mode                          | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-10:0`             | Attach disk to unit 10, drive 0                     | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-10:1`             | Attach disk to unit 10, drive 1                     | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-11:0`             | Attach disk to unit 11, drive 0                     | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-11:1`             | Attach disk to unit 11, drive 1                     | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-8:0`              | Attach disk to unit 8, drive 0                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-8:1`              | Attach disk to unit 8, drive 1                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-9:0`              | Attach disk to unit 9, drive 0                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-attach-9:1`              | Attach disk to unit 9, drive 1                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-create`                  | Create and attach empty disk image                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-10:0`             | Detach disk from unit 10, drive 0                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-10:1`             | Detach disk from unit 10, drive 1                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-11:0`             | Detach disk from unit 11, drive 0                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-11:1`             | Detach disk from unit 11, drive 1                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-8:0`              | Detach disk from unit 8, drive 0                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-8:1`              | Detach disk from unit 8, drive 1                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-9:0`              | Detach disk from unit 9, drive 0                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-9:1`              | Detach disk from unit 9, drive 1                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `drive-detach-all`              | Detach all disks                                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `edit-copy`                     | Copy screen content to clipboard                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `edit-paste`                    | Paste clipboard content into machine                | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-add`                  | Add current disk to fliplist                        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-clear`                | Clear fliplist                                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-load`                 | Load fliplist                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-next`                 | Attach next disk in fliplist                        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-previous`             | Attach previous disk in fliplist                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-remove`               | Remove current disk from fliplist                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fliplist-save`                 | Save fliplist                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fullscreen-decorations-toggle` | Show menu/status in fullscreen                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `fullscreen-toggle`             | Toggle fullscreen                                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `help-about`                    | Show about dialog                                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `help-command-line`             | Command line options                                | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `help-compile-time`             | Compile time features                               | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `help-hotkeys`                  | Hotkeys                                             | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `help-manual`                   | Browse VICE manual                                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-milestone-reset`       | Return to recording milestone                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-milestone-set`         | Set recording milestone                             | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-playback-start`        | Start playing back events                           | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-playback-stop`         | Stop playing back events                            | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-record-start`          | Start recording events                              | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `history-record-stop`           | Stop recording events                               | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `keyset-joystick-enable`        | Allow keyset joystick                               | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `media-record`                  | Record media                                        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `media-stop`                    | Stop media recording                                | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `monitor-open`                  | Open monitor                                        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `mouse-grab-toggle`             | Toggle Mouse Grab                                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `pause-toggle`                  | Toggle Pause                                        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `quit`                          | Quit emulator                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-drive-10`                | Reset drive 10                                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-drive-11`                | Reset drive 11                                      | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-drive-8`                 | Reset drive 8                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-drive-9`                 | Reset drive 9                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-hard`                    | Hard reset the machine                              | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `reset-soft`                    | Soft reset the machine                              | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `restore-display`               | Resize application window to fit contents           | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `screenshot-quicksave`          | Save screenshot in current working directory        | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-default`              | Restore default settings                            | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-dialog`               | Open settings dialog                                | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-load`                 | Load settings                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-load-extra`           | Load additional settings                            | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-load-from`            | Load settings from alternate file                   | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-save`                 | Save settings                                       | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `settings-save-to`              | Save settings to alternate file                     | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `smart-attach`                  | Attach a medium to the emulator inspecting its type | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `snapshot-load`                 | Load snapshot file                                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `snapshot-quickload`            | Quickload snapshot                                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `snapshot-quicksave`            | Quicksave snapshot                                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `snapshot-save`                 | Save snapshot file                                  | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |
| `swap-controlport-toggle`       | Swap controlport joysticks                          | yes | yes   | yes    | yes     | no   | no   | yes    | no   | yes     | no    |
| `tape-attach-1`                 | Attach tape to datasette 1                          | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-attach-2`                 | Attach tape to datasette 2                          | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-create-1`                 | Create tape and attach to datasette 1               | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-create-2`                 | Create tape and attach to datasette 2               | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-detach-1`                 | Detach tape from datasette 1                        | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-detach-2`                 | Detach tape from datasette 2                        | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-ffwd-1`                   | Press FFWD on datasette 1                           | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-ffwd-2`                   | Press FFWD on datasette 2                           | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-play-1`                   | Press PLAY on datasette 1                           | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-play-2`                   | Press PLAY on datasette 2                           | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-record-1`                 | Press RECORD on datasette 1                         | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-record-2`                 | Press RECORD on datasette 2                         | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-reset-1`                  | Reset datasette 1                                   | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-reset-1`                  | Reset datasette 2                                   | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-reset-counter-1`          | Reset datasette 1 counter                           | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-reset-counter-2`          | Reset datasette 2 counter                           | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-rewind-1`                 | Press REWIND on datasette 1                         | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-rewind-2`                 | Press REWIND on datasette 2                         | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `tape-stop-1`                   | Press STOP on datasette 1                           | yes | yes   | no     | no      | yes  | yes  | yes    | yes  | yes     | yes   |
| `tape-stop-2`                   | Press STOP on datasette 2                           | no  | no    | no     | no      | no   | no   | no     | yes  | no      | no    |
| `warp-mode-toggle`              | Toggle Warp Mode                                    | yes | yes   | yes    | yes     | yes  | yes  | yes    | yes  | yes     | yes   |


### Syntax highlighting

Vim syntax highlighting files can be found in doc/vim/. Currently there are two
files: `syntax/vhk.vim` and `ftdetect/vhk.vim`, these can be copied to
`$VIMFILES/` to enable hotkeys syntax highlighting in Vim.

