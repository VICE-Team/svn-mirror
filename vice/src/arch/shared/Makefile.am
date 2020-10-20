# vim: set noet ts=8:
#
# Makefile.am for the shared archdep code

# Set directory constants here as well. Somehow that seems to be the way
# autotools wants us to handle bin/, data/, doc/ and such paths, not via
# config.h.
#
# To properly handle bindists, we'll probably have to use a few branches here
# depending on target and set some vars to relative paths for at least Windows
# and MacOS dmg things.
#
# -- compyx, 2020-02-08
#
AM_CPPFLAGS = \
	@VICE_CPPFLAGS@ \
	@ARCH_INCLUDES@ \
	-I$(top_srcdir)/src/arch/shared \
	$(extra_includes) \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/rs232drv \
	-DVICE_DATADIR='"$(datadir)/$(PACKAGE)"' \
	-DVICE_DOCDIR='"$(docdir)"'

AM_CFLAGS = @VICE_CFLAGS@

AM_CXXFLAGS = @VICE_CXXFLAGS@

AM_LDFLAGS = @VICE_LDFLAGS@


noinst_LIBRARIES = libarchdep.a

libarchdep_a_SOURCES = \
	archdep_boot_path.c \
	archdep_cbmfont.c \
	archdep_create_user_cache_dir.c \
	archdep_create_user_config_dir.c \
	archdep_default_autostart_disk_image_file_name.c \
	archdep_default_fliplist_file_name.c \
	archdep_default_hotkey_file_name.c \
	archdep_default_joymap_file_name.c \
	archdep_default_logger.c \
	archdep_default_resource_file_name.c \
	archdep_default_portable_resource_file_name.c \
	archdep_default_rtc_file_name.c \
	archdep_default_sysfile_pathlist.c \
	archdep_ethernet_available.c \
	archdep_exit.c \
	archdep_expand_path.c \
	archdep_extra_title_text.c \
	archdep_file_exists.c \
	archdep_file_is_blockdev.c \
	archdep_file_is_chardev.c \
	archdep_filename_parameter.c \
	archdep_fix_permissions.c \
	archdep_get_current_drive.c \
	archdep_get_runtime_info.c \
	archdep_get_vice_datadir.c \
	archdep_get_vice_docsdir.c \
	archdep_home_path.c \
	archdep_icon_path.c \
	archdep_is_haiku.c \
	archdep_is_windows_nt.c \
	archdep_is_macos_bindist.c \
	archdep_join_paths.c \
	archdep_kbd_get_host_mapping.c \
	archdep_list_drives.c \
	archdep_make_backup_filename.c \
	archdep_mkdir.c \
	archdep_mkstemp_fd.c \
	archdep_open_default_log_file.c \
	archdep_path_is_relative.c \
	archdep_pref_path.c \
	archdep_program_name.c \
	archdep_program_path.c \
	archdep_quote_parameter.c \
	archdep_quote_unzip.c \
	archdep_rename.c \
	archdep_require_vkbd.c \
	archdep_rmdir.c \
	archdep_rtc_get_centisecond.c \
	archdep_sanitize_filename.c \
	archdep_set_current_drive.c \
	archdep_signals.c \
	archdep_sound.c \
	archdep_spawn.c \
	archdep_startup_log_error.c \
	archdep_stat.c \
	archdep_tmpnam.c \
	archdep_user_cache_path.c \
	archdep_user_config_path.c \
	archdep_usleep.c \
	archdep_xdg.c \
	console.c \
	coproc.c \
	dynlib.c \
	rawnetarch.c \
	rs232dev.c \
	signals.c

EXTRA_DIST = \
	archdep_defs.h \
	archdep_boot_path.h \
	archdep_cbmfont.h \
	archdep_create_user_cache_dir.h \
	archdep_create_user_config_dir.h \
	archdep_default_autostart_disk_image_file_name.h \
	archdep_default_fliplist_file_name.h \
	archdep_default_hotkey_file_name.h \
	archdep_default_joymap_file_name.h \
	archdep_default_logger.h \
	archdep_default_resource_file_name.h \
	archdep_default_portable_resource_file_name.h \
	archdep_default_rtc_file_name.h \
	archdep_default_sysfile_pathlist.h \
	archdep_ethernet_available.h \
	archdep_exit.h \
	archdep_expand_path.h \
	archdep_extra_title_text.h \
	archdep_file_exists.h \
	archdep_file_is_blockdev.h \
	archdep_file_is_chardev.h \
	archdep_filename_parameter.h \
	archdep_fix_permissions.h \
	archdep_get_runtime_info.h \
	archdep_get_vice_datadir.h \
	archdep_get_vice_docsdir.h \
	archdep_home_path.h \
	archdep_icon_path.h \
	archdep_is_haiku.h \
	archdep_is_windows_nt.h \
	archdep_is_macos_bindist.h \
	archdep_join_paths.h \
	archdep_kbd_get_host_mapping.h \
	archdep_make_backup_filename.h \
	archdep_mkdir.h \
	archdep_mkstemp_fd.h \
	archdep_open_default_log_file.h \
	archdep_path_is_relative.h \
	archdep_pref_path.h \
	archdep_program_name.h \
	archdep_program_path.h \
	archdep_quote_parameter.h \
	archdep_quote_unzip.h \
	archdep_rename.h \
	archdep_require_vkbd.h \
	archdep_rmdir.h \
	archdep_rtc_get_centisecond.h \
	archdep_sanitize_filename.h \
	archdep_signals.h \
	archdep_sound.h \
	archdep_spawn.h \
	archdep_startup_log_error.h \
	archdep_stat.h \
	archdep_tmpnam.h \
	archdep_user_cache_path.h \
	archdep_user_config_path.h \
	archdep_unix.h \
	archdep_usleep.h \
	archdep_xdg.h \
	archdep_beos.h \
	archdep_unix.h \
	archdep_win32.h \
	console_none.c \
	console_unix.c \
	coproc.h \
	dynlib-unix.c \
	dynlib-win32.c \
	rawnetarch_unix.c \
	rawnetarch_win32.c \
	rs232-unix-dev.c \
	rs232-win32-dev.c \
	macOS-common-runtime.sh \
	macOS-launcher.sh \
	macOS-redirect-launcher.sh \
	macOS-wrap-ar.sh \
	macOS-wrap-ranlib.sh \
	make-bindist_osx.sh

# FIXME: the Extra MacOS script stuff should live elsewhere
