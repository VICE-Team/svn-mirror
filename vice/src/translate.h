/*
 * translate.h - Global internationalization routines.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifdef HAS_TRANSLATION

#ifndef _TRANSLATE_H
#define _TRANSLATE_H

extern int current_language_index;
extern char *current_language;

extern int translate_resources_init(void);
extern void translate_resources_shutdown(void);
extern int translate_cmdline_options_init(void);
extern void translate_arch_language_init(void);
extern char *translate_text(int en_resource);
extern int translate_res(int en_resource);


/* GLOBAL STRING ID DEFINITIONS */

/* autostart.c */
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE          0x10000
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_DE       0x10001
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_FR       0x10002
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_IT       0x10003
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_NL       0x10004
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_PL       0x10005
#define IDGS_CANNOT_LOAD_SNAPSHOT_FILE_SV       0x10006

/* debug.c */
#define IDGS_PLAYBACK_ERROR_DIFFERENT           0x10007
#define IDGS_PLAYBACK_ERROR_DIFFERENT_DE        0x10008
#define IDGS_PLAYBACK_ERROR_DIFFERENT_FR        0x10009
#define IDGS_PLAYBACK_ERROR_DIFFERENT_IT        0x1000a
#define IDGS_PLAYBACK_ERROR_DIFFERENT_NL        0x1000b
#define IDGS_PLAYBACK_ERROR_DIFFERENT_PL        0x1000c
#define IDGS_PLAYBACK_ERROR_DIFFERENT_SV        0x1000d

/* event.c */
#define IDGS_CANNOT_CREATE_IMAGE_S              0x1000e
#define IDGS_CANNOT_CREATE_IMAGE_S_DE           0x1000f
#define IDGS_CANNOT_CREATE_IMAGE_S_FR           0x10010
#define IDGS_CANNOT_CREATE_IMAGE_S_IT           0x10011
#define IDGS_CANNOT_CREATE_IMAGE_S_NL           0x10012
#define IDGS_CANNOT_CREATE_IMAGE_S_PL           0x10013
#define IDGS_CANNOT_CREATE_IMAGE_S_SV           0x10014

/* event.c */
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S          0x10015
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_DE       0x10016
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_FR       0x10017
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_IT       0x10018
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_NL       0x10019
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_PL       0x1001a
#define IDGS_CANNOT_WRITE_IMAGE_FILE_S_SV       0x1001b

/* event.c */
#define IDGS_CANNOT_FIND_MAPPED_NAME_S          0x1001c
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_DE       0x1001d
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_FR       0x1001e
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_IT       0x1001f
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_NL       0x10020
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_PL       0x10021
#define IDGS_CANNOT_FIND_MAPPED_NAME_S_SV       0x10022

/* event.c */
#define IDGS_CANT_CREATE_START_SNAP_S           0x10023
#define IDGS_CANT_CREATE_START_SNAP_S_DE        0x10024
#define IDGS_CANT_CREATE_START_SNAP_S_FR        0x10025
#define IDGS_CANT_CREATE_START_SNAP_S_IT        0x10026
#define IDGS_CANT_CREATE_START_SNAP_S_NL        0x10027
#define IDGS_CANT_CREATE_START_SNAP_S_PL        0x10028
#define IDGS_CANT_CREATE_START_SNAP_S_SV        0x10029

/* event.c */
#define IDGS_ERROR_READING_END_SNAP_S           0x1002a
#define IDGS_ERROR_READING_END_SNAP_S_DE        0x1002b
#define IDGS_ERROR_READING_END_SNAP_S_FR        0x1002c
#define IDGS_ERROR_READING_END_SNAP_S_IT        0x1002d
#define IDGS_ERROR_READING_END_SNAP_S_NL        0x1002e
#define IDGS_ERROR_READING_END_SNAP_S_PL        0x1002f
#define IDGS_ERROR_READING_END_SNAP_S_SV        0x10030

/* event.c */
#define IDGS_CANT_CREATE_END_SNAP_S             0x10031
#define IDGS_CANT_CREATE_END_SNAP_S_DE          0x10032
#define IDGS_CANT_CREATE_END_SNAP_S_FR          0x10033
#define IDGS_CANT_CREATE_END_SNAP_S_IT          0x10034
#define IDGS_CANT_CREATE_END_SNAP_S_NL          0x10035
#define IDGS_CANT_CREATE_END_SNAP_S_PL          0x10036
#define IDGS_CANT_CREATE_END_SNAP_S_SV          0x10037

/* event.c */
#define IDGS_CANT_OPEN_END_SNAP_S               0x10038
#define IDGS_CANT_OPEN_END_SNAP_S_DE            0x10039
#define IDGS_CANT_OPEN_END_SNAP_S_FR            0x1003a
#define IDGS_CANT_OPEN_END_SNAP_S_IT            0x1003b
#define IDGS_CANT_OPEN_END_SNAP_S_NL            0x1003c
#define IDGS_CANT_OPEN_END_SNAP_S_PL            0x1003d
#define IDGS_CANT_OPEN_END_SNAP_S_SV            0x1003e

/* event.c */
#define IDGS_CANT_FIND_SECTION_END_SNAP         0x1003f
#define IDGS_CANT_FIND_SECTION_END_SNAP_DE      0x10040
#define IDGS_CANT_FIND_SECTION_END_SNAP_FR      0x10041
#define IDGS_CANT_FIND_SECTION_END_SNAP_IT      0x10042
#define IDGS_CANT_FIND_SECTION_END_SNAP_NL      0x10043
#define IDGS_CANT_FIND_SECTION_END_SNAP_PL      0x10044
#define IDGS_CANT_FIND_SECTION_END_SNAP_SV      0x10045

/* event.c */
#define IDGS_ERROR_READING_START_SNAP_TRIED     0x10046
#define IDGS_ERROR_READING_START_SNAP_TRIED_DE  0x10047
#define IDGS_ERROR_READING_START_SNAP_TRIED_FR  0x10048
#define IDGS_ERROR_READING_START_SNAP_TRIED_IT  0x10049
#define IDGS_ERROR_READING_START_SNAP_TRIED_NL  0x1004a
#define IDGS_ERROR_READING_START_SNAP_TRIED_PL  0x1004b
#define IDGS_ERROR_READING_START_SNAP_TRIED_SV  0x1004c

/* event.c */
#define IDGS_ERROR_READING_START_SNAP           0x1004d
#define IDGS_ERROR_READING_START_SNAP_DE        0x1004e
#define IDGS_ERROR_READING_START_SNAP_FR        0x1004f
#define IDGS_ERROR_READING_START_SNAP_IT        0x10050
#define IDGS_ERROR_READING_START_SNAP_NL        0x10051
#define IDGS_ERROR_READING_START_SNAP_PL        0x10052
#define IDGS_ERROR_READING_START_SNAP_SV        0x10053

/* screenshot.c */
#define IDGS_SORRY_NO_MULTI_RECORDING           0x10054
#define IDGS_SORRY_NO_MULTI_RECORDING_DE        0x10055
#define IDGS_SORRY_NO_MULTI_RECORDING_FR        0x10056
#define IDGS_SORRY_NO_MULTI_RECORDING_IT        0x10057
#define IDGS_SORRY_NO_MULTI_RECORDING_NL        0x10058
#define IDGS_SORRY_NO_MULTI_RECORDING_PL        0x10059
#define IDGS_SORRY_NO_MULTI_RECORDING_SV        0x1005a

/* sound.c */
#define IDGS_RECORD_DEVICE_S_NOT_EXIST          0x1005b
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_DE       0x1005c
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_FR       0x1005d
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_IT       0x1005e
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_NL       0x1005f
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_PL       0x10060
#define IDGS_RECORD_DEVICE_S_NOT_EXIST_SV       0x10061

/* sound.c */
#define IDGS_RECORD_DIFFERENT_PLAYBACK          0x10062
#define IDGS_RECORD_DIFFERENT_PLAYBACK_DE       0x10063
#define IDGS_RECORD_DIFFERENT_PLAYBACK_FR       0x10064
#define IDGS_RECORD_DIFFERENT_PLAYBACK_IT       0x10065
#define IDGS_RECORD_DIFFERENT_PLAYBACK_NL       0x10066
#define IDGS_RECORD_DIFFERENT_PLAYBACK_PL       0x10067
#define IDGS_RECORD_DIFFERENT_PLAYBACK_SV       0x10068

/* sound.c */
#define IDGS_WARNING_RECORDING_REALTIME         0x10069
#define IDGS_WARNING_RECORDING_REALTIME_DE      0x1006a
#define IDGS_WARNING_RECORDING_REALTIME_FR      0x1006b
#define IDGS_WARNING_RECORDING_REALTIME_IT      0x1006c
#define IDGS_WARNING_RECORDING_REALTIME_NL      0x1006d
#define IDGS_WARNING_RECORDING_REALTIME_PL      0x1006e
#define IDGS_WARNING_RECORDING_REALTIME_SV      0x1006f

/* sound.c */
#define IDGS_INIT_FAILED_FOR_DEVICE_S           0x10070
#define IDGS_INIT_FAILED_FOR_DEVICE_S_DE        0x10071
#define IDGS_INIT_FAILED_FOR_DEVICE_S_FR        0x10072
#define IDGS_INIT_FAILED_FOR_DEVICE_S_IT        0x10073
#define IDGS_INIT_FAILED_FOR_DEVICE_S_NL        0x10074
#define IDGS_INIT_FAILED_FOR_DEVICE_S_PL        0x10075
#define IDGS_INIT_FAILED_FOR_DEVICE_S_SV        0x10076

/* sound.c */
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR       0x10077
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_DE    0x10078
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_FR    0x10079
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_IT    0x1007a
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_NL    0x1007b
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_PL    0x1007c
#define IDGS_RECORD_NOT_SUPPORT_SOUND_PAR_SV    0x1007d

/* sound.c */
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED       0x1007e
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_DE    0x1007f
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_FR    0x10080
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_IT    0x10081
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_NL    0x10082
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_PL    0x10083
#define IDGS_WRITE_TO_SOUND_DEVICE_FAILED_SV    0x10084

/* sound.c */
#define IDGS_CANNOT_OPEN_SID_ENGINE             0x10085
#define IDGS_CANNOT_OPEN_SID_ENGINE_DE          0x10086
#define IDGS_CANNOT_OPEN_SID_ENGINE_FR          0x10087
#define IDGS_CANNOT_OPEN_SID_ENGINE_IT          0x10088
#define IDGS_CANNOT_OPEN_SID_ENGINE_NL          0x10089
#define IDGS_CANNOT_OPEN_SID_ENGINE_PL          0x1008a
#define IDGS_CANNOT_OPEN_SID_ENGINE_SV          0x1008b

/* sound.c */
#define IDGS_CANNOT_INIT_SID_ENGINE             0x1008c
#define IDGS_CANNOT_INIT_SID_ENGINE_DE          0x1008d
#define IDGS_CANNOT_INIT_SID_ENGINE_FR          0x1008e
#define IDGS_CANNOT_INIT_SID_ENGINE_IT          0x1008f
#define IDGS_CANNOT_INIT_SID_ENGINE_NL          0x10090
#define IDGS_CANNOT_INIT_SID_ENGINE_PL          0x10091
#define IDGS_CANNOT_INIT_SID_ENGINE_SV          0x10092

/* sound.c */
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT         0x10093
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_DE      0x10094
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_FR      0x10095
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_IT      0x10096
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_NL      0x10097
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_PL      0x10098
#define IDGS_DEVICE_S_NOT_FOUND_SUPPORT_SV      0x10099

/* sound.c */
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE        0x1009a
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_DE     0x1009b
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_FR     0x1009c
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_IT     0x1009d
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_NL     0x1009e
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_PL     0x1009f
#define IDGS_SOUND_BUFFER_OVERFLOW_CYCLE_SV     0x100a0

/* sound.c */
#define IDGS_SOUND_BUFFER_OVERFLOW              0x100a1
#define IDGS_SOUND_BUFFER_OVERFLOW_DE           0x100a2
#define IDGS_SOUND_BUFFER_OVERFLOW_FR           0x100a3
#define IDGS_SOUND_BUFFER_OVERFLOW_IT           0x100a4
#define IDGS_SOUND_BUFFER_OVERFLOW_NL           0x100a5
#define IDGS_SOUND_BUFFER_OVERFLOW_PL           0x100a6
#define IDGS_SOUND_BUFFER_OVERFLOW_SV           0x100a7

/* sound.c */
#define IDGS_CANNOT_FLUSH                       0x100a8
#define IDGS_CANNOT_FLUSH_DE                    0x100a9
#define IDGS_CANNOT_FLUSH_FR                    0x100aa
#define IDGS_CANNOT_FLUSH_IT                    0x100ab
#define IDGS_CANNOT_FLUSH_NL                    0x100ac
#define IDGS_CANNOT_FLUSH_PL                    0x100ad
#define IDGS_CANNOT_FLUSH_SV                    0x100ae

/* sound.c */
#define IDGS_FRAGMENT_PROBLEMS                  0x100af
#define IDGS_FRAGMENT_PROBLEMS_DE               0x100b0
#define IDGS_FRAGMENT_PROBLEMS_FR               0x100b1
#define IDGS_FRAGMENT_PROBLEMS_IT               0x100b2
#define IDGS_FRAGMENT_PROBLEMS_NL               0x100b3
#define IDGS_FRAGMENT_PROBLEMS_PL               0x100b4
#define IDGS_FRAGMENT_PROBLEMS_SV               0x100b5

/* sound.c */
#define IDGS_RUNNING_TOO_SLOW                   0x100b6
#define IDGS_RUNNING_TOO_SLOW_DE                0x100b7
#define IDGS_RUNNING_TOO_SLOW_FR                0x100b8
#define IDGS_RUNNING_TOO_SLOW_IT                0x100b9
#define IDGS_RUNNING_TOO_SLOW_NL                0x100ba
#define IDGS_RUNNING_TOO_SLOW_PL                0x100bb
#define IDGS_RUNNING_TOO_SLOW_SV                0x100bc

/* sound.c */
#define IDGS_STORE_SOUNDDEVICE_FAILED           0x100bd
#define IDGS_STORE_SOUNDDEVICE_FAILED_DE        0x100be
#define IDGS_STORE_SOUNDDEVICE_FAILED_FR        0x100bf
#define IDGS_STORE_SOUNDDEVICE_FAILED_IT        0x100c0
#define IDGS_STORE_SOUNDDEVICE_FAILED_NL        0x100c1
#define IDGS_STORE_SOUNDDEVICE_FAILED_PL        0x100c2
#define IDGS_STORE_SOUNDDEVICE_FAILED_SV        0x100c3

/* c64/c64export.c */
#define IDGS_RESOURCE_S_BLOCKED_BY_S            0x100c4
#define IDGS_RESOURCE_S_BLOCKED_BY_S_DE         0x100c5
#define IDGS_RESOURCE_S_BLOCKED_BY_S_FR         0x100c6
#define IDGS_RESOURCE_S_BLOCKED_BY_S_IT         0x100c7
#define IDGS_RESOURCE_S_BLOCKED_BY_S_NL         0x100c8
#define IDGS_RESOURCE_S_BLOCKED_BY_S_PL         0x100c9
#define IDGS_RESOURCE_S_BLOCKED_BY_S_SV         0x100ca


/* COMMAND-LINE STRING ID DEFINITIONS */

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   c64/psid.c, plus4/plus4-cmdline-options.c */
#define IDCLS_USE_PAL_SYNC_FACTOR               0x11000
#define IDCLS_USE_PAL_SYNC_FACTOR_DE            0x11001
#define IDCLS_USE_PAL_SYNC_FACTOR_FR            0x11002
#define IDCLS_USE_PAL_SYNC_FACTOR_IT            0x11003
#define IDCLS_USE_PAL_SYNC_FACTOR_NL            0x11004
#define IDCLS_USE_PAL_SYNC_FACTOR_PL            0x11005
#define IDCLS_USE_PAL_SYNC_FACTOR_SV            0x11006

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c */
#define IDCLS_USE_NTSC_SYNC_FACTOR              0x11007
#define IDCLS_USE_NTSC_SYNC_FACTOR_DE           0x11008
#define IDCLS_USE_NTSC_SYNC_FACTOR_FR           0x11009
#define IDCLS_USE_NTSC_SYNC_FACTOR_IT           0x1100a
#define IDCLS_USE_NTSC_SYNC_FACTOR_NL           0x1100b
#define IDCLS_USE_NTSC_SYNC_FACTOR_PL           0x1100c
#define IDCLS_USE_NTSC_SYNC_FACTOR_SV           0x1100d

/* c128/c128-cmdline-options.c, c128/functionrom.c
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/rancart.c, c64/reu.c, c64/c64cart.c,
   c64/ide64.c, cbm2/cbm2-cmdline-options.c,
   drive/iec-cmdline-options.c, drive/iec128dcr-cmdline-options.c,
   drive/ieee-cmdline-options.c, drive/tcbm-cmdline-options.c,
   fsdevice/fsdevice-cmdline-options.c, pet/pet-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
#define IDCLS_P_NAME                            0x1100e
#define IDCLS_P_NAME_DE                         0x1100f
#define IDCLS_P_NAME_FR                         0x11010
#define IDCLS_P_NAME_IT                         0x11011
#define IDCLS_P_NAME_NL                         0x11012
#define IDCLS_P_NAME_PL                         0x11013
#define IDCLS_P_NAME_SV                         0x11014

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_INT_KERNEL_NAME           0x11015
#define IDCLS_SPECIFY_INT_KERNEL_NAME_DE        0x11016
#define IDCLS_SPECIFY_INT_KERNEL_NAME_FR        0x11017
#define IDCLS_SPECIFY_INT_KERNEL_NAME_IT        0x11018
#define IDCLS_SPECIFY_INT_KERNEL_NAME_NL        0x11019
#define IDCLS_SPECIFY_INT_KERNEL_NAME_PL        0x1101a
#define IDCLS_SPECIFY_INT_KERNEL_NAME_SV        0x1101b

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_DE_KERNEL_NAME            0x1101c
#define IDCLS_SPECIFY_DE_KERNEL_NAME_DE         0x1101d
#define IDCLS_SPECIFY_DE_KERNEL_NAME_FR         0x1101e
#define IDCLS_SPECIFY_DE_KERNEL_NAME_IT         0x1101f
#define IDCLS_SPECIFY_DE_KERNEL_NAME_NL         0x11020
#define IDCLS_SPECIFY_DE_KERNEL_NAME_PL         0x11021
#define IDCLS_SPECIFY_DE_KERNEL_NAME_SV         0x11022

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_FI_KERNEL_NAME            0x11023
#define IDCLS_SPECIFY_FI_KERNEL_NAME_DE         0x11024
#define IDCLS_SPECIFY_FI_KERNEL_NAME_FR         0x11025
#define IDCLS_SPECIFY_FI_KERNEL_NAME_IT         0x11026
#define IDCLS_SPECIFY_FI_KERNEL_NAME_NL         0x11027
#define IDCLS_SPECIFY_FI_KERNEL_NAME_PL         0x11028
#define IDCLS_SPECIFY_FI_KERNEL_NAME_SV         0x11029

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_FR_KERNEL_NAME            0x1102a
#define IDCLS_SPECIFY_FR_KERNEL_NAME_DE         0x1102b
#define IDCLS_SPECIFY_FR_KERNEL_NAME_FR         0x1102c
#define IDCLS_SPECIFY_FR_KERNEL_NAME_IT         0x1102d
#define IDCLS_SPECIFY_FR_KERNEL_NAME_NL         0x1102e
#define IDCLS_SPECIFY_FR_KERNEL_NAME_PL         0x1102f
#define IDCLS_SPECIFY_FR_KERNEL_NAME_SV         0x11030

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_IT_KERNEL_NAME            0x11031
#define IDCLS_SPECIFY_IT_KERNEL_NAME_DE         0x11032
#define IDCLS_SPECIFY_IT_KERNEL_NAME_FR         0x11033
#define IDCLS_SPECIFY_IT_KERNEL_NAME_IT         0x11034
#define IDCLS_SPECIFY_IT_KERNEL_NAME_NL         0x11035
#define IDCLS_SPECIFY_IT_KERNEL_NAME_PL         0x11036
#define IDCLS_SPECIFY_IT_KERNEL_NAME_SV         0x11037

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_NO_KERNEL_NAME            0x11038
#define IDCLS_SPECIFY_NO_KERNEL_NAME_DE         0x11039
#define IDCLS_SPECIFY_NO_KERNEL_NAME_FR         0x1103a
#define IDCLS_SPECIFY_NO_KERNEL_NAME_IT         0x1103b
#define IDCLS_SPECIFY_NO_KERNEL_NAME_NL         0x1103c
#define IDCLS_SPECIFY_NO_KERNEL_NAME_PL         0x1103d
#define IDCLS_SPECIFY_NO_KERNEL_NAME_SV         0x1103e

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_SV_KERNEL_NAME            0x1103f
#define IDCLS_SPECIFY_SV_KERNEL_NAME_DE         0x11040
#define IDCLS_SPECIFY_SV_KERNEL_NAME_FR         0x11041
#define IDCLS_SPECIFY_SV_KERNEL_NAME_IT         0x11042
#define IDCLS_SPECIFY_SV_KERNEL_NAME_NL         0x11043
#define IDCLS_SPECIFY_SV_KERNEL_NAME_PL         0x11044
#define IDCLS_SPECIFY_SV_KERNEL_NAME_SV         0x11045

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW        0x11046
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_DE     0x11047
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_FR     0x11048
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_IT     0x11049
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_NL     0x1104a
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_PL     0x1104b
#define IDCLS_SPECIFY_BASIC_ROM_NAME_LOW_SV     0x1104c

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH       0x1104d
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_DE    0x1104e
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_FR    0x1104f
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_IT    0x11050
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_NL    0x11051
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_PL    0x11052
#define IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH_SV    0x11053

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME      0x11054
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_DE   0x11055
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_FR   0x11056
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_IT   0x11057
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_NL   0x11058
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_PL   0x11059
#define IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME_SV   0x1105a

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME       0x1105b
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_DE    0x1105c
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_FR    0x1105d
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_IT    0x1105e
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_NL    0x1105f
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_PL    0x11060
#define IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME_SV    0x11061

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME       0x11062
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_DE    0x11063
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_FR    0x11064
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_IT    0x11065
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_NL    0x11066
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_PL    0x11067
#define IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME_SV    0x11068

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME       0x11069
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_DE    0x1106a
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_FR    0x1106b
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_IT    0x1106c
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_NL    0x1106d
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_PL    0x1106e
#define IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME_SV    0x1106f

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME      0x11070
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_DE   0x11071
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_FR   0x11072
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_IT   0x11073
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_NL   0x11074
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_PL   0x11075
#define IDCLS_SPECIFY_C64_MODE_KERNAL_NAME_SV   0x11076

/* c128/c128-cmdline-options.c */
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME       0x11077
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_DE    0x11078
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_FR    0x11079
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_IT    0x1107a
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_NL    0x1107b
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_PL    0x1107c
#define IDCLS_SPECIFY_C64_MODE_BASIC_NAME_SV    0x1107d

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_ENABLE_EMULATOR_ID                0x1107e
#define IDCLS_ENABLE_EMULATOR_ID_DE             0x1107f
#define IDCLS_ENABLE_EMULATOR_ID_FR             0x11080
#define IDCLS_ENABLE_EMULATOR_ID_IT             0x11081
#define IDCLS_ENABLE_EMULATOR_ID_NL             0x11082
#define IDCLS_ENABLE_EMULATOR_ID_PL             0x11083
#define IDCLS_ENABLE_EMULATOR_ID_SV             0x11084

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_DISABLE_EMULATOR_ID               0x11085
#define IDCLS_DISABLE_EMULATOR_ID_DE            0x11086
#define IDCLS_DISABLE_EMULATOR_ID_FR            0x11087
#define IDCLS_DISABLE_EMULATOR_ID_IT            0x11088
#define IDCLS_DISABLE_EMULATOR_ID_NL            0x11089
#define IDCLS_DISABLE_EMULATOR_ID_PL            0x1108a
#define IDCLS_DISABLE_EMULATOR_ID_SV            0x1108b

/* c128/c128-cmdline-options.c */
#define IDCLS_ENABLE_IEE488                     0x1108c
#define IDCLS_ENABLE_IEE488_DE                  0x1108d
#define IDCLS_ENABLE_IEE488_FR                  0x1108e
#define IDCLS_ENABLE_IEE488_IT                  0x1108f
#define IDCLS_ENABLE_IEE488_NL                  0x11090
#define IDCLS_ENABLE_IEE488_PL                  0x11091
#define IDCLS_ENABLE_IEE488_SV                  0x11092

/* c128/c128-cmdline-options.c */
#define IDCLS_DISABLE_IEE488                    0x11093
#define IDCLS_DISABLE_IEE488_DE                 0x11094
#define IDCLS_DISABLE_IEE488_FR                 0x11095
#define IDCLS_DISABLE_IEE488_IT                 0x11096
#define IDCLS_DISABLE_IEE488_NL                 0x11097
#define IDCLS_DISABLE_IEE488_PL                 0x11098
#define IDCLS_DISABLE_IEE488_SV                 0x11099

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
#define IDCLS_P_REVISION                        0x1109a
#define IDCLS_P_REVISION_DE                     0x1109b
#define IDCLS_P_REVISION_FR                     0x1109c
#define IDCLS_P_REVISION_IT                     0x1109d
#define IDCLS_P_REVISION_NL                     0x1109e
#define IDCLS_P_REVISION_PL                     0x1109f
#define IDCLS_P_REVISION_SV                     0x110a0

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
#define IDCLS_PATCH_KERNAL_TO_REVISION          0x110a1
#define IDCLS_PATCH_KERNAL_TO_REVISION_DE       0x110a2
#define IDCLS_PATCH_KERNAL_TO_REVISION_FR       0x110a3
#define IDCLS_PATCH_KERNAL_TO_REVISION_IT       0x110a4
#define IDCLS_PATCH_KERNAL_TO_REVISION_NL       0x110a5
#define IDCLS_PATCH_KERNAL_TO_REVISION_PL       0x110a6
#define IDCLS_PATCH_KERNAL_TO_REVISION_SV       0x110a7

#ifdef HAVE_RS232
/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU        0x110a8
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_DE     0x110a9
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_FR     0x110aa
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_IT     0x110ab
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_NL     0x110ac
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_PL     0x110ad
#define IDCLS_ENABLE_DEXX_ACIA_RS232_EMU_SV     0x110ae

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c */
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU       0x110af
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_DE    0x110b0
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_FR    0x110b1
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_IT    0x110b2
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_NL    0x110b3
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_PL    0x110b4
#define IDCLS_DISABLE_DEXX_ACIA_RS232_EMU_SV    0x110b5
#endif

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, c64/c64-cmdline-optionc.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
#define IDCLS_P_NUMBER                          0x110b6
#define IDCLS_P_NUMBER_DE                       0x110b7
#define IDCLS_P_NUMBER_FR                       0x110b8
#define IDCLS_P_NUMBER_IT                       0x110b9
#define IDCLS_P_NUMBER_NL                       0x110ba
#define IDCLS_P_NUMBER_PL                       0x110bb
#define IDCLS_P_NUMBER_SV                       0x110bc

/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX         0x110bd
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE      0x110be
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR      0x110bf
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT      0x110c0
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL      0x110c1
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL      0x110c2
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV      0x110c3

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options,
   plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME      0x110c4
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE   0x110c5
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR   0x110c6
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT   0x110c7
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL   0x110c8
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL   0x110c9
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV   0x110ca

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME      0x110cb
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_DE   0x110cc
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_FR   0x110cd
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_IT   0x110ce
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_NL   0x110cf
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_PL   0x110d0
#define IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME_SV   0x110d1
#endif

/* c128/c128mmu.c */
#define IDCLS_ACTIVATE_40_COL_MODE              0x110d2
#define IDCLS_ACTIVATE_40_COL_MODE_DE           0x110d3
#define IDCLS_ACTIVATE_40_COL_MODE_FR           0x110d4
#define IDCLS_ACTIVATE_40_COL_MODE_IT           0x110d5
#define IDCLS_ACTIVATE_40_COL_MODE_NL           0x110d6
#define IDCLS_ACTIVATE_40_COL_MODE_PL           0x110d7
#define IDCLS_ACTIVATE_40_COL_MODE_SV           0x110d8

/* c128/c128mmu.c */
#define IDCLS_ACTIVATE_80_COL_MODE              0x110d9
#define IDCLS_ACTIVATE_80_COL_MODE_DE           0x110da
#define IDCLS_ACTIVATE_80_COL_MODE_FR           0x110db
#define IDCLS_ACTIVATE_80_COL_MODE_IT           0x110dc
#define IDCLS_ACTIVATE_80_COL_MODE_NL           0x110dd
#define IDCLS_ACTIVATE_80_COL_MODE_PL           0x110de
#define IDCLS_ACTIVATE_80_COL_MODE_SV           0x110df

/* c128/functionrom.c */
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME         0x110e0
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_DE      0x110e1
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_FR      0x110e2
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_IT      0x110e3
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_NL      0x110e4
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_PL      0x110e5
#define IDCLS_SPECIFY_INT_FUNC_ROM_NAME_SV      0x110e6

/* c128/functionrom.c */
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME         0x110e7
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_DE      0x110e8
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_FR      0x110e9
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_IT      0x110ea
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_NL      0x110eb
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_PL      0x110ec
#define IDCLS_SPECIFY_EXT_FUNC_ROM_NAME_SV      0x110ed

/* c128/functionrom.c */
#define IDCLS_ENABLE_INT_FUNC_ROM               0x110ee
#define IDCLS_ENABLE_INT_FUNC_ROM_DE            0x110ef
#define IDCLS_ENABLE_INT_FUNC_ROM_FR            0x110f0
#define IDCLS_ENABLE_INT_FUNC_ROM_IT            0x110f1
#define IDCLS_ENABLE_INT_FUNC_ROM_NL            0x110f2
#define IDCLS_ENABLE_INT_FUNC_ROM_PL            0x110f3
#define IDCLS_ENABLE_INT_FUNC_ROM_SV            0x110f4

/* c128/functionrom.c */
#define IDCLS_DISABLE_INT_FUNC_ROM              0x110f5
#define IDCLS_DISABLE_INT_FUNC_ROM_DE           0x110f6
#define IDCLS_DISABLE_INT_FUNC_ROM_FR           0x110f7
#define IDCLS_DISABLE_INT_FUNC_ROM_IT           0x110f8
#define IDCLS_DISABLE_INT_FUNC_ROM_NL           0x110f9
#define IDCLS_DISABLE_INT_FUNC_ROM_PL           0x110fa
#define IDCLS_DISABLE_INT_FUNC_ROM_SV           0x110fb

/* c128/functionrom.c */
#define IDCLS_ENABLE_EXT_FUNC_ROM               0x110fc
#define IDCLS_ENABLE_EXT_FUNC_ROM_DE            0x110fd
#define IDCLS_ENABLE_EXT_FUNC_ROM_FR            0x110fe
#define IDCLS_ENABLE_EXT_FUNC_ROM_IT            0x110ff
#define IDCLS_ENABLE_EXT_FUNC_ROM_NL            0x11100
#define IDCLS_ENABLE_EXT_FUNC_ROM_PL            0x11101
#define IDCLS_ENABLE_EXT_FUNC_ROM_SV            0x11102

/* c128/functionrom.c */
#define IDCLS_DISABLE_EXT_FUNC_ROM              0x11103
#define IDCLS_DISABLE_EXT_FUNC_ROM_DE           0x11104
#define IDCLS_DISABLE_EXT_FUNC_ROM_FR           0x11105
#define IDCLS_DISABLE_EXT_FUNC_ROM_IT           0x11106
#define IDCLS_DISABLE_EXT_FUNC_ROM_NL           0x11107
#define IDCLS_DISABLE_EXT_FUNC_ROM_PL           0x11108
#define IDCLS_DISABLE_EXT_FUNC_ROM_SV           0x11109

/* c64/c64-cmdline-options.c */
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR          0x1110a
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_DE       0x1110b
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_FR       0x1110c
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_IT       0x1110d
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_NL       0x1110e
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_PL       0x1110f
#define IDCLS_USE_OLD_NTSC_SYNC_FACTOR_SV       0x11110

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_KERNAL_ROM_NAME           0x11111
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_DE        0x11112
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_FR        0x11113
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_IT        0x11114
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_NL        0x11115
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_PL        0x11116
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_SV        0x11117

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_BASIC_ROM_NAME            0x11118
#define IDCLS_SPECIFY_BASIC_ROM_NAME_DE         0x11119
#define IDCLS_SPECIFY_BASIC_ROM_NAME_FR         0x1111a
#define IDCLS_SPECIFY_BASIC_ROM_NAME_IT         0x1111b
#define IDCLS_SPECIFY_BASIC_ROM_NAME_NL         0x1111c
#define IDCLS_SPECIFY_BASIC_ROM_NAME_PL         0x1111d
#define IDCLS_SPECIFY_BASIC_ROM_NAME_SV         0x1111e

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME          0x1111f
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_DE       0x11120
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_FR       0x11121
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_IT       0x11122
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_NL       0x11123
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_PL       0x11124
#define IDCLS_SPECIFY_CHARGEN_ROM_NAME_SV       0x11125

#ifdef COMMON_KBD
/* c64/c64-cmdline-options.c */
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2     0x11126
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_DE  0x11127
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_FR  0x11128
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_IT  0x11129
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_NL  0x1112a
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_PL  0x1112b
#define IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2_SV  0x1112c

/* c64/c64-cmdline-options.c */
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP        0x1112d
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_DE     0x1112e
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_FR     0x1112f
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_IT     0x11130
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_NL     0x11131
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_PL     0x11132
#define IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP_SV     0x11133
#endif

/* c64/georam.c */
#define IDCLS_ENABLE_GEORAM                     0x11134
#define IDCLS_ENABLE_GEORAM_DE                  0x11135
#define IDCLS_ENABLE_GEORAM_FR                  0x11136
#define IDCLS_ENABLE_GEORAM_IT                  0x11137
#define IDCLS_ENABLE_GEORAM_NL                  0x11138
#define IDCLS_ENABLE_GEORAM_PL                  0x11139
#define IDCLS_ENABLE_GEORAM_SV                  0x1113a

/* c64/georam.c */
#define IDCLS_DISABLE_GEORAM                    0x1113b
#define IDCLS_DISABLE_GEORAM_DE                 0x1113c
#define IDCLS_DISABLE_GEORAM_FR                 0x1113d
#define IDCLS_DISABLE_GEORAM_IT                 0x1113e
#define IDCLS_DISABLE_GEORAM_NL                 0x1113f
#define IDCLS_DISABLE_GEORAM_PL                 0x11140
#define IDCLS_DISABLE_GEORAM_SV                 0x11141

/* c64/georam.c */
#define IDCLS_SPECIFY_GEORAM_NAME               0x11142
#define IDCLS_SPECIFY_GEORAM_NAME_DE            0x11143
#define IDCLS_SPECIFY_GEORAM_NAME_FR            0x11144
#define IDCLS_SPECIFY_GEORAM_NAME_IT            0x11145
#define IDCLS_SPECIFY_GEORAM_NAME_NL            0x11146
#define IDCLS_SPECIFY_GEORAM_NAME_PL            0x11147
#define IDCLS_SPECIFY_GEORAM_NAME_SV            0x11148

/* c64/georam.c */
#define IDCLS_P_SIZE_IN_KB                      0x11149
#define IDCLS_P_SIZE_IN_KB_DE                   0x1114a
#define IDCLS_P_SIZE_IN_KB_FR                   0x1114b
#define IDCLS_P_SIZE_IN_KB_IT                   0x1114c
#define IDCLS_P_SIZE_IN_KB_NL                   0x1114d
#define IDCLS_P_SIZE_IN_KB_PL                   0x1114e
#define IDCLS_P_SIZE_IN_KB_SV                   0x1114f

/* c64/georam.c */
#define IDCLS_GEORAM_SIZE                       0x11150
#define IDCLS_GEORAM_SIZE_DE                    0x11151
#define IDCLS_GEORAM_SIZE_FR                    0x11152
#define IDCLS_GEORAM_SIZE_IT                    0x11153
#define IDCLS_GEORAM_SIZE_NL                    0x11154
#define IDCLS_GEORAM_SIZE_PL                    0x11155
#define IDCLS_GEORAM_SIZE_SV                    0x11156

/* c64/psid.c */
#define IDCLS_SID_PLAYER_MODE                   0x11157
#define IDCLS_SID_PLAYER_MODE_DE                0x11158
#define IDCLS_SID_PLAYER_MODE_FR                0x11159
#define IDCLS_SID_PLAYER_MODE_IT                0x1115a
#define IDCLS_SID_PLAYER_MODE_NL                0x1115b
#define IDCLS_SID_PLAYER_MODE_PL                0x1115c
#define IDCLS_SID_PLAYER_MODE_SV                0x1115d

/* c64/psid.c */
#define IDCLS_OVERWRITE_PSID_SETTINGS           0x1115e
#define IDCLS_OVERWRITE_PSID_SETTINGS_DE        0x1115f
#define IDCLS_OVERWRITE_PSID_SETTINGS_FR        0x11160
#define IDCLS_OVERWRITE_PSID_SETTINGS_IT        0x11161
#define IDCLS_OVERWRITE_PSID_SETTINGS_NL        0x11162
#define IDCLS_OVERWRITE_PSID_SETTINGS_PL        0x11163
#define IDCLS_OVERWRITE_PSID_SETTINGS_SV        0x11164

/* c64/psid.c */
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER          0x11165
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_DE       0x11166
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_FR       0x11167
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_IT       0x11168
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_NL       0x11169
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_PL       0x1116a
#define IDCLS_SPECIFY_PSID_TUNE_NUMBER_SV       0x1116b

/* c64/ramcart.c */
#define IDCLS_ENABLE_RAMCART                    0x1116c
#define IDCLS_ENABLE_RAMCART_DE                 0x1116d
#define IDCLS_ENABLE_RAMCART_FR                 0x1116e
#define IDCLS_ENABLE_RAMCART_IT                 0x1116f
#define IDCLS_ENABLE_RAMCART_NL                 0x11170
#define IDCLS_ENABLE_RAMCART_PL                 0x11171
#define IDCLS_ENABLE_RAMCART_SV                 0x11172

/* c64/ramcart.c */
#define IDCLS_DISABLE_RAMCART                   0x11173
#define IDCLS_DISABLE_RAMCART_DE                0x11174
#define IDCLS_DISABLE_RAMCART_FR                0x11175
#define IDCLS_DISABLE_RAMCART_IT                0x11176
#define IDCLS_DISABLE_RAMCART_NL                0x11177
#define IDCLS_DISABLE_RAMCART_PL                0x11178
#define IDCLS_DISABLE_RAMCART_SV                0x11179

/* c64/ramcart.c */
#define IDCLS_SPECIFY_RAMCART_NAME              0x1117a
#define IDCLS_SPECIFY_RAMCART_NAME_DE           0x1117b
#define IDCLS_SPECIFY_RAMCART_NAME_FR           0x1117c
#define IDCLS_SPECIFY_RAMCART_NAME_IT           0x1117d
#define IDCLS_SPECIFY_RAMCART_NAME_NL           0x1117e
#define IDCLS_SPECIFY_RAMCART_NAME_PL           0x1117f
#define IDCLS_SPECIFY_RAMCART_NAME_SV           0x11180

/* c64/ramcart.c */
#define IDCLS_RAMCART_SIZE                      0x11181
#define IDCLS_RAMCART_SIZE_DE                   0x11182
#define IDCLS_RAMCART_SIZE_FR                   0x11183
#define IDCLS_RAMCART_SIZE_IT                   0x11184
#define IDCLS_RAMCART_SIZE_NL                   0x11185
#define IDCLS_RAMCART_SIZE_PL                   0x11186
#define IDCLS_RAMCART_SIZE_SV                   0x11187

/* c64/reu.c */
#define IDCLS_ENABLE_REU                        0x11188
#define IDCLS_ENABLE_REU_DE                     0x11189
#define IDCLS_ENABLE_REU_FR                     0x1118a
#define IDCLS_ENABLE_REU_IT                     0x1118b
#define IDCLS_ENABLE_REU_NL                     0x1118c
#define IDCLS_ENABLE_REU_PL                     0x1118d
#define IDCLS_ENABLE_REU_SV                     0x1118e

/* c64/reu.c */
#define IDCLS_DISABLE_REU                       0x1118f
#define IDCLS_DISABLE_REU_DE                    0x11190
#define IDCLS_DISABLE_REU_FR                    0x11191
#define IDCLS_DISABLE_REU_IT                    0x11192
#define IDCLS_DISABLE_REU_NL                    0x11193
#define IDCLS_DISABLE_REU_PL                    0x11194
#define IDCLS_DISABLE_REU_SV                    0x11195

/* c64/reu.c */
#define IDCLS_SPECIFY_REU_NAME                  0x11196
#define IDCLS_SPECIFY_REU_NAME_DE               0x11197
#define IDCLS_SPECIFY_REU_NAME_FR               0x11198
#define IDCLS_SPECIFY_REU_NAME_IT               0x11199
#define IDCLS_SPECIFY_REU_NAME_NL               0x1119a
#define IDCLS_SPECIFY_REU_NAME_PL               0x1119b
#define IDCLS_SPECIFY_REU_NAME_SV               0x1119c

/* c64/reu.c */
#define IDCLS_REU_SIZE                          0x1119d
#define IDCLS_REU_SIZE_DE                       0x1119e
#define IDCLS_REU_SIZE_FR                       0x1119f
#define IDCLS_REU_SIZE_IT                       0x111a0
#define IDCLS_REU_SIZE_NL                       0x111a1
#define IDCLS_REU_SIZE_PL                       0x111a2
#define IDCLS_REU_SIZE_SV                       0x111a3

#ifdef HAVE_TFE
/* c64/tfe.c */
#define IDCLS_ENABLE_TFE                        0x111a4
#define IDCLS_ENABLE_TFE_DE                     0x111a5
#define IDCLS_ENABLE_TFE_FR                     0x111a6
#define IDCLS_ENABLE_TFE_IT                     0x111a7
#define IDCLS_ENABLE_TFE_NL                     0x111a8
#define IDCLS_ENABLE_TFE_PL                     0x111a9
#define IDCLS_ENABLE_TFE_SV                     0x111aa

/* c64/tfe.c */
#define IDCLS_DISABLE_TFE                       0x111ab
#define IDCLS_DISABLE_TFE_DE                    0x111ac
#define IDCLS_DISABLE_TFE_FR                    0x111ad
#define IDCLS_DISABLE_TFE_IT                    0x111ae
#define IDCLS_DISABLE_TFE_NL                    0x111af
#define IDCLS_DISABLE_TFE_PL                    0x111b0
#define IDCLS_DISABLE_TFE_SV                    0x111b1
#endif

/* c64/c64cart.c */
#define IDCLS_CART_ATTACH_DETACH_RESET          0x111b2
#define IDCLS_CART_ATTACH_DETACH_RESET_DE       0x111b3
#define IDCLS_CART_ATTACH_DETACH_RESET_FR       0x111b4
#define IDCLS_CART_ATTACH_DETACH_RESET_IT       0x111b5
#define IDCLS_CART_ATTACH_DETACH_RESET_NL       0x111b6
#define IDCLS_CART_ATTACH_DETACH_RESET_PL       0x111b7
#define IDCLS_CART_ATTACH_DETACH_RESET_SV       0x111b8

/* c64/c64cart.c */
#define IDCLS_CART_ATTACH_DETACH_NO_RESET       0x111b9
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_DE    0x111ba
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_FR    0x111bb
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_IT    0x111bc
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_NL    0x111bd
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_PL    0x111be
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_SV    0x111bf

/* c64/c64cart.c */
#define IDCLS_ATTACH_CRT_CART                   0x111c0
#define IDCLS_ATTACH_CRT_CART_DE                0x111c1
#define IDCLS_ATTACH_CRT_CART_FR                0x111c2
#define IDCLS_ATTACH_CRT_CART_IT                0x111c3
#define IDCLS_ATTACH_CRT_CART_NL                0x111c4
#define IDCLS_ATTACH_CRT_CART_PL                0x111c5
#define IDCLS_ATTACH_CRT_CART_SV                0x111c6

/* c64/c64cart.c */
#define IDCLS_ATTACH_GENERIC_8KB_CART           0x111c7
#define IDCLS_ATTACH_GENERIC_8KB_CART_DE        0x111c8
#define IDCLS_ATTACH_GENERIC_8KB_CART_FR        0x111c9
#define IDCLS_ATTACH_GENERIC_8KB_CART_IT        0x111ca
#define IDCLS_ATTACH_GENERIC_8KB_CART_NL        0x111cb
#define IDCLS_ATTACH_GENERIC_8KB_CART_PL        0x111cc
#define IDCLS_ATTACH_GENERIC_8KB_CART_SV        0x111cd

/* c64/c64cart.c */
#define IDCLS_ATTACH_GENERIC_16KB_CART          0x111ce
#define IDCLS_ATTACH_GENERIC_16KB_CART_DE       0x111cf
#define IDCLS_ATTACH_GENERIC_16KB_CART_FR       0x111d0
#define IDCLS_ATTACH_GENERIC_16KB_CART_IT       0x111d1
#define IDCLS_ATTACH_GENERIC_16KB_CART_NL       0x111d2
#define IDCLS_ATTACH_GENERIC_16KB_CART_PL       0x111d3
#define IDCLS_ATTACH_GENERIC_16KB_CART_SV       0x111d4

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART     0x111d5
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE  0x111d6
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR  0x111d7
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT  0x111d8
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL  0x111d9
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL  0x111da
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV  0x111db

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART      0x111dc
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE   0x111dd
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR   0x111de
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT   0x111df
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL   0x111e0
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL   0x111e1
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV   0x111e2

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_IDE64_CART             0x111e3
#define IDCLS_ATTACH_RAW_IDE64_CART_DE          0x111e4
#define IDCLS_ATTACH_RAW_IDE64_CART_FR          0x111e5
#define IDCLS_ATTACH_RAW_IDE64_CART_IT          0x111e6
#define IDCLS_ATTACH_RAW_IDE64_CART_NL          0x111e7
#define IDCLS_ATTACH_RAW_IDE64_CART_PL          0x111e8
#define IDCLS_ATTACH_RAW_IDE64_CART_SV          0x111e9

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART      0x111ea
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE   0x111eb
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR   0x111ec
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT   0x111ed
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL   0x111ee
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL   0x111ef
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV   0x111f0

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART     0x111f1
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE  0x111f2
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR  0x111f3
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT  0x111f4
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL  0x111f5
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL  0x111f6
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV  0x111f7

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_SS4_CART               0x111f8
#define IDCLS_ATTACH_RAW_SS4_CART_DE            0x111f9
#define IDCLS_ATTACH_RAW_SS4_CART_FR            0x111fa
#define IDCLS_ATTACH_RAW_SS4_CART_IT            0x111fb
#define IDCLS_ATTACH_RAW_SS4_CART_NL            0x111fc
#define IDCLS_ATTACH_RAW_SS4_CART_PL            0x111fd
#define IDCLS_ATTACH_RAW_SS4_CART_SV            0x111fe

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_SS5_CART               0x111ff
#define IDCLS_ATTACH_RAW_SS5_CART_DE            0x11200
#define IDCLS_ATTACH_RAW_SS5_CART_FR            0x11201
#define IDCLS_ATTACH_RAW_SS5_CART_IT            0x11202
#define IDCLS_ATTACH_RAW_SS5_CART_NL            0x11203
#define IDCLS_ATTACH_RAW_SS5_CART_PL            0x11204
#define IDCLS_ATTACH_RAW_SS5_CART_SV            0x11205

/* c64/c64cart.c */
#define IDCLS_ATTACH_CBM_IEEE488_CART           0x11206
#define IDCLS_ATTACH_CBM_IEEE488_CART_DE        0x11207
#define IDCLS_ATTACH_CBM_IEEE488_CART_FR        0x11208
#define IDCLS_ATTACH_CBM_IEEE488_CART_IT        0x11209
#define IDCLS_ATTACH_CBM_IEEE488_CART_NL        0x1120a
#define IDCLS_ATTACH_CBM_IEEE488_CART_PL        0x1120b
#define IDCLS_ATTACH_CBM_IEEE488_CART_SV        0x1120c

/* c64/c64cart.c */
#define IDCLS_ATTACH_RAW_WESTERMANN_CART        0x1120d
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_DE     0x1120e
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_FR     0x1120f
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_IT     0x11210
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_NL     0x11211
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_PL     0x11212
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_SV     0x11213

/* c64/c64cart.c */
#define IDCLS_ENABLE_EXPERT_CART                0x11214
#define IDCLS_ENABLE_EXPERT_CART_DE             0x11215
#define IDCLS_ENABLE_EXPERT_CART_FR             0x11216
#define IDCLS_ENABLE_EXPERT_CART_IT             0x11217
#define IDCLS_ENABLE_EXPERT_CART_NL             0x11218
#define IDCLS_ENABLE_EXPERT_CART_PL             0x11219
#define IDCLS_ENABLE_EXPERT_CART_SV             0x1121a

/* c64/ide64.c */
#define IDCLS_SPECIFY_IDE64_NAME                0x1121b
#define IDCLS_SPECIFY_IDE64_NAME_DE             0x1121c
#define IDCLS_SPECIFY_IDE64_NAME_FR             0x1121d
#define IDCLS_SPECIFY_IDE64_NAME_IT             0x1121e
#define IDCLS_SPECIFY_IDE64_NAME_NL             0x1121f
#define IDCLS_SPECIFY_IDE64_NAME_PL             0x11220
#define IDCLS_SPECIFY_IDE64_NAME_SV             0x11221

/* c64/ide64.c, gfxoutputdrv/ffmpegdrv.c */
#define IDCLS_P_VALUE                           0x11222
#define IDCLS_P_VALUE_DE                        0x11223
#define IDCLS_P_VALUE_FR                        0x11224
#define IDCLS_P_VALUE_IT                        0x11225
#define IDCLS_P_VALUE_NL                        0x11226
#define IDCLS_P_VALUE_PL                        0x11227
#define IDCLS_P_VALUE_SV                        0x11228

/* c64/ide64.c */
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64        0x11229
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE     0x1122a
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR     0x1122b
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT     0x1122c
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL     0x1122d
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL     0x1122e
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV     0x1122f

/* c64/ide64.c */
#define IDCLS_SET_AMOUNT_HEADS_IDE64            0x11230
#define IDCLS_SET_AMOUNT_HEADS_IDE64_DE         0x11231
#define IDCLS_SET_AMOUNT_HEADS_IDE64_FR         0x11232
#define IDCLS_SET_AMOUNT_HEADS_IDE64_IT         0x11233
#define IDCLS_SET_AMOUNT_HEADS_IDE64_NL         0x11234
#define IDCLS_SET_AMOUNT_HEADS_IDE64_PL         0x11235
#define IDCLS_SET_AMOUNT_HEADS_IDE64_SV         0x11236

/* c64/ide64.c */
#define IDCLS_SET_AMOUNT_SECTORS_IDE64          0x11237
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_DE       0x11238
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_FR       0x11239
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_IT       0x1123a
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_NL       0x1123b
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_PL       0x1123c
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_SV       0x1123d

/* c64/ide64.c */
#define IDCLS_AUTODETECT_IDE64_GEOMETRY         0x1123e
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_DE      0x1123f
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_FR      0x11240
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_IT      0x11241
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_NL      0x11242
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_PL      0x11243
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_SV      0x11244

/* c64/ide64.c */
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY      0x11245
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_DE   0x11246
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_FR   0x11247
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_IT   0x11248
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_NL   0x11249
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_PL   0x1124a
#define IDCLS_NO_AUTODETECT_IDE64_GEOMETRY_SV   0x1124b

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_P_MODELNUMBER                     0x1124c
#define IDCLS_P_MODELNUMBER_DE                  0x1124d
#define IDCLS_P_MODELNUMBER_FR                  0x1124e
#define IDCLS_P_MODELNUMBER_IT                  0x1124f
#define IDCLS_P_MODELNUMBER_NL                  0x11250
#define IDCLS_P_MODELNUMBER_PL                  0x11251
#define IDCLS_P_MODELNUMBER_SV                  0x11252

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CBM2_MODEL                0x11253
#define IDCLS_SPECIFY_CBM2_MODEL_DE             0x11254
#define IDCLS_SPECIFY_CBM2_MODEL_FR             0x11255
#define IDCLS_SPECIFY_CBM2_MODEL_IT             0x11256
#define IDCLS_SPECIFY_CBM2_MODEL_NL             0x11257
#define IDCLS_SPECIFY_CBM2_MODEL_PL             0x11258
#define IDCLS_SPECIFY_CBM2_MODEL_SV             0x11259

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_TO_USE_VIC_II             0x1125a
#define IDCLS_SPECIFY_TO_USE_VIC_II_DE          0x1125b
#define IDCLS_SPECIFY_TO_USE_VIC_II_FR          0x1125c
#define IDCLS_SPECIFY_TO_USE_VIC_II_IT          0x1125d
#define IDCLS_SPECIFY_TO_USE_VIC_II_NL          0x1125e
#define IDCLS_SPECIFY_TO_USE_VIC_II_PL          0x1125f
#define IDCLS_SPECIFY_TO_USE_VIC_II_SV          0x11260

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_TO_USE_CRTC               0x11261
#define IDCLS_SPECIFY_TO_USE_CRTC_DE            0x11262
#define IDCLS_SPECIFY_TO_USE_CRTC_FR            0x11263
#define IDCLS_SPECIFY_TO_USE_CRTC_IT            0x11264
#define IDCLS_SPECIFY_TO_USE_CRTC_NL            0x11265
#define IDCLS_SPECIFY_TO_USE_CRTC_PL            0x11266
#define IDCLS_SPECIFY_TO_USE_CRTC_SV            0x11267

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_P_LINENUMBER                      0x11268
#define IDCLS_P_LINENUMBER_DE                   0x11269
#define IDCLS_P_LINENUMBER_FR                   0x1126a
#define IDCLS_P_LINENUMBER_IT                   0x1126b
#define IDCLS_P_LINENUMBER_NL                   0x1126c
#define IDCLS_P_LINENUMBER_PL                   0x1126d
#define IDCLS_P_LINENUMBER_SV                   0x1126e

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE       0x1126f
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_DE    0x11270
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_FR    0x11271
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_IT    0x11272
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_NL    0x11273
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_PL    0x11274
#define IDCLS_SPECIFY_CBM2_MODEL_HARDWARE_SV    0x11275

/* cbm2/cbm2-cmdline-options.c, plus4/plus4-cmdline-options.c */
#define IDCLS_P_RAMSIZE                         0x11276
#define IDCLS_P_RAMSIZE_DE                      0x11277
#define IDCLS_P_RAMSIZE_FR                      0x11278
#define IDCLS_P_RAMSIZE_IT                      0x11279
#define IDCLS_P_RAMSIZE_NL                      0x1127a
#define IDCLS_P_RAMSIZE_PL                      0x1127b
#define IDCLS_P_RAMSIZE_SV                      0x1127c

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_SIZE_OF_RAM               0x1127d
#define IDCLS_SPECIFY_SIZE_OF_RAM_DE            0x1127e
#define IDCLS_SPECIFY_SIZE_OF_RAM_FR            0x1127f
#define IDCLS_SPECIFY_SIZE_OF_RAM_IT            0x11280
#define IDCLS_SPECIFY_SIZE_OF_RAM_NL            0x11281
#define IDCLS_SPECIFY_SIZE_OF_RAM_PL            0x11282
#define IDCLS_SPECIFY_SIZE_OF_RAM_SV            0x11283

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CART_ROM_1000_NAME        0x11284
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_DE     0x11285
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_FR     0x11286
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_IT     0x11287
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_NL     0x11288
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_PL     0x11289
#define IDCLS_SPECIFY_CART_ROM_1000_NAME_SV     0x1128a

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CART_ROM_2000_NAME        0x1128b
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_DE     0x1128c
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_FR     0x1128d
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_IT     0x1128e
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_NL     0x1128f
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_PL     0x11290
#define IDCLS_SPECIFY_CART_ROM_2000_NAME_SV     0x11291

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CART_ROM_4000_NAME        0x11292
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_DE     0x11293
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_FR     0x11294
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_IT     0x11295
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_NL     0x11296
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_PL     0x11297
#define IDCLS_SPECIFY_CART_ROM_4000_NAME_SV     0x11298

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_SPECIFY_CART_ROM_6000_NAME        0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_DE     0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_FR     0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_IT     0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_NL     0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_PL     0x11299
#define IDCLS_SPECIFY_CART_ROM_6000_NAME_SV     0x11299

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800        0x1129a
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_DE     0x1129b
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_FR     0x1129c
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_IT     0x1129d
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_NL     0x1129e
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_PL     0x1129f
#define IDCLS_ENABLE_RAM_MAPPING_IN_0800_SV     0x112a0

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800       0x112a1
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_DE    0x112a2
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_FR    0x112a3
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_IT    0x112a4
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_NL    0x112a5
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_PL    0x112a6
#define IDCLS_DISABLE_RAM_MAPPING_IN_0800_SV    0x112a7

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000        0x112a8
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_DE     0x112a9
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_FR     0x112aa
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_IT     0x112ab
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_NL     0x112ac
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_PL     0x112ad
#define IDCLS_ENABLE_RAM_MAPPING_IN_1000_SV     0x112ae

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000       0x112af
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_DE    0x112b0
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_FR    0x112b1
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_IT    0x112b2
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_NL    0x112b3
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_PL    0x112b4
#define IDCLS_DISABLE_RAM_MAPPING_IN_1000_SV    0x112b5

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000        0x112b6
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_DE     0x112b7
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_FR     0x112b8
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_IT     0x112b9
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_NL     0x112ba
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_PL     0x112bb
#define IDCLS_ENABLE_RAM_MAPPING_IN_2000_SV     0x112bc

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000       0x112bd
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_DE    0x112be
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_FR    0x112bf
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_IT    0x112c0
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_NL    0x112c1
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_PL    0x112c2
#define IDCLS_DISABLE_RAM_MAPPING_IN_2000_SV    0x112c3

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000        0x112c4
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_DE     0x112c5
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_FR     0x112c6
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_IT     0x112c7
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_NL     0x112c8
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_PL     0x112c9
#define IDCLS_ENABLE_RAM_MAPPING_IN_4000_SV     0x112ca

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000       0x112cb
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_DE    0x112cc
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_FR    0x112cd
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_IT    0x112ce
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_NL    0x112cf
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_PL    0x112d0
#define IDCLS_DISABLE_RAM_MAPPING_IN_4000_SV    0x112d1

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000        0x112d2
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_DE     0x112d3
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_FR     0x112d4
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_IT     0x112d5
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_NL     0x112d6
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_PL     0x112d7
#define IDCLS_ENABLE_RAM_MAPPING_IN_6000_SV     0x112d8

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000       0x112d9
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_DE    0x112da
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_FR    0x112db
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_IT    0x112dc
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_NL    0x112dd
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_PL    0x112de
#define IDCLS_DISABLE_RAM_MAPPING_IN_6000_SV    0x112df

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000        0x112c0
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_DE     0x112c1
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_FR     0x112c2
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_IT     0x112c3
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_NL     0x112c4
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_PL     0x112c5
#define IDCLS_ENABLE_RAM_MAPPING_IN_C000_SV     0x112c6

/* cbm2/cbm2-cmdline-options.c */
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000       0x112c7
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_DE    0x112c8
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_FR    0x112c9
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_IT    0x112ca
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_NL    0x112cb
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_PL    0x112cc
#define IDCLS_DISABLE_RAM_MAPPING_IN_C000_SV    0x112cd

#ifdef COMMON_KBD
/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_KEYMAP_INDEX              0x112ce
#define IDCLS_SPECIFY_KEYMAP_INDEX_DE           0x112cf
#define IDCLS_SPECIFY_KEYMAP_INDEX_FR           0x112d0
#define IDCLS_SPECIFY_KEYMAP_INDEX_IT           0x112d1
#define IDCLS_SPECIFY_KEYMAP_INDEX_NL           0x112d2
#define IDCLS_SPECIFY_KEYMAP_INDEX_PL           0x112d3
#define IDCLS_SPECIFY_KEYMAP_INDEX_SV           0x112d4

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME       0x112d5
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_DE    0x112d6
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_FR    0x112d7
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_IT    0x112d8
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_NL    0x112d9
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_PL    0x112da
#define IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME_SV    0x112db

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME       0x112dc
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_DE    0x112dd
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_FR    0x112de
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_IT    0x112df
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_NL    0x112e0
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_PL    0x112e1
#define IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME_SV    0x112e2

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME       0x112e3
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_DE    0x112e4
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_FR    0x112e5
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_IT    0x112e6
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_NL    0x112e7
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_PL    0x112e8
#define IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME_SV    0x112e9

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME       0x112ea
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_DE    0x112eb
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_FR    0x112ec
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_IT    0x112ed
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_NL    0x112ee
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_PL    0x112ef
#define IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME_SV    0x112f0

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME       0x112f1
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_DE    0x112f2
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_FR    0x112f3
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_IT    0x112f4
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_NL    0x112f5
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_PL    0x112f6
#define IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME_SV    0x112f7

/* cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME       0x112f8
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_DE    0x112f9
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_FR    0x112fa
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_IT    0x112fb
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_NL    0x112fc
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_PL    0x112fd
#define IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME_SV    0x112fe
#endif

/* drive/drive-cmdline-options.c */
#define IDCLS_ENABLE_TRUE_DRIVE                 0x112ff
#define IDCLS_ENABLE_TRUE_DRIVE_DE              0x11300
#define IDCLS_ENABLE_TRUE_DRIVE_FR              0x11301
#define IDCLS_ENABLE_TRUE_DRIVE_IT              0x11302
#define IDCLS_ENABLE_TRUE_DRIVE_NL              0x11303
#define IDCLS_ENABLE_TRUE_DRIVE_PL              0x11304
#define IDCLS_ENABLE_TRUE_DRIVE_SV              0x11305

/* drive/drive-cmdline-options.c */
#define IDCLS_DISABLE_TRUE_DRIVE                0x11306
#define IDCLS_DISABLE_TRUE_DRIVE_DE             0x11307
#define IDCLS_DISABLE_TRUE_DRIVE_FR             0x11308
#define IDCLS_DISABLE_TRUE_DRIVE_IT             0x11309
#define IDCLS_DISABLE_TRUE_DRIVE_NL             0x1130a
#define IDCLS_DISABLE_TRUE_DRIVE_PL             0x1130b
#define IDCLS_DISABLE_TRUE_DRIVE_SV             0x1130c

/* drive/drive-cmdline-options.c */
#define IDCLS_P_TYPE                            0x1130d
#define IDCLS_P_TYPE_DE                         0x1130e
#define IDCLS_P_TYPE_FR                         0x1130f
#define IDCLS_P_TYPE_IT                         0x11310
#define IDCLS_P_TYPE_NL                         0x11311
#define IDCLS_P_TYPE_PL                         0x11312
#define IDCLS_P_TYPE_SV                         0x11313

/* drive/drive-cmdline-options.c */
#define IDCLS_SET_DRIVE_TYPE                    0x11314
#define IDCLS_SET_DRIVE_TYPE_DE                 0x11315
#define IDCLS_SET_DRIVE_TYPE_FR                 0x11316
#define IDCLS_SET_DRIVE_TYPE_IT                 0x11317
#define IDCLS_SET_DRIVE_TYPE_NL                 0x11318
#define IDCLS_SET_DRIVE_TYPE_PL                 0x11319
#define IDCLS_SET_DRIVE_TYPE_SV                 0x1131a

/* drive/drive-cmdline-options.c */
#define IDCLS_P_METHOD                          0x1131b
#define IDCLS_P_METHOD_DE                       0x1131c
#define IDCLS_P_METHOD_FR                       0x1131d
#define IDCLS_P_METHOD_IT                       0x1131e
#define IDCLS_P_METHOD_NL                       0x1131f
#define IDCLS_P_METHOD_PL                       0x11320
#define IDCLS_P_METHOD_SV                       0x11321

/* drive/drive-cmdline-options.c */
#define IDCLS_SET_DRIVE_EXTENSION_POLICY        0x11322
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_DE     0x11323
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_FR     0x11324
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_IT     0x11325
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_NL     0x11326
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_PL     0x11327
#define IDCLS_SET_DRIVE_EXTENSION_POLICY_SV     0x11328

/* drive/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME         0x11329
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE      0x1132a
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR      0x1132b
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT      0x1132c
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL      0x1132d
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL      0x1132e
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV      0x1132f

/* drive/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME      0x11330
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE   0x11331
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR   0x11332
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT   0x11333
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL   0x11334
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL   0x11335
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV   0x11336

/* drive/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME         0x11337
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE      0x11338
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR      0x11339
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT      0x1133a
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL      0x1133b
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL      0x1133c
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV      0x1133d

/* drive/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME         0x1133e
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE      0x1133f
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR      0x11340
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT      0x11341
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL      0x11342
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL      0x11343
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV      0x11344

/* drive/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME         0x11345
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE      0x11346
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR      0x11347
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT      0x11348
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL      0x11349
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL      0x1134a
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV      0x1134b

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_PAR_CABLE                  0x1134c
#define IDCLS_ENABLE_PAR_CABLE_DE               0x1134d
#define IDCLS_ENABLE_PAR_CABLE_FR               0x1134e
#define IDCLS_ENABLE_PAR_CABLE_IT               0x1134f
#define IDCLS_ENABLE_PAR_CABLE_NL               0x11350
#define IDCLS_ENABLE_PAR_CABLE_PL               0x11351
#define IDCLS_ENABLE_PAR_CABLE_SV               0x11352

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_PAR_CABLE                 0x11353
#define IDCLS_DISABLE_PAR_CABLE_DE              0x11354
#define IDCLS_DISABLE_PAR_CABLE_FR              0x11355
#define IDCLS_DISABLE_PAR_CABLE_IT              0x11356
#define IDCLS_DISABLE_PAR_CABLE_NL              0x11357
#define IDCLS_DISABLE_PAR_CABLE_PL              0x11358
#define IDCLS_DISABLE_PAR_CABLE_SV              0x11359

/* drive/iec-cmdline-options.c */
#define IDCLS_SET_IDLE_METHOD                   0x1135a
#define IDCLS_SET_IDLE_METHOD_DE                0x1135b
#define IDCLS_SET_IDLE_METHOD_FR                0x1135c
#define IDCLS_SET_IDLE_METHOD_IT                0x1135d
#define IDCLS_SET_IDLE_METHOD_NL                0x1135e
#define IDCLS_SET_IDLE_METHOD_PL                0x1135f
#define IDCLS_SET_IDLE_METHOD_SV                0x11360

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_2000             0x11361
#define IDCLS_ENABLE_DRIVE_RAM_2000_DE          0x11362
#define IDCLS_ENABLE_DRIVE_RAM_2000_FR          0x11363
#define IDCLS_ENABLE_DRIVE_RAM_2000_IT          0x11364
#define IDCLS_ENABLE_DRIVE_RAM_2000_NL          0x11365
#define IDCLS_ENABLE_DRIVE_RAM_2000_PL          0x11366
#define IDCLS_ENABLE_DRIVE_RAM_2000_SV          0x11367

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_2000            0x11368
#define IDCLS_DISABLE_DRIVE_RAM_2000_DE         0x11369
#define IDCLS_DISABLE_DRIVE_RAM_2000_FR         0x1136a
#define IDCLS_DISABLE_DRIVE_RAM_2000_IT         0x1136b
#define IDCLS_DISABLE_DRIVE_RAM_2000_NL         0x1136c
#define IDCLS_DISABLE_DRIVE_RAM_2000_PL         0x1136d
#define IDCLS_DISABLE_DRIVE_RAM_2000_SV         0x1136e

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_4000             0x1136f
#define IDCLS_ENABLE_DRIVE_RAM_4000_DE          0x11370
#define IDCLS_ENABLE_DRIVE_RAM_4000_FR          0x11371
#define IDCLS_ENABLE_DRIVE_RAM_4000_IT          0x11372
#define IDCLS_ENABLE_DRIVE_RAM_4000_NL          0x11373
#define IDCLS_ENABLE_DRIVE_RAM_4000_PL          0x11374
#define IDCLS_ENABLE_DRIVE_RAM_4000_SV          0x11375

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_4000            0x11376
#define IDCLS_DISABLE_DRIVE_RAM_4000_DE         0x11377
#define IDCLS_DISABLE_DRIVE_RAM_4000_FR         0x11378
#define IDCLS_DISABLE_DRIVE_RAM_4000_IT         0x11379
#define IDCLS_DISABLE_DRIVE_RAM_4000_NL         0x1137a
#define IDCLS_DISABLE_DRIVE_RAM_4000_PL         0x1137b
#define IDCLS_DISABLE_DRIVE_RAM_4000_SV         0x1137c

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_6000             0x1137d
#define IDCLS_ENABLE_DRIVE_RAM_6000_DE          0x1137e
#define IDCLS_ENABLE_DRIVE_RAM_6000_FR          0x1137f
#define IDCLS_ENABLE_DRIVE_RAM_6000_IT          0x11380
#define IDCLS_ENABLE_DRIVE_RAM_6000_NL          0x11381
#define IDCLS_ENABLE_DRIVE_RAM_6000_PL          0x11382
#define IDCLS_ENABLE_DRIVE_RAM_6000_SV          0x11383

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_6000            0x11384
#define IDCLS_DISABLE_DRIVE_RAM_6000_DE         0x11385
#define IDCLS_DISABLE_DRIVE_RAM_6000_FR         0x11386
#define IDCLS_DISABLE_DRIVE_RAM_6000_IT         0x11387
#define IDCLS_DISABLE_DRIVE_RAM_6000_NL         0x11388
#define IDCLS_DISABLE_DRIVE_RAM_6000_PL         0x11389
#define IDCLS_DISABLE_DRIVE_RAM_6000_SV         0x1138a

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_8000             0x1138b
#define IDCLS_ENABLE_DRIVE_RAM_8000_DE          0x1138c
#define IDCLS_ENABLE_DRIVE_RAM_8000_FR          0x1138d
#define IDCLS_ENABLE_DRIVE_RAM_8000_IT          0x1138e
#define IDCLS_ENABLE_DRIVE_RAM_8000_NL          0x1138f
#define IDCLS_ENABLE_DRIVE_RAM_8000_PL          0x11390
#define IDCLS_ENABLE_DRIVE_RAM_8000_SV          0x11391

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_8000            0x11392
#define IDCLS_DISABLE_DRIVE_RAM_8000_DE         0x11393
#define IDCLS_DISABLE_DRIVE_RAM_8000_FR         0x11394
#define IDCLS_DISABLE_DRIVE_RAM_8000_IT         0x11395
#define IDCLS_DISABLE_DRIVE_RAM_8000_NL         0x11396
#define IDCLS_DISABLE_DRIVE_RAM_8000_PL         0x11397
#define IDCLS_DISABLE_DRIVE_RAM_8000_SV         0x11398

/* drive/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_A000             0x11399
#define IDCLS_ENABLE_DRIVE_RAM_A000_DE          0x1139a
#define IDCLS_ENABLE_DRIVE_RAM_A000_FR          0x1139b
#define IDCLS_ENABLE_DRIVE_RAM_A000_IT          0x1139c
#define IDCLS_ENABLE_DRIVE_RAM_A000_NL          0x1139d
#define IDCLS_ENABLE_DRIVE_RAM_A000_PL          0x1139e
#define IDCLS_ENABLE_DRIVE_RAM_A000_SV          0x1139f

/* drive/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_A000            0x113a0
#define IDCLS_DISABLE_DRIVE_RAM_A000_DE         0x113a1
#define IDCLS_DISABLE_DRIVE_RAM_A000_FR         0x113a2
#define IDCLS_DISABLE_DRIVE_RAM_A000_IT         0x113a3
#define IDCLS_DISABLE_DRIVE_RAM_A000_NL         0x113a4
#define IDCLS_DISABLE_DRIVE_RAM_A000_PL         0x113a5
#define IDCLS_DISABLE_DRIVE_RAM_A000_SV         0x113a6

/* drive/iec128dcr-cmdline-options.c */
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME       0x113a7
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE    0x113a8
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR    0x113a9
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT    0x113aa
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL    0x113ab
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL    0x113ac
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV    0x113ad

/* drive/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME         0x113ae
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE      0x113af
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR      0x113b0
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT      0x113b1
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL      0x113b2
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL      0x113b3
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV      0x113b4

/* drive/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME         0x113b5
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE      0x113b6
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR      0x113b7
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT      0x113b8
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL      0x113b9
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL      0x113ba
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV      0x113bb

/* drive/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME         0x113bc
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE      0x113bd
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR      0x113be
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT      0x113bf
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL      0x113c0
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL      0x113c1
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV      0x113c2

/* drive/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME         0x113c3
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE      0x113c4
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR      0x113c5
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT      0x113c6
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL      0x113c7
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL      0x113c8
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV      0x113c9

/* drive/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME         0x113ca
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE      0x113cb
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR      0x113cc
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT      0x113cd
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL      0x113ce
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL      0x113cf
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV      0x113d0

/* drive/tcbm-cmdline-options.c */
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME         0x113d1
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_DE      0x113d2
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_FR      0x113d3
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_IT      0x113d4
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_NL      0x113d5
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_PL      0x113d6
#define IDCLS_SPECIFY_1551_DOS_ROM_NAME_SV      0x113d7

/* fsdevice/fsdevice-cmdline-options.c */
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8       0x113d8
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_DE    0x113d9
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_FR    0x113da
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_IT    0x113db
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_NL    0x113dc
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_PL    0x113dd
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_8_SV    0x113de

/* fsdevice/fsdevice-cmdline-options.c */
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9       0x113df
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_DE    0x113e0
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_FR    0x113e1
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_IT    0x113e2
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_NL    0x113e3
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_PL    0x113e4
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_9_SV    0x113e5

/* fsdevice/fsdevice-cmdline-options.c */
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10      0x113e6
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_DE   0x113e7
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_FR   0x113e8
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_IT   0x113e9
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_NL   0x113ea
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_PL   0x113eb
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_10_SV   0x113ec

/* fsdevice/fsdevice-cmdline-options.c */
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11      0x113ed
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_DE   0x113ee
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_FR   0x113ef
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_IT   0x113f0
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_NL   0x113f1
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_PL   0x113f2
#define IDCLS_USE_AS_DIRECTORY_FSDEVICE_11_SV   0x113f3

#ifdef HAVE_FFMPEG
/* gfxoutputdrv/ffmpegdrv.c */
#define IDCLS_SET_AUDIO_STREAM_BITRATE          0x113f4
#define IDCLS_SET_AUDIO_STREAM_BITRATE_DE       0x113f5
#define IDCLS_SET_AUDIO_STREAM_BITRATE_FR       0x113f6
#define IDCLS_SET_AUDIO_STREAM_BITRATE_IT       0x113f7
#define IDCLS_SET_AUDIO_STREAM_BITRATE_NL       0x113f8
#define IDCLS_SET_AUDIO_STREAM_BITRATE_PL       0x113f9
#define IDCLS_SET_AUDIO_STREAM_BITRATE_SV       0x113fa

/* gfxoutputdrv/ffmpegdrv.c */
#define IDCLS_SET_VIDEO_STREAM_BITRATE          0x113fb
#define IDCLS_SET_VIDEO_STREAM_BITRATE_DE       0x113fc
#define IDCLS_SET_VIDEO_STREAM_BITRATE_FR       0x113fd
#define IDCLS_SET_VIDEO_STREAM_BITRATE_IT       0x113fe
#define IDCLS_SET_VIDEO_STREAM_BITRATE_NL       0x113ff
#define IDCLS_SET_VIDEO_STREAM_BITRATE_PL       0x11400
#define IDCLS_SET_VIDEO_STREAM_BITRATE_SV       0x11401
#endif

/* pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_PET_MODEL                 0x11402
#define IDCLS_SPECIFY_PET_MODEL_DE              0x11403
#define IDCLS_SPECIFY_PET_MODEL_FR              0x11404
#define IDCLS_SPECIFY_PET_MODEL_IT              0x11405
#define IDCLS_SPECIFY_PET_MODEL_NL              0x11406
#define IDCLS_SPECIFY_PET_MODEL_PL              0x11407
#define IDCLS_SPECIFY_PET_MODEL_SV              0x11408

/* pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_EDITOR_ROM_NAME           0x11409
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_DE        0x1140a
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_FR        0x1140b
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_IT        0x1140c
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_NL        0x1140d
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_PL        0x1140e
#define IDCLS_SPECIFY_EDITOR_ROM_NAME_SV        0x1140f

/* pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME          0x11410
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_DE       0x11411
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_FR       0x11412
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_IT       0x11413
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_NL       0x11414
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_PL       0x11415
#define IDCLS_SPECIFY_4K_ROM_9XXX_NAME_SV       0x11416

/* pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME          0x11417
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_DE       0x11418
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_FR       0x11419
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_IT       0x1141a
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_NL       0x1141b
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_PL       0x1141c
#define IDCLS_SPECIFY_4K_ROM_AXXX_NAME_SV       0x1141d

/* pet/pet-cmdline-options.c */
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME          0x1141e
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_DE       0x1141f
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_FR       0x11420
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_IT       0x11421
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_NL       0x11422
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_PL       0x11423
#define IDCLS_SPECIFY_4K_ROM_BXXX_NAME_SV       0x11424

/* pet/pet-cmdline-options.c */
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000     0x11425
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_DE  0x11426
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_FR  0x11427
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_IT  0x11428
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_NL  0x11429
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_PL  0x1142a
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_9000_SV  0x1142b

/* pet/pet-cmdline-options.c */
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000    0x1142c
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_DE 0x1142d
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_FR 0x1142e
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_IT 0x1142f
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_NL 0x11430
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_PL 0x11431
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_9000_SV 0x11432

/* pet/pet-cmdline-options.c */
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000     0x11433
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_DE  0x11434
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_FR  0x11435
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_IT  0x11436
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_NL  0x11437
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_PL  0x11438
#define IDCLS_PET8296_ENABLE_4K_RAM_AT_A000_SV  0x11439

/* pet/pet-cmdline-options.c */
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000    0x1143a
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_DE 0x1143b
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_FR 0x1143c
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_IT 0x1143d
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_NL 0x1143e
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_PL 0x1143f
#define IDCLS_PET8296_DISABLE_4K_RAM_AT_A000_SV 0x11440

/* pet/pet-cmdline-options.c */
#define IDCLS_ENABLE_SUPERPET_IO                0x11441
#define IDCLS_ENABLE_SUPERPET_IO_DE             0x11442
#define IDCLS_ENABLE_SUPERPET_IO_FR             0x11443
#define IDCLS_ENABLE_SUPERPET_IO_IT             0x11444
#define IDCLS_ENABLE_SUPERPET_IO_NL             0x11445
#define IDCLS_ENABLE_SUPERPET_IO_PL             0x11446
#define IDCLS_ENABLE_SUPERPET_IO_SV             0x11447

/* pet/pet-cmdline-options.c */
#define IDCLS_DISABLE_SUPERPET_IO               0x11448
#define IDCLS_DISABLE_SUPERPET_IO_DE            0x11449
#define IDCLS_DISABLE_SUPERPET_IO_FR            0x1144a
#define IDCLS_DISABLE_SUPERPET_IO_IT            0x1144b
#define IDCLS_DISABLE_SUPERPET_IO_NL            0x1144c
#define IDCLS_DISABLE_SUPERPET_IO_PL            0x1144d
#define IDCLS_DISABLE_SUPERPET_IO_SV            0x1144e

/* pet/pet-cmdline-options.c */
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES       0x1144f
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_DE    0x11450
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_FR    0x11451
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_IT    0x11452
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_NL    0x11453
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_PL    0x11454
#define IDCLS_ENABLE_ROM_1_KERNAL_PATCHES_SV    0x11455

/* pet/pet-cmdline-options.c */
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES      0x11456
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_DE   0x11457
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_FR   0x11458
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_IT   0x11459
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_NL   0x1145a
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_PL   0x1145b
#define IDCLS_DISABLE_ROM_1_KERNAL_PATCHES_SV   0x1145c

/* pet/pet-cmdline-options.c */
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET        0x1145d
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_DE     0x1145e
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_FR     0x1145f
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_IT     0x11460
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_NL     0x11461
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_PL     0x11462
#define IDCLS_SWITCH_UPPER_LOWER_CHARSET_SV     0x11463

/* pet/pet-cmdline-options.c */
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET     0x11464
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_DE  0x11465
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_FR  0x11466
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_IT  0x11467
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_NL  0x11468
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_PL  0x11469
#define IDCLS_NO_SWITCH_UPPER_LOWER_CHARSET_SV  0x1146a

/* pet/pet-cmdline-options.c */
#define IDCLS_EOI_BLANKS_SCREEN                 0x1146b
#define IDCLS_EOI_BLANKS_SCREEN_DE              0x1146c
#define IDCLS_EOI_BLANKS_SCREEN_FR              0x1146d
#define IDCLS_EOI_BLANKS_SCREEN_IT              0x1146e
#define IDCLS_EOI_BLANKS_SCREEN_NL              0x1146f
#define IDCLS_EOI_BLANKS_SCREEN_PL              0x11470
#define IDCLS_EOI_BLANKS_SCREEN_SV              0x11471

/* pet/pet-cmdline-options.c */
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN         0x11472
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_DE      0x11473
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_FR      0x11474
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_IT      0x11475
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_NL      0x11476
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_PL      0x11477
#define IDCLS_EOI_DOES_NOT_BLANK_SCREEN_SV      0x11478

/* pet/petpia1.c */
#define IDCLS_ENABLE_USERPORT_DIAG_PIN          0x11479
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_DE       0x1147a
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_FR       0x1147b
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_IT       0x1147c
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_NL       0x1147d
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_PL       0x1147e
#define IDCLS_ENABLE_USERPORT_DIAG_PIN_SV       0x1147f

/* pet/petpia1.c */
#define IDCLS_DISABLE_USERPORT_DIAG_PIN         0x11480
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_DE      0x11481
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_FR      0x11482
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_IT      0x11483
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_NL      0x11484
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_PL      0x11485
#define IDCLS_DISABLE_USERPORT_DIAG_PIN_SV      0x11486

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME     0x11487
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_DE  0x11488
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_FR  0x11489
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_IT  0x1148a
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_NL  0x1148b
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_PL  0x1148c
#define IDCLS_SPECIFY_FUNCTION_LOW_ROM_NAME_SV  0x1148d

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME    0x1148e
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_DE 0x1148f
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_FR 0x11490
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_IT 0x11491
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_NL 0x11492
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_PL 0x11493
#define IDCLS_SPECIFY_FUNCTION_HIGH_ROM_NAME_SV 0x11494

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME       0x11495
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_DE    0x11496
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_FR    0x11497
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_IT    0x11498
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_NL    0x11499
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_PL    0x1149a
#define IDCLS_SPECIFY_CART_1_LOW_ROM_NAME_SV    0x1149b

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME      0x1149c
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_DE   0x1149d
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_FR   0x1149e
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_IT   0x1149f
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_NL   0x114a0
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_PL   0x114a1
#define IDCLS_SPECIFY_CART_1_HIGH_ROM_NAME_SV   0x114a2

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME       0x114a3
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_DE    0x114a4
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_FR    0x114a5
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_IT    0x114a6
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_NL    0x114a7
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_PL    0x114a8
#define IDCLS_SPECIFY_CART_2_LOW_ROM_NAME_SV    0x114a9

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME      0x114aa
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_DE   0x114ab
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_FR   0x114ac
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_IT   0x114ad
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_NL   0x114ae
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_PL   0x114af
#define IDCLS_SPECIFY_CART_2_HIGH_ROM_NAME_SV   0x114b0

/* plus4/plus4-cmdline-options.c */
#define IDCLS_SPECIFY_RAM_INSTALLED             0x114b1
#define IDCLS_SPECIFY_RAM_INSTALLED_DE          0x114b2
#define IDCLS_SPECIFY_RAM_INSTALLED_FR          0x114b3
#define IDCLS_SPECIFY_RAM_INSTALLED_IT          0x114b4
#define IDCLS_SPECIFY_RAM_INSTALLED_NL          0x114b5
#define IDCLS_SPECIFY_RAM_INSTALLED_PL          0x114b6
#define IDCLS_SPECIFY_RAM_INSTALLED_SV          0x114b7

/* plus4/ted-cmdline-options.c */
#define IDCLS_P_0_2000                          0x114b8
#define IDCLS_P_0_2000_DE                       0x114b9
#define IDCLS_P_0_2000_FR                       0x114ba
#define IDCLS_P_0_2000_IT                       0x114bb
#define IDCLS_P_0_2000_NL                       0x114bc
#define IDCLS_P_0_2000_PL                       0x114bd
#define IDCLS_P_0_2000_SV                       0x114be

/* plus4/ted-cmdline-options.c */
#define IDCLS_SET_SATURATION                    0x114bf
#define IDCLS_SET_SATURATION_DE                 0x114c0
#define IDCLS_SET_SATURATION_FR                 0x114c1
#define IDCLS_SET_SATURATION_IT                 0x114c2
#define IDCLS_SET_SATURATION_NL                 0x114c3
#define IDCLS_SET_SATURATION_PL                 0x114c4
#define IDCLS_SET_SATURATION_SV                 0x114c5

/* plus4/ted-cmdline-options.c */
#define IDCLS_SET_CONTRAST                      0x114c6
#define IDCLS_SET_CONTRAST_DE                   0x114c7
#define IDCLS_SET_CONTRAST_FR                   0x114c8
#define IDCLS_SET_CONTRAST_IT                   0x114c9
#define IDCLS_SET_CONTRAST_NL                   0x114ca
#define IDCLS_SET_CONTRAST_PL                   0x114cb
#define IDCLS_SET_CONTRAST_SV                   0x114cc

/* plus4/ted-cmdline-options.c */
#define IDCLS_SET_BRIGHTNESS                    0x114cd
#define IDCLS_SET_BRIGHTNESS_DE                 0x114ce
#define IDCLS_SET_BRIGHTNESS_FR                 0x114cf
#define IDCLS_SET_BRIGHTNESS_IT                 0x114d0
#define IDCLS_SET_BRIGHTNESS_NL                 0x114d1
#define IDCLS_SET_BRIGHTNESS_PL                 0x114d2
#define IDCLS_SET_BRIGHTNESS_SV                 0x114d3

/* plus4/ted-cmdline-options.c */
#define IDCLS_SET_GAMMA                         0x114d4
#define IDCLS_SET_GAMMA_DE                      0x114d5
#define IDCLS_SET_GAMMA_FR                      0x114d6
#define IDCLS_SET_GAMMA_IT                      0x114d7
#define IDCLS_SET_GAMMA_NL                      0x114d8
#define IDCLS_SET_GAMMA_PL                      0x114d9
#define IDCLS_SET_GAMMA_SV                      0x114da

#endif
#endif
