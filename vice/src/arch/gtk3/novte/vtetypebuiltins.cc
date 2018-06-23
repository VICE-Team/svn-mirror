
/* Generated data (by glib-mkenums) */

#include "novte.h"

/* enumerations from "vteenums.h" */
GType
vte_cursor_blink_mode_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_CURSOR_BLINK_SYSTEM, "VTE_CURSOR_BLINK_SYSTEM", "system" },
      { VTE_CURSOR_BLINK_ON, "VTE_CURSOR_BLINK_ON", "on" },
      { VTE_CURSOR_BLINK_OFF, "VTE_CURSOR_BLINK_OFF", "off" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteCursorBlinkMode"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_cursor_shape_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_CURSOR_SHAPE_BLOCK, "VTE_CURSOR_SHAPE_BLOCK", "block" },
      { VTE_CURSOR_SHAPE_IBEAM, "VTE_CURSOR_SHAPE_IBEAM", "ibeam" },
      { VTE_CURSOR_SHAPE_UNDERLINE, "VTE_CURSOR_SHAPE_UNDERLINE", "underline" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteCursorShape"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_text_blink_mode_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_TEXT_BLINK_NEVER, "VTE_TEXT_BLINK_NEVER", "never" },
      { VTE_TEXT_BLINK_FOCUSED, "VTE_TEXT_BLINK_FOCUSED", "focused" },
      { VTE_TEXT_BLINK_UNFOCUSED, "VTE_TEXT_BLINK_UNFOCUSED", "unfocused" },
      { VTE_TEXT_BLINK_ALWAYS, "VTE_TEXT_BLINK_ALWAYS", "always" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteTextBlinkMode"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_erase_binding_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_ERASE_AUTO, "VTE_ERASE_AUTO", "auto" },
      { VTE_ERASE_ASCII_BACKSPACE, "VTE_ERASE_ASCII_BACKSPACE", "ascii-backspace" },
      { VTE_ERASE_ASCII_DELETE, "VTE_ERASE_ASCII_DELETE", "ascii-delete" },
      { VTE_ERASE_DELETE_SEQUENCE, "VTE_ERASE_DELETE_SEQUENCE", "delete-sequence" },
      { VTE_ERASE_TTY, "VTE_ERASE_TTY", "tty" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteEraseBinding"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_pty_error_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_PTY_ERROR_PTY_HELPER_FAILED, "VTE_PTY_ERROR_PTY_HELPER_FAILED", "pty-helper-failed" },
      { VTE_PTY_ERROR_PTY98_FAILED, "VTE_PTY_ERROR_PTY98_FAILED", "pty98-failed" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VtePtyError"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_pty_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GFlagsValue values[] = {
      { VTE_PTY_NO_LASTLOG, "VTE_PTY_NO_LASTLOG", "no-lastlog" },
      { VTE_PTY_NO_UTMP, "VTE_PTY_NO_UTMP", "no-utmp" },
      { VTE_PTY_NO_WTMP, "VTE_PTY_NO_WTMP", "no-wtmp" },
      { VTE_PTY_NO_HELPER, "VTE_PTY_NO_HELPER", "no-helper" },
      { VTE_PTY_NO_FALLBACK, "VTE_PTY_NO_FALLBACK", "no-fallback" },
      { VTE_PTY_DEFAULT, "VTE_PTY_DEFAULT", "default" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_flags_register_static (g_intern_static_string ("VtePtyFlags"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_write_flags_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_WRITE_DEFAULT, "VTE_WRITE_DEFAULT", "default" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteWriteFlags"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_regex_error_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_REGEX_ERROR_INCOMPATIBLE, "VTE_REGEX_ERROR_INCOMPATIBLE", "incompatible" },
      { VTE_REGEX_ERROR_NOT_SUPPORTED, "VTE_REGEX_ERROR_NOT_SUPPORTED", "not-supported" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteRegexError"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}

GType
vte_format_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const GEnumValue values[] = {
      { VTE_FORMAT_TEXT, "VTE_FORMAT_TEXT", "text" },
      { VTE_FORMAT_HTML, "VTE_FORMAT_HTML", "html" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id = \
       g_enum_register_static (g_intern_static_string ("VteFormat"), values);

    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}



/* Generated data ends here */

