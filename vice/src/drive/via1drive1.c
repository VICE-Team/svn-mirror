/*
 * via1drive1.c - VIA1 emulation in the 1541, 1541II, 1571 and 2031 disk drive.
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#define mycpu drive1
#define myclk drive_clk[1]
#define myvia via1d1
#define myvia_init via1d1_init

#define I_MYVIAFL I_VIA1D1FL
#define MYVIA_INT VIA1D1_INT
#define MYVIA_NAME "Drive1Via1"

#define mycpu_rmw_flag drive1_rmw_flag
#define mycpu_int_status drive1_int_status
#define mycpu_alarm_context drive1_alarm_context
#define mycpu_clk_guard drive1_clk_guard

#define reset_myvia reset_via1d1
#define store_myvia store_via1d1
#define read_myvia read_via1d1
#define peek_myvia peek_via1d1

#define myvia_log via1d1_log
#define myvia_signal via1d1_signal
#define myvia_prevent_clk_overflow via1d1_prevent_clk_overflow
#define myvia_read_snapshot_module via1d1_read_snapshot_module
#define myvia_write_snapshot_module via1d1_write_snapshot_module

#include "vice.h"
#include "viacore.h"

#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "viad.h"
#include "parallel.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)

#define	via_set_int		drive1_set_irq
#define	VIA1D1_INT		IK_IRQ

/* #define VIA1D1_TIMER_DEBUG */

static char snap_module_name[] = "VIA1D1";

static int parieee_is_out = 1;    /* 0= listener, 1= talker */

void drive1_parallel_set_atn(int state)
{
    if (drive[1].type == DRIVE_TYPE_2031) {
        via1d1_signal(VIA_SIG_CA1, state ? VIA_SIG_RISE : 0);
        parallel_drv1_set_nrfd( ((!parieee_is_out) && (!(oldpb & 0x02)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01)));
        parallel_drv1_set_ndac( ((!parieee_is_out) && (!(oldpb & 0x04)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01)));
    }
}

static iec_info_t *iec_info;

static void undump_pra(BYTE byte)
{
    iec_info = iec_get_drive_port();
    if (drive[1].type == DRIVE_TYPE_1571) {
        drive_set_1571_sync_factor(byte & 0x20, 1);
        drive_set_1571_side((byte >> 2) & 1, 1);
    } else
    if (drive[1].type == DRIVE_TYPE_2031) {
        parallel_drv1_set_bus(parieee_is_out ? byte : 0);
    }
    if (drive[1].parallel_cable_enabled && (drive[1].type == DRIVE_TYPE_1541
        || drive[1].type == DRIVE_TYPE_1541II))
        parallel_cable_drive1_write(byte, 0);
}

inline static void store_pra(BYTE byte, BYTE oldpa_value, ADDRESS addr)
{
    {
        if (drive[1].type == DRIVE_TYPE_1571) {
            if ((oldpa_value ^ byte) & 0x20)
                drive_set_1571_sync_factor(byte & 0x20, 1);
            if ((oldpa_value ^ byte) & 0x04)
                drive_set_1571_side((byte >> 2) & 1, 1);
        } else
        if (drive[1].type == DRIVE_TYPE_2031) {
            if(parallel_debug) 
                printf("store_pra(byte=%02x, ~byte=%02x)\n",byte, 0xff^byte);
                parallel_drv1_set_bus(parieee_is_out ? byte : 0);
        } else
        if (drive[1].parallel_cable_enabled && (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_1541II))
            parallel_cable_drive1_write(byte,
                                        (((addr == VIA_PRA)
                                        && ((via1d1[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0));
    }
}

static void undump_prb(BYTE byte)
{
    if (iec_info != NULL) {
        iec_info->drive2_data = ~byte;
        iec_info->drive2_bus = (((iec_info->drive2_data << 3) & 0x40)
            | ((iec_info->drive2_data << 6)
            & ((~iec_info->drive2_data ^ iec_info->cpu_bus) << 3) & 0x80));
        iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive2_bus
            & iec_info->drive_bus;
        iec_info->drive2_port = iec_info->drive_port = (((iec_info->cpu_port >> 4) & 0x4)
            | (iec_info->cpu_port >> 7)
            | ((iec_info->cpu_bus << 3) & 0x80));
    } else {
        if (drive[1].type == DRIVE_TYPE_2031) {
            parieee_is_out = byte & 0x10;
            parallel_drv1_set_bus(parieee_is_out ? oldpa : 0);

            parallel_drv1_set_eoi( parieee_is_out && !(byte & 0x08) );
            parallel_drv1_set_dav( parieee_is_out && !(byte & 0x40) );
            parallel_drv1_set_ndac( ((!parieee_is_out) && (!(byte & 0x04)))
                               || (parallel_atn && (!(byte & 0x01)))
                               || ((!parallel_atn) && (byte & 0x01)));
            parallel_drv1_set_nrfd( ((!parieee_is_out) && (!(byte & 0x02)))
                               || (parallel_atn && (!(byte & 0x01)))
                               || ((!parallel_atn) && (byte & 0x01)));
        } else {
            iec_drive1_write(~byte);
        }
    }
}

inline static void store_prb(BYTE byte, BYTE oldpb, ADDRESS addr)
{
    if (byte != oldpb) {
	if (iec_info != NULL) {
	    iec_info->drive2_data = ~byte;
	    iec_info->drive2_bus = (((iec_info->drive2_data << 3) & 0x40)
	        | ((iec_info->drive2_data << 6)
	        & ((~iec_info->drive2_data ^ iec_info->cpu_bus) << 3) & 0x80));
	    iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive2_bus
	        & iec_info->drive_bus;
	    iec_info->drive2_port = iec_info->drive_port = (((iec_info->cpu_port >> 4) & 0x4)
	        | (iec_info->cpu_port >> 7)
	        | ((iec_info->cpu_bus << 3) & 0x80));
       } else 
       if (drive[1].type == DRIVE_TYPE_2031) {
           BYTE tmp = ~byte;
           if(parallel_debug) {
               printf("store_prb(byte=%02x, ~byte=%02x, prb=%02x, ddrb=%02x)\n",
                       byte, tmp, via1d1[VIA_PRB],via1d1[VIA_DDRB]);
               printf("  -> is_out=%d, eoi=%d, dav=%d\n",byte & 0x10, 
                       !(byte & 0x08), !(byte & 0x40));
           }
           parieee_is_out = byte & 0x10;
           parallel_drv1_set_bus(parieee_is_out ? oldpa : 0);

           if ( parieee_is_out ) {
                parallel_drv1_set_eoi( tmp & 0x08 );
                parallel_drv1_set_dav( tmp & 0x40 );
           } else {
                parallel_drv1_set_eoi( 0 );
                parallel_drv1_set_dav( 0 );
           }
            parallel_drv1_set_nrfd( ((!parieee_is_out) && (tmp & 0x02))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01)));
            parallel_drv1_set_ndac( ((!parieee_is_out) && (tmp & 0x04))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01)));
        } else {
	    iec_drive1_write(~byte);
	}
    }
}

static void undump_pcr(BYTE byte)
{
    drive_update_viad2_pcr(byte, &drive[0]);
}

inline static BYTE store_pcr(BYTE byte, ADDRESS addr)
{
    return byte;
}

static void undump_acr(BYTE byte)
{
}

inline void static store_acr(BYTE byte)
{
}

inline void static store_sr(BYTE byte)
{
}

inline void static store_t2l(BYTE byte)
{
}

static void res_via(void)
{
    parallel_drv1_set_ndac(0);
    parallel_drv1_set_nrfd(0);
    parallel_drv1_set_dav(0);
    parallel_drv1_set_eoi(0);
    parallel_drv1_set_bus(0);

    parieee_is_out = 1;

    iec_info = iec_get_drive_port();
    if (iec_info && drive[1].type == DRIVE_TYPE_2031) {
        iec_info->drive2_bus = 0xff;
        iec_info->drive2_data = 0xff;
        iec_info = NULL;
    }
}

inline static BYTE read_pra(ADDRESS addr)
{
    BYTE byte;
    if (drive[1].type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (drive[1].byte_ready_active == 0x6)
            drive_rotate_disk(&drive[1]);
        tmp = (drive[1].byte_ready ? 0 : 0x80)
            | (drive[1].current_half_track == 2 ? 0 : 1);
        return (tmp & ~via1d1[VIA_DDRA])
            | (via1d1[VIA_PRA] & via1d1[VIA_DDRA]);
    }
    if (drive[1].type == DRIVE_TYPE_2031) {
        if (parallel_debug) {
            printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
                   parieee_is_out, parallel_bus, via1d1[VIA_DDRA]);
        }
        byte = parieee_is_out ? 0xff : ~parallel_bus;
        return (byte & ~via1d1[VIA_DDRA]) | (via1d1[VIA_PRA] & via1d1[VIA_DDRA]);
    }
    byte = (drive[1].parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (via1d1[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((via1d1[VIA_PRA] & via1d1[VIA_DDRA])
               | (0xff & ~via1d1[VIA_DDRA])));
    return byte;
}

inline static BYTE read_prb(void)
{
    BYTE byte;
    if (iec_info != NULL) {
	byte = (((via1d1[VIA_PRB] & 0x1a) | iec_info->drive2_port) ^ 0x85) | 0x20;
    } else {
        if (drive[1].type == DRIVE_TYPE_2031) {
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
               printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
                      parieee_is_out, byte, via1d1[VIA_PRB], via1d1[VIA_DDRB]);
           }
           byte = (byte & ~via1d1[VIA_DDRB]) | (via1d1[VIA_PRB] & via1d1[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xff /* 0xfe */;  /* (byte & 3) + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
               if (parallel_debug) {
                   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
               }
           }
           if (parallel_debug) {
               printf("       -> byte=%02x\n", byte);
           }
        } else {
           byte = (((via1d1[VIA_PRB] & 0x1a) | iec_drive1_read()) ^ 0x85) | 0x20;
        }
    }
    return byte;
}

#include "viacore.c"

