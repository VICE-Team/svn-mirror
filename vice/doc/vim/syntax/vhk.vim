" Vim syntax file
" Language: VICE Gtk3 hotkeys
" Maintainer: Bas Wassink
" Latest Revision: 2021-10-06

if exists("b:current_syntax")
    finish
endif

" Keywords, special identifiers
syn match vhkKeyword    "\c!\(DEBUG\|CLEAR\|ENDIF\|INCLUDE\|IFDEF\|\IFNDEF\|UNDEF\)"
syn keyword vhkTodo     TODO FIXME XXX
syn match vhkModifier   "\c<\(alt\|command\|control\|hyper\|option\|shift\|super\)>"
syn match vhkVariable   "\$\(USERDIR\|VICEDIR\)"
syn match vhkDebugArg   "\c\<\(enable\|disable\|on\|off\)\>"

" Action names
syn match vhkActionName "\<advance-frame\>"
syn match vhkActionName "\<cart-attach\>"
syn match vhkActionName "\<cart-detach\>"
syn match vhkActionName "\<cart-freeze\>"
syn match vhkActionName "\<debug-autoplayback-frames\>"
syn match vhkActionName "\<debug-blitter-log-toggle\>"
syn match vhkActionName "\<debug-core-dump-toggle\>"
syn match vhkActionName "\<debug-dma-log-toggle\>"
syn match vhkActionName "\<debug-flash-log-toggle\>"
syn match vhkActionName "\<debug-trace-cpu-toggle\>"
syn match vhkActionName "\<debug-trace-drive-10-toggle\>"
syn match vhkActionName "\<debug-trace-drive-11-toggle\>"
syn match vhkActionName "\<debug-trace-drive-8-toggle\>"
syn match vhkActionName "\<debug-trace-drive-9-toggle\>"
syn match vhkActionName "\<debug-trace-iec-toggle\>"
syn match vhkActionName "\<debug-trace-ieee488-toggle\>"
syn match vhkActionName "\<debug-trace-mode\>"
syn match vhkActionName "\<drive-attach-10:0\>"
syn match vhkActionName "\<drive-attach-10:1\>"
syn match vhkActionName "\<drive-attach-11:0\>"
syn match vhkActionName "\<drive-attach-11:1\>"
syn match vhkActionName "\<drive-attach-8:0\>"
syn match vhkActionName "\<drive-attach-8:1\>"
syn match vhkActionName "\<drive-attach-9:0\>"
syn match vhkActionName "\<drive-attach-9:1\>"
syn match vhkActionName "\<drive-create\>"
syn match vhkActionName "\<drive-detach-10:0\>"
syn match vhkActionName "\<drive-detach-10:1\>"
syn match vhkActionName "\<drive-detach-11:0\>"
syn match vhkActionName "\<drive-detach-11:1\>"
syn match vhkActionName "\<drive-detach-8:0\>"
syn match vhkActionName "\<drive-detach-8:1\>"
syn match vhkActionName "\<drive-detach-9:0\>"
syn match vhkActionName "\<drive-detach-9:1\>"
syn match vhkActionName "\<drive-detach-all\>"
syn match vhkActionName "\<edit-copy\>"
syn match vhkActionName "\<edit-paste\>"
syn match vhkActionName "\<fliplist-add\>"
syn match vhkActionName "\<fliplist-clear\>"
syn match vhkActionName "\<fliplist-load\>"
syn match vhkActionName "\<fliplist-next\>"
syn match vhkActionName "\<fliplist-previous\>"
syn match vhkActionName "\<fliplist-remove\>"
syn match vhkActionName "\<fliplist-save\>"
syn match vhkActionName "\<fullscreen-decorations-toggle\>"
syn match vhkActionName "\<fullscreen-toggle\>"
syn match vhkActionName "\<help-about\>"
syn match vhkActionName "\<help-command-line\>"
syn match vhkActionName "\<help-compile-time\>"
syn match vhkActionName "\<help-hotkeys\>"
syn match vhkActionName "\<help-manual\>"
syn match vhkActionName "\<history-milestone-reset\>"
syn match vhkActionName "\<history-milestone-set\>"
syn match vhkActionName "\<history-playback-start\>"
syn match vhkActionName "\<history-playback-stop\>"
syn match vhkActionName "\<history-record-start\>"
syn match vhkActionName "\<history-record-stop\>"
syn match vhkActionName "\<keyset-joystick-toggle\>"
syn match vhkActionName "\<media-record\>"
syn match vhkActionName "\<media-stop\>"
syn match vhkActionName "\<monitor-open\>"
syn match vhkActionName "\<mouse-grab-toggle\>"
syn match vhkActionName "\<pause-toggle\>"
syn match vhkActionName "\<quit\>"
syn match vhkActionName "\<reset-drive-10\>"
syn match vhkActionName "\<reset-drive-11\>"
syn match vhkActionName "\<reset-drive-8\>"
syn match vhkActionName "\<reset-drive-9\>"
syn match vhkActionName "\<reset-hard\>"
syn match vhkActionName "\<reset-soft\>"
syn match vhkActionName "\<restore-display\>"
syn match vhkActionName "\<screenshot-quicksave\>"
syn match vhkActionName "\<settings-default\>"
syn match vhkActionName "\<settings-dialog\>"
syn match vhkActionName "\<settings-load\>"
syn match vhkActionName "\<settings-load-extra\>"
syn match vhkActionName "\<settings-load-from\>"
syn match vhkActionName "\<settings-save\>"
syn match vhkActionName "\<settings-save-to\>"
syn match vhkActionName "\<smart-attach\>"
syn match vhkActionName "\<snapshot-load\>"
syn match vhkActionName "\<snapshot-quickload\>"
syn match vhkActionName "\<snapshot-quicksave\>"
syn match vhkActionName "\<snapshot-save\>"
syn match vhkActionName "\<swap-controlport-toggle\>"
syn match vhkActionName "\<swap-userport-toggle\>"
syn match vhkActionName "\<tape-attach-1\>"
syn match vhkActionName "\<tape-attach-2\>"
syn match vhkActionName "\<tape-create-1\>"
syn match vhkActionName "\<tape-create-2\>"
syn match vhkActionName "\<tape-detach-1\>"
syn match vhkActionName "\<tape-detach-2\>"
syn match vhkActionName "\<tape-ffwd-1\>"
syn match vhkActionName "\<tape-ffwd-2\>"
syn match vhkActionName "\<tape-play-1\>"
syn match vhkActionName "\<tape-play-2\>"
syn match vhkActionName "\<tape-record-1\>"
syn match vhkActionName "\<tape-record-2\>"
syn match vhkActionName "\<tape-reset-1\>"
syn match vhkActionName "\<tape-reset-1\>"
syn match vhkActionName "\<tape-reset-counter-1\>"
syn match vhkActionName "\<tape-reset-counter-2\>"
syn match vhkActionName "\<tape-rewind-1\>"
syn match vhkActionName "\<tape-rewind-2\>"
syn match vhkActionName "\<tape-stop-1\>"
syn match vhkActionName "\<tape-stop-2\>"
syn match vhkActionName "\<warp-mode-toggle\>"


" Comments
syn match vhkComment "[#;].*$" contains=vhkTodo

" Debug enable/disable
syn match vhkDebug "\c\(!debug\).*$" contains=vhkDebugArg

let b:current_syntax = "vhk"

hi link vhkTodo         Todo
hi link vhkComment      Comment
hi link vhkKeyword      Keyword
hi link vhkModifier     Type
hi link vhkVariable     PreProc
hi link vhkActionName   Special
hi link vhkDebug        Keyword
hi link vhkDebugArg     Type
