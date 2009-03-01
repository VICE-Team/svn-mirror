#ifndef DINPUT_HANDLE_H
#define DINPUT_HANDLE_H

#include "config.h"

#ifdef HAVE_DINPUT
#define DIRECTINPUT_VERSION     0x0500
#include <dinput.h>

/* not defined in DirectInput headers prior to 8 */
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL          0x80000000
#endif

LPDIRECTINPUT get_directinput_handle();
#endif

#endif