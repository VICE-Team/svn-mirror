#ifndef _VICE_CONFIG_H
#define _VICE_CONFIG_H


/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if the X Window System is missing or not being used.  */
/* #undef X_DISPLAY_MISSING */
#define X_DISPLAY_MISSING

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

/* Define to your X11 display depth.  */
#define X_DISPLAY_DEPTH 0

/* The number of bytes in an unsigned long.  */
#define SIZEOF_UNSIGNED_LONG 4

/* The number of bytes in an unsigned int.  */
#define SIZEOF_UNSIGNED_INT 4

/* The number of bytes in an unsigned short.  */
#define SIZEOF_UNSIGNED_SHORT 2

/* Are we using the `readline' library or one replacement?  */
#define HAVE_READLINE 1

/* Do we want to enable RS232 support via ACIA emulation?  */
#define HAVE_RS232 1

/* Do we want to enable Printer support?  */
#define HAVE_PRINTER 1

/* Do we want to enable the ReSID code support?  */
/* #undef HAVE_RESID */
#define HAVE_RESID 1

/* Package name.  */
#define PACKAGE "vice"

/* Version number.  */
#define VERSION "1.0"

/* The number of bytes in a unsigned int.  */
#define SIZEOF_UNSIGNED_INT 4

/* The number of bytes in a unsigned long.  */
#define SIZEOF_UNSIGNED_LONG 4

/* The number of bytes in a unsigned short.  */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define if you have the atexit function.  */
#define HAVE_ATEXIT 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

#define HAVE_STRERROR 1

#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAS_JOYSTICK 1


#endif
