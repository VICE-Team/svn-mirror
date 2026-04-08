joymapsdir = $(VICE_DATADIR)/joymaps

sdl_joymaps =	sdl_joymap_8bitdo_6b.vjm \
		sdl_joymap_ds3.vjm \
		sdl_joymap_ds4.vjm \
		sdl_joymap_ds4_windows.vjm \
		sdl_joymap_ds5.vjm \
		sdl_joymap_keyrah_v3.vjm \
		sdl_joymap_thec64.vjm \
		sdl_joymap_xbox360.vjm \
		sdl_joymap_xbox_one_windows.vjm

gtk_joymaps =	gtk3_joymap_ds3_DsHidMini.vjm \
		gtk3_joymap_ds3_scp_tools.vjm \
		gtk3_joymap_ds3_Sony.vjm \
		gtk3_joymap_ds3.vjm \
		gtk3_joymap_ds4.vjm \
		gtk3_joymap_ds4_windows.vjm \
		gtk3_joymap_ds5.vjm \
		gtk3_joymap_logitech_x3dpro_windows.vjm \
		gtk3_joymap_xbox_one_windows.vjm

EXTRA_DIST = $(sdl_joymaps) $(gtk_joymaps)
# Do not install joymaps files when configured for headless
if !USE_HEADLESSUI
joymaps_DATA = $(sdl_joymaps) $(gtk_joymaps)
endif
