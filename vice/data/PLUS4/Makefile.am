plus4dir = $(VICE_DATADIR)/PLUS4

roms = \
	3plus1hi \
	3plus1lo \
	basic \
	kernal \
	kernal.005 \
	kernal.232 \
	kernal.364 \
	c2lo.364

romsets = \
	default.vrs

palettes = \
	yape-pal.vpl \
	yape-ntsc.vpl \
	colodore_ted.vpl

gtk3_hotkeys = \
	gtk3-hotkeys.vhk

gtk3_keymaps = \
	gtk3_pos.vkm \
	gtk3_sym.vkm \
	gtk3_sym_fr.vkm \
	gtk3_sym_de.vkm \
	gtk3_sym_it.vkm \
	gtk3_pos_it.vkm \
	gtk3_c16_pos_it.vkm \
	gtk3_c116_pos_it.vkm

sdl_keymaps = \
	sdl_sym_de.vkm \
	sdl_sym.vkm \
	sdl_pos_fr.vkm \
	sdl_pos.vkm

sdl_hotkeys = \
	sdl-hotkeys.vhk

if SDL_COMPILE
plus4_DATA = $(roms) $(romsets) $(palettes) $(sdl_keymaps) $(sdl_hotkeys)

EXTRA_DIST = $(plus4_DATA) $(gtk3_keymaps) $(gtk3_hotkeys)
else
plus4_DATA = $(roms) $(romsets) $(palettes) $(gtk3_keymaps) $(gtk3_hotkeys)

EXTRA_DIST = $(plus4_DATA) $(sdl_keymaps) $(sdl_hotkeys)
endif
