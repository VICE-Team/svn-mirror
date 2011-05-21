/*
 * crt.c - CRT image handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
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

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "crt.h"
#include "resources.h"
#include "types.h"

#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "actionreplay.h"
#include "actionreplay2.h"
#include "actionreplay3.h"
#include "actionreplay4.h"
#include "atomicpower.h"
#include "c64-generic.h"
#include "c64tpi.h"
#include "comal80.h"
#include "capture.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "diashowmaker.h"
#include "dinamic.h"
#include "easyflash.h"
#include "epyxfastload.h"
#include "exos.h"
#include "expert.h"
#include "final.h"
#include "finalplus.h"
#include "final3.h"
#include "freezeframe.h"
#include "freezemachine.h"
#include "funplay.h"
#include "gamekiller.h"
#include "gs.h"
#include "ide64.h"
#include "isepic.h"
#include "kcs.h"
#include "mach5.h"
#include "magicdesk.h"
#include "magicformel.h"
#include "magicvoice.h"
#include "mikroass.h"
#include "mmc64.h"
#include "mmcreplay.h"
#include "ocean.h"
#include "prophet64.h"
#include "retroreplay.h"
#include "rexep256.h"
#include "rexutility.h"
#include "ross.h"
#include "simonsbasic.h"
#include "stardos.h"
#include "stb.h"
#include "snapshot64.h"
#include "supergames.h"
#include "supersnapshot4.h"
#include "supersnapshot.h"
#include "superexplode5.h"
#include "warpspeed.h"
#include "westermann.h"
#include "zaxxon.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API

/* #define DEBUGCRT */

#ifdef DEBUGCRT
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

/*
 * CRT image "strings".
 */
const char CRT_HEADER[] = "C64 CARTRIDGE   ";
const char CHIP_HEADER[] = "CHIP";

static int crt_read_header(FILE *fd, BYTE *header)
{
    if (fread(header, 0x40, 1, fd) < 1) {
        DBG(("CRT: could not read header\n"));
        fclose(fd);
        return -1;
    }

    if (strncmp((char*)header, CRT_HEADER, 16)) {
        DBG(("CRT: header invalid\n"));
        fclose(fd);
        return -1;
    }

    return 0;
}
/*
    returns -1 on error, else a positive CRT ID
*/
int crt_getid(const char *filename)
{
    BYTE header[0x40];
    FILE *fd;

    fd = fopen(filename, MODE_READ);

    if (fd == NULL) {
        return -1;
    }

    if (crt_read_header(fd, header) == -1) {
        return -1;
    }

    fclose(fd);

    return header[0x17] + header[0x16] * 256;
}

/*
    returns -1 on error, else a positive CRT ID

    FIXME: to simplify this function a little bit, all subfunctions should
           also return the respective CRT ID on success
*/
int crt_attach(const char *filename, BYTE *rawcart)
{
    BYTE header[0x40];
    int rc, new_crttype;
    FILE *fd;

    DBG(("crt_attach: %s\n", filename));

    fd = fopen(filename, MODE_READ);

    if (fd == NULL) {
        return -1;
    }

    if (crt_read_header(fd, header) == -1) {
        return -1;
    }

    new_crttype = (header[0x17] + (header[0x16] * 256));
    if (header[0x17] & 0x80) {
        /* handle our negative test IDs */
        new_crttype -= 0x10000;
    }
    DBG(("crt_attach ID: %d\n", new_crttype));

/*  cart should always be detached. there is no reason for doing fancy checks
    here, and it will cause problems incase a cart MUST be detached before
    attaching another, or even itself. (eg for initialization reasons)

    most obvious reason: attaching a different ROM (software) for the same
    cartridge (hardware) */

    cartridge_detach_image(new_crttype);

    switch (new_crttype) {
        case CARTRIDGE_CRT:
            rc = generic_crt_attach(fd, rawcart);
            if ( rc !=  CARTRIDGE_NONE) {
                new_crttype = rc;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY:
            rc = actionreplay_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            rc = actionreplay2_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            rc = actionreplay3_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            rc = actionreplay4_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ATOMIC_POWER:
            rc = atomicpower_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_CAPTURE:
            rc = capture_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_COMAL80:
            rc = comal80_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_DELA_EP256:
            rc = delaep256_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_DELA_EP64:
            rc = delaep64_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_DELA_EP7x8:
            rc = delaep7x8_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_DIASHOW_MAKER:
            rc = dsm_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_DINAMIC:
            rc = dinamic_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_EASYFLASH:
            rc = easyflash_crt_attach(fd, rawcart, header, filename);
            break;
        case CARTRIDGE_EPYX_FASTLOAD:
            rc = epyxfastload_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_EXOS:
            rc = exos_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_EXPERT:
            rc = expert_crt_attach(fd, rawcart, filename);
            break;
        case CARTRIDGE_FINAL_I:
            rc = final_v1_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_FINAL_III:
            rc = final_v3_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_FINAL_PLUS:
            rc = final_plus_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_FREEZE_FRAME:
            rc = freezeframe_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            rc = freezemachine_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_FUNPLAY:
            rc = funplay_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_GAME_KILLER:
            rc = gamekiller_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_GS:
            rc = gs_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_IDE64:
            rc = ide64_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_IEEE488:
            rc = tpi_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ISEPIC:
            rc = isepic_crt_attach(fd, rawcart, filename);
            break;
        case CARTRIDGE_KCS_POWER:
            rc = kcs_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MACH5:
            rc = mach5_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MAGIC_DESK:
            rc = magicdesk_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            rc = magicformel_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MAGIC_VOICE:
            rc = magicvoice_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            rc = mikroass_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MMC64:
            rc = mmc64_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_MMC_REPLAY:
            rc = mmcreplay_crt_attach(fd, rawcart, filename);
            break;
        case CARTRIDGE_OCEAN:
            rc = ocean_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_P64:
            rc = p64_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_RETRO_REPLAY:
            rc = retroreplay_crt_attach(fd, rawcart, filename);
            break;
        case CARTRIDGE_REX_EP256:
            rc = rexep256_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_REX:
            rc = rex_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ROSS:
            rc = ross_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SIMONS_BASIC:
            rc = simon_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_STARDOS:
            rc = stardos_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SNAPSHOT64:
            rc = snapshot64_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            rc = stb_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SUPER_GAMES:
            rc = supergames_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            rc = supersnapshot_v4_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            rc = supersnapshot_v5_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            rc = se5_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_WARPSPEED:
            rc = warpspeed_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_WESTERMANN:
            rc = westermann_crt_attach(fd, rawcart);
            break;
        case CARTRIDGE_ZAXXON:
            rc = zaxxon_crt_attach(fd, rawcart);
            break;
        default:
            archdep_startup_log_error("unknown CRT ID: %d\n", new_crttype);
            rc = -1;
            break;
    }

    fclose(fd);

    if (rc == -1) {
        DBG(("crt_attach error (%d)\n", rc));
        return -1;
    }
    DBG(("crt_attach return ID: %d\n", new_crttype));
    return new_crttype;
}
