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

#include "archdep_cbmfont.h"


/** \fn  int archdep_register_cbmfont(void)
 * \brief    Try to register the CBM font with the OS
 *
 * This tries to use fontconfig on Unix, GDI on windows and CoreText on macOS.
 *
 * \return    bool as int
 */

#ifdef ARCHDEP_OS_OSX

# include <CoreText/CTFontManager.h>

int archdep_register_cbmfont(void)
{
    char *fontPath;
    CFStringRef fontPathStringRef;
    CFURLRef fontUrl;
    CFArrayRef fontUrls;
    CFArrayRef errors;
    unsigned int len;
    unsigned int isdir;

    fontPath = archdep_join_paths(archdep_boot_path(), "..", "lib", "vice", "fonts", "CBM.ttf", NULL);
    if (-1 == archdep_stat(fontPath, &len, &isdir)) {
        lib_free(fontPath);

        log_error(LOG_ERR, "Failed to find CBM.ttf");           
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
    char *datadir;
    char *path;

    if (!FcInit()) {
        return 0;
    }

    fc_config = FcConfigGetCurrent();
    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "..", "fonts", "CBM.ttf", NULL);
    lib_free(datadir);

    if (!FcConfigAppFontAddFile(fc_config, (FcChar8 *)path)) {
        lib_free(path);
        return 0;
    }

    lib_free(path);
    return 1;
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

#  include "windows.h"

int archdep_register_cbmfont(void)
{
    char *datadir;
    char *path;
    int result;

    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "CBM.ttf", NULL);
    lib_free(datadir);

    result = AddFontResourceEx(path, 0, 0);
    if (result == 0) {
        lib_free(path);
        return 0;
    }
    lib_free(path);
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

    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "CBM.ttf", NULL);
    lib_free(datadir);

    RemoveFontResourceEx(path, 0, 0);
    lib_free(path);
#endif
}
