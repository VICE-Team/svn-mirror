c128dir = $(VICEDIR)/C128

roms = \
	basic64 \
	basichi \
	basiclo \
	chargde \
	chargen \
	chargfr \
	chargse \
	chargch \
	kernal \
	kernal64 \
	kernalde \
	kernalfi \
	kernalfr \
	kernalit \
	kernalno \
	kernalse \
	kernalch

romsets = \
	default.vrs

palettes = \
	c64hq.vpl \
	c64s.vpl \
	ccs64.vpl \
	default.vpl \
	frodo.vpl \
	godot.vpl \
	pc64.vpl \
	vdc_deft.vpl \
	vdc_comp.vpl \
	vice.vpl

x11_keymaps = \
	x11_sym.vkm \
	x11_pos.vkm

sdl_keymaps = \
	sdl_sym.vkm

extra_keymaps = \
	amiga_sym.vkm \
	amiga_pos.vkm \
	beos_sym.vkm \
	beos_pos.vkm \
	dos_sym.vkm \
	dos_pos.vkm \
	os2.vkm \
	osx_pos.vkm \
	osx_sym.vkm \
	win_sym.vkm \
	win_pos.vkm

win32_files = \
	win_hotkeys.vhk \
	win_shortcuts.vsc

if SDL_COMPILE
c128_DATA = $(roms) $(romsets) $(palettes) $(sdl_keymaps)

EXTRA_DIST = $(c128_DATA) $(extra_keymaps) $(x11_keymaps) $(win32_files)
else
c128_DATA = $(roms) $(romsets) $(palettes) $(x11_keymaps)

EXTRA_DIST = $(c128_DATA) $(extra_keymaps) $(sdl_keymaps) $(win32_files)
endif
