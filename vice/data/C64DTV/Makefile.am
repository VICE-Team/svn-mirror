c64dtvdir = $(VICEDIR)/C64DTV

roms = \
	basic \
	chargen \
	kernal \
	dtvrom.bin

romsets = \
	default.vrs

x11_keymaps = \
	x11_sym.vkm \
	x11_pos.vkm

sdl_keymaps = \
	sdl_pos.vkm \
	sdl_pos_da.vkm \
	sdl_pos_de.vkm \
	sdl_pos_fi.vkm \
	sdl_pos_no.vkm

extra_keymaps = \
	amiga_sym.vkm \
	amiga_pos.vkm \
	beos_sym.vkm \
	beos_pos.vkm \
	dos_sym.vkm \
	dos_pos.vkm \
	os2.vkm \
	os2_sym.vkm \
	osx_pos.vkm \
	osx_sym_de.vkm \
	osx_sym.vkm \
	win_sym_de.vkm \
	win_sym_us.vkm \
	win_pos.vkm

win32_files = \
	win_hotkeys.vhk \
	win_shortcuts.vsc

sym_file = \
	c64mem.sym

if SDL_COMPILE
c64dtv_DATA = $(roms) $(romsets) $(sdl_keymaps) $(sym_file)

EXTRA_DIST = $(c64dtv_DATA) $(extra_keymaps) $(x11_keymaps) $(win32_files)
else
c64dtv_DATA = $(roms) $(romsets) $(x11_keymaps) $(sym_file)

EXTRA_DIST = $(c64dtv_DATA) $(extra_keymaps) $(sdl_keymaps) $(win32_files)
endif
