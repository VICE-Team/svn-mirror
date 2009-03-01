#include "config.h"
#ifndef HAVE_GUIDLIB
/* all GUIDs will be defined in this object file */
#define INITGUID
/* else all GUIDs will be defined in the external library dxguid.lib */
#endif

/* for CoInitializeEx */
#ifndef HAVE_DINPUT_LIB
#define _WIN32_DCOM
#endif

#include "dinput_handle.h"

#ifdef HAVE_DINPUT

#include "winmain.h"

static LPDIRECTINPUT di = NULL;

LPDIRECTINPUT get_directinput_handle()
{
    HRESULT res;
    if (di == NULL)
    {
#ifdef HAVE_DINPUT_LIB
        if (DirectInputCreate(winmain_instance, 0x0500, &di, NULL) != DI_OK)
            di = NULL;
#else
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

        res = CoCreateInstance(&CLSID_DirectInput, NULL, CLSCTX_INPROC_SERVER, &IID_IDirectInput2A, (PVOID*)&di);
        if (res != S_OK)
        {
            return NULL;
        }
        if (IDirectInput_Initialize(di, winmain_instance, 0x0500) != S_OK)
        {
            IDirectInput_Release(di);
            di = NULL;
        }
#endif
    }
    return di;
}
#endif
