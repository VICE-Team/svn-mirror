
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

