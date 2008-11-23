c64dir = $(VICEDIR)/C64

roms = \
	basic \
	chargen \
	kernal

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
	vice.vpl

x11_keymaps = \
	x11_sym.vkm \
	x11_pos.vkm

extra_keymaps = \
	amiga_sym.vkm \
	amiga_pos.vkm \
	beos_sym.vkm \
	beos_pos.vkm \
	dos_sym.vkm \
	dos_pos.vkm \
	gp2x_sym.vkm \
	gp2x_pos.vkm \
	os2.vkm \
	os2_sym.vkm \
	osx_pos.vkm \
	osx_sym_de.vkm \
	osx_sym_us.vkm \
	ROdflt.vkm \
	win_sym_de.vkm \
	win_sym_us.vkm \
	win_pos.vkm

win32_shortcuts = \
	win_shortcuts.vsc

sym_file = \
	c64mem.sym

c64_DATA = $(roms) $(romsets) $(palettes) $(x11_keymaps) $(sym_file)

EXTRA_DIST = $(c64_DATA) $(extra_keymaps) $(win32_shortcuts)
