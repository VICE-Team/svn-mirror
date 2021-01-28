/** \file   archdep_cbmfont.c
 * \brief   CBM font handling
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"
#include "archdep_boot_path.h"
#include "archdep_defs.h"
#include "archdep_join_paths.h"
#include "archdep_stat.h"
#include "lib.h"
#include "log.h"
#include "sysfile.h"

#include "archdep_cbmfont.h"


/** \brief  Filename of the TrueType CBM font used for directory display
 */
#define VICE_CBM_FONT_TTF "C64_Pro_Mono-STYLE.ttf"


/** \fn  int archdep_register_cbmfont(void)
 * \brief    Try to register the CBM font with the OS
 *
 * This tries to use fontconfig on Unix, GDI on windows and CoreText on macOS.
 *
 * \return    bool as int
 */

#ifdef ARCHDEP_OS_MACOS

# include <CoreText/CTFontManager.h>

int archdep_register_cbmfont(void)
{
    char *datadir;
    char *fontPath;
    CFStringRef fontPathStringRef;
    CFURLRef fontUrl;
    CFArrayRef fontUrls;
    CFArrayRef errors;
    size_t len;
    unsigned int isdir;

    if (sysfile_locate(VICE_CBM_FONT_TTF, "common", &fontPath) < 0) {
        log_error(LOG_ERR, "failed to find resource data '%s'.",
                VICE_CBM_FONT_TTF);
        return 0;
    }

    fontPathStringRef = CFStringCreateWithCString(NULL, fontPath, kCFStringEncodingUTF8);
    fontUrl = CFURLCreateWithFileSystemPath(NULL, fontPathStringRef, kCFURLPOSIXPathStyle, false);
    fontUrls = CFArrayCreate(NULL, (const void **)&fontUrl, 1, NULL);

    CFRelease(fontPathStringRef);

    if(!CTFontManagerRegisterFontsForURLs(fontUrls, kCTFontManagerScopeProcess, &errors))
    {
        log_error(LOG_ERR, "Failed to register font for file: %s", fontPath);
        CFRelease(fontUrls);
        CFRelease(fontUrl);
        lib_free(fontPath);
        return 0;
    }

    CFRelease(fontUrls);
    CFRelease(fontUrl);
    lib_free(fontPath);
    return 1;
}

#elif defined(ARCHDEP_OS_UNIX)

# ifdef HAVE_FONTCONFIG

#  include <fontconfig/fontconfig.h>

int archdep_register_cbmfont(void)
{
    FcConfig *fc_config;
    int result;
    char *path;

    if (!FcInit()) {
        return 0;
    }

    fc_config = FcConfigGetCurrent();
    if (sysfile_locate(VICE_CBM_FONT_TTF, "common", &path) < 0) {
        log_error(LOG_ERR, "failed to find resource data '%s'.",
                VICE_CBM_FONT_TTF);
        return 0;
    }
#if 0
    printf("Path = '%s'\n", path);
#endif

    result = FcConfigAppFontAddFile(fc_config, (FcChar8 *)path) ? 1 : 0;

    lib_free(path);
    return result;
}

# else     /* HAVE_FONTCONFIG */

int archdep_register_cbmfont(void)
{
    log_error(LOG_ERR, "no fontconfig support, sorry.");
    return 0;
}

# endif

#else   /* UNIX_COMPILE */


/*
 * Windows part of the API
 */

# ifdef ARCHDEP_OS_WINDOWS

/* Make sure AddFontResourceEx prototyped is used in wingdi.h */
#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0500
#else
#  if (_WIN32_WINNT < 0x0500)
#    undef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif
#endif

#  include "windows.h"

int archdep_register_cbmfont(void)
{
    char *path;
    int result;

    if (sysfile_locate(VICE_CBM_FONT_TTF, "common", &path) < 0) {
        log_error(LOG_ERR, "failed to find resource data '%s'.",
                VICE_CBM_FONT_TTF);
        return 0;
    }

    result = AddFontResourceEx(path, FR_PRIVATE, 0);
    lib_free(path);
    if (result == 0) {
        return 0;
    }
    return 1;
}

# else

int archdep_register_cbmfont(void)
{
    /* OS not supported */
   return 0;
}

# endif

#endif

/** \brief  Unregister the CBM font
 *
 * Seems like only on Windows this is actually required.
 */
void archdep_unregister_cbmfont(void)
{
#ifdef ARCHDEP_OS_WINDOWS
    char *datadir;
    char *path;

    if (sysfile_locate(VICE_CBM_FONT_TTF, "common", &path) < 0) {
        log_error(LOG_ERR, "failed to find resource data '%s'.",
                VICE_CBM_FONT_TTF);
        return;
    }

    RemoveFontResourceExA(path, FR_PRIVATE, 0);
    lib_free(path);
#endif
}
