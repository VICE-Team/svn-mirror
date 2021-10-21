c128dir = $(VICE_DATADIR)/C128

roms = \
	basic64 \
	basichi \
	basiclo \
	chargde \
	chargen \
	chargfr \
	chargse \
	chargch \
	chargno \
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
	frodo.vpl \
	godot.vpl \
	pc64.vpl \
	vdc_deft.vpl \
	vdc_comp.vpl \
	vice.vpl \
	pepto-ntsc-sony.vpl \
	pepto-ntsc.vpl \
	pepto-pal.vpl \
	pepto-palold.vpl \
	community-colors.vpl \
	colodore.vpl \
	ptoing.vpl \
	deekay.vpl \
	cjam.vpl \
	palette.vpl \
	pixcen.vpl \
	rgb.vpl

gtk3_keymaps = \
	gtk3_sym.vkm \
	gtk3_sym_fr.vkm \
	gtk3_sym_de.vkm \
	gtk3_pos.vkm

gtk3_hotkeys = \
	gtk3-hotkeys.vhk

sdl_keymaps = \
	sdl_sym.vkm \
	sdl_sym_de.vkm \
	sdl_sym_fr.vkm \
	sdl_pos.vkm \
	sdl_v128.vkm

sdl_hotkeys = \
	sdl-hotkeys.vhk

if SDL_COMPILE
c128_DATA = $(roms) $(romsets) $(palettes) $(sdl_keymaps) $(sdl_hotkeys)

EXTRA_DIST = $(c128_DATA) $(gtk3_keymaps) $(gtk3_hotkeys)
else
c128_DATA = $(roms) $(romsets) $(palettes) $(gtk3_keymaps) $(gtk3_hotkeys)

EXTRA_DIST = $(c128_DATA) $(sdl_keymaps) $(sdl_hotkeys)
endif
