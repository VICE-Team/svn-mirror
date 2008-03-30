/* Define if we want to use aRts */
#undef USE_ARTS

/* Define if you want to use Rober W. McMullen's TextField widget.  */
#undef ENABLE_TEXTFIELD

/* The number of bytes in an unsigned long.  */
#undef SIZEOF_UNSIGNED_LONG

/* The number of bytes in an unsigned int.  */
#undef SIZEOF_UNSIGNED_INT

/* The number of bytes in an unsigned short.  */
#undef SIZEOF_UNSIGNED_SHORT

/* Do we use MITSHM extensions?  */
#undef USE_MITSHM

/* Is libXv available?  */
#undef HAVE_XVIDEO

/* Is libXpm available?  */
#undef HAVE_LIBXPM

/* Do we want to use XFree86 fullscreen extensions?  */
#undef USE_XF86_EXTENSIONS

/* Do we want to use XFree86 VIDMODE ?  */
#undef USE_XF86_VIDMODE_EXT

/* Do we want to use XFree86 DGA1 extensions?  */
#undef USE_XF86_DGA1_EXTENSIONS

/* Do we want to use XFree86 DGA2 extensions?  */
#undef USE_XF86_DGA2_EXTENSIONS

/* Are we using the `readline' library or one replacement?  */
#undef HAVE_READLINE

/* Does the `readline' library support `rl_readline_name'?  */
#undef HAVE_RLNAME

/* Are we using the gnomeui  */
#undef USE_GNOMEUI

/* Do we want to enable RS232 support via ACIA emulation?  */
#undef HAVE_RS232

/* Do we want to enable the ReSID code support?  */
#undef HAVE_RESID

/* Do we support a 1351 mouse?  */
#undef HAVE_MOUSE

/* Do you want to use MIDAS Sound System instead of Allegro for sound
   playback?  */
#undef USE_MIDAS_SOUND

/* Do we have to emulate a joystick?  */
#undef HAS_JOYSTICK

/* Is digital joystick support avaiable?  */
#undef HAS_DIGITAL_JOYSTICK

/* Is a Linux compatible joystick support avaiable?  */
#undef LINUX_JOYSTICK

/* Is a BSD compatible joystick support avaiable?  */
#undef BSD_JOYSTICK

/* Define if this version is unstable.  */
#define UNSTABLE

/* Where do we want to install the executable?  */
#undef PREFIX

/* Is only one canvas supported?  */
#undef HAS_SINGLE_CANVAS

/* Is the GUID lib of DX SDK present? */
#undef HAVE_GUIDLIB

/* Is DWORD defined as long or int in the Windows header files? */
#undef DWORD_IS_LONG

/* Can we use the PNG library? */
#undef HAVE_PNG

/* Can we use the ZLIB compression library? */
#undef HAVE_ZLIB

/* Do we want to use the new color management code? */
#undef USE_COLOR_MANAGEMENT

/* gettext stuff */
#undef ENABLE_NLS
#undef HAVE_CATGETS
#undef HAVE_GETTEXT
#undef HAVE_LC_MESSAGES
#undef HAVE_STPCPY
#undef HAVE_LIBINTL_H
#undef DATADIRNAME
#undef NLS_LOCALEDIR

/* 64bit integer for Win32 performance counter */
#undef HAS_LONGLONG_INTEGER

/* Do we have UnlockResource()? Some mingw32 header do not provide this
   function. */
#undef HAS_UNLOCKRESOURCE

/* Support for OpenCBM (former CBM4Linux) */
#undef HAVE_OPENCBM

/* Support for block device disk image access */
#undef HAVE_RAWDRIVE

