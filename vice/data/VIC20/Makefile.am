vic20dir = $(VICE_DATADIR)/VIC20

roms = \
	basic \
	chargen \
	chargen-jp \
	kernal

romsets = \
	default.vrs

palettes = \
	vice.vpl \
	mike-pal.vpl \
	mike-ntsc.vpl \
	colodore_vic.vpl

gtk3_hotkeys = \
	gtk3-hotkeys.vhk

gtk3_keymaps = \
	gtk3_pos_it.vkm \
	gtk3_sym_it.vkm \
	gtk3_pos_de.vkm \
	gtk3_sym_de.vkm \
	gtk3_sym_fr.vkm \
	gtk3_pos.vkm \
	gtk3_sym.vkm

sdl_keymaps = \
	sdl_sym.vkm \
	sdl_sym_de.vkm \
	sdl_pos.vkm \
	sdl_pos_fr.vkm \
	sdl_pos_fi.vkm

sdl_hotkeys = \
	sdl-hotkeys.vhk

if SDL_COMPILE
vic20_DATA = $(roms) $(romsets) $(palettes) $(sdl_keymaps) $(sdl_hotkeys)

EXTRA_DIST = $(vic20_DATA) $(gtk3_keymaps) $(gtk3_hotkeys)
else
vic20_DATA = $(roms) $(romsets) $(palettes) $(gtk3_keymaps) $(gtk3_hotkeys)

EXTRA_DIST = $(vic20_DATA) $(sdl_keymaps) $(sdl_hotkeys)
endif
