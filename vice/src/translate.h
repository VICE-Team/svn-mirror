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

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_USE_PAL_SYNC_FACTOR               0x11000
#define IDCLS_USE_PAL_SYNC_FACTOR_DE            0x11001
#define IDCLS_USE_PAL_SYNC_FACTOR_FR            0x11002
#define IDCLS_USE_PAL_SYNC_FACTOR_IT            0x11003
#define IDCLS_USE_PAL_SYNC_FACTOR_NL            0x11004
#define IDCLS_USE_PAL_SYNC_FACTOR_PL            0x11005
#define IDCLS_USE_PAL_SYNC_FACTOR_SV            0x11006

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_USE_NTSC_SYNC_FACTOR              0x11007
#define IDCLS_USE_NTSC_SYNC_FACTOR_DE           0x11008
#define IDCLS_USE_NTSC_SYNC_FACTOR_FR           0x11009
#define IDCLS_USE_NTSC_SYNC_FACTOR_IT           0x1100a
#define IDCLS_USE_NTSC_SYNC_FACTOR_NL           0x1100b
#define IDCLS_USE_NTSC_SYNC_FACTOR_PL           0x1100c
#define IDCLS_USE_NTSC_SYNC_FACTOR_SV           0x1100d

/* c128/c128-cmdline-options.c, c128/functionrom.c,
   c64/c64-cmdline-options.c, c64/georam.c,
   c64/ramcart.c, c64/reu.c, c64/cart/c64cart.c,
   c64/cart/ide64.c, cbm2/cbm2-cmdline-options.c,
   drive/iec/iec-cmdline-options.c, drive/iec128dcr/iec128dcr-cmdline-options.c,
   drive/ieee/ieee-cmdline-options.c, drive/tcbm/tcbm-cmdline-options.c,
   fsdevice/fsdevice-cmdline-options.c, pet/pet-cmdline-options.c,
   plus4/plus4-cmdline-options.c, printerdrv/driver-select.c,
   printerdrv/output-select.c, printerdrv/output-text.c,
   rs232drv/rs232drv.c, vic20/vic20-cmdline-options.c,
   vic20/vic20cartridge.c, video/video-cmdline-options.c,
   fliplist.c, initcmdline.c, log.c, sound.c, c64/plus60k.c,
   c64/c64_256k.c */
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
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_ENABLE_EMULATOR_ID                0x1107e
#define IDCLS_ENABLE_EMULATOR_ID_DE             0x1107f
#define IDCLS_ENABLE_EMULATOR_ID_FR             0x11080
#define IDCLS_ENABLE_EMULATOR_ID_IT             0x11081
#define IDCLS_ENABLE_EMULATOR_ID_NL             0x11082
#define IDCLS_ENABLE_EMULATOR_ID_PL             0x11083
#define IDCLS_ENABLE_EMULATOR_ID_SV             0x11084

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   cbm2/cbm2-cmdline-options.c, pet/pet-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
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

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   c64/psid.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vdc/vdc-cmdline-options.c, vic20/vic20-cmdline-options.c */
#define IDCLS_P_NUMBER                          0x110b6
#define IDCLS_P_NUMBER_DE                       0x110b7
#define IDCLS_P_NUMBER_FR                       0x110b8
#define IDCLS_P_NUMBER_IT                       0x110b9
#define IDCLS_P_NUMBER_NL                       0x110ba
#define IDCLS_P_NUMBER_PL                       0x110bb
#define IDCLS_P_NUMBER_SV                       0x110bc

#ifdef COMMON_KBD
/* c128/c128-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX         0x110bd
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_DE      0x110be
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_FR      0x110bf
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_IT      0x110c0
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_NL      0x110c1
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_PL      0x110c2
#define IDCLS_SPECIFY_KEYMAP_FILE_INDEX_SV      0x110c3

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME      0x110c4
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_DE   0x110c5
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_FR   0x110c6
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_IT   0x110c7
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_NL   0x110c8
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_PL   0x110c9
#define IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME_SV   0x110ca

/* c128/c128-cmdline-options.c, c64/c64-cmdline-options.c,
   plus4/plus4-cmdline-options.c, vic20/vic20-cmdline-options.c */
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
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_SPECIFY_KERNAL_ROM_NAME           0x11111
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_DE        0x11112
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_FR        0x11113
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_IT        0x11114
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_NL        0x11115
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_PL        0x11116
#define IDCLS_SPECIFY_KERNAL_ROM_NAME_SV        0x11117

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, plus4/plus4-cmdline-options.c,
   vic20/vic20-cmdline-options.c */
#define IDCLS_SPECIFY_BASIC_ROM_NAME            0x11118
#define IDCLS_SPECIFY_BASIC_ROM_NAME_DE         0x11119
#define IDCLS_SPECIFY_BASIC_ROM_NAME_FR         0x1111a
#define IDCLS_SPECIFY_BASIC_ROM_NAME_IT         0x1111b
#define IDCLS_SPECIFY_BASIC_ROM_NAME_NL         0x1111c
#define IDCLS_SPECIFY_BASIC_ROM_NAME_PL         0x1111d
#define IDCLS_SPECIFY_BASIC_ROM_NAME_SV         0x1111e

/* c64/c64-cmdline-options.c, cbm2/cbm2-cmdline-options.c,
   pet/pet-cmdline-options.c, vic20/vic20-cmdline-options.c */
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

/* c64/cart/c64cart.c */
#define IDCLS_CART_ATTACH_DETACH_RESET          0x111b2
#define IDCLS_CART_ATTACH_DETACH_RESET_DE       0x111b3
#define IDCLS_CART_ATTACH_DETACH_RESET_FR       0x111b4
#define IDCLS_CART_ATTACH_DETACH_RESET_IT       0x111b5
#define IDCLS_CART_ATTACH_DETACH_RESET_NL       0x111b6
#define IDCLS_CART_ATTACH_DETACH_RESET_PL       0x111b7
#define IDCLS_CART_ATTACH_DETACH_RESET_SV       0x111b8

/* c64/cart/c64cart.c */
#define IDCLS_CART_ATTACH_DETACH_NO_RESET       0x111b9
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_DE    0x111ba
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_FR    0x111bb
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_IT    0x111bc
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_NL    0x111bd
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_PL    0x111be
#define IDCLS_CART_ATTACH_DETACH_NO_RESET_SV    0x111bf

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_CRT_CART                   0x111c0
#define IDCLS_ATTACH_CRT_CART_DE                0x111c1
#define IDCLS_ATTACH_CRT_CART_FR                0x111c2
#define IDCLS_ATTACH_CRT_CART_IT                0x111c3
#define IDCLS_ATTACH_CRT_CART_NL                0x111c4
#define IDCLS_ATTACH_CRT_CART_PL                0x111c5
#define IDCLS_ATTACH_CRT_CART_SV                0x111c6

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_GENERIC_8KB_CART           0x111c7
#define IDCLS_ATTACH_GENERIC_8KB_CART_DE        0x111c8
#define IDCLS_ATTACH_GENERIC_8KB_CART_FR        0x111c9
#define IDCLS_ATTACH_GENERIC_8KB_CART_IT        0x111ca
#define IDCLS_ATTACH_GENERIC_8KB_CART_NL        0x111cb
#define IDCLS_ATTACH_GENERIC_8KB_CART_PL        0x111cc
#define IDCLS_ATTACH_GENERIC_8KB_CART_SV        0x111cd

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_GENERIC_16KB_CART          0x111ce
#define IDCLS_ATTACH_GENERIC_16KB_CART_DE       0x111cf
#define IDCLS_ATTACH_GENERIC_16KB_CART_FR       0x111d0
#define IDCLS_ATTACH_GENERIC_16KB_CART_IT       0x111d1
#define IDCLS_ATTACH_GENERIC_16KB_CART_NL       0x111d2
#define IDCLS_ATTACH_GENERIC_16KB_CART_PL       0x111d3
#define IDCLS_ATTACH_GENERIC_16KB_CART_SV       0x111d4

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART     0x111d5
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_DE  0x111d6
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_FR  0x111d7
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_IT  0x111d8
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_NL  0x111d9
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_PL  0x111da
#define IDCLS_ATTACH_RAW_ACTION_REPLAY_CART_SV  0x111db

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART      0x111dc
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_DE   0x111dd
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_FR   0x111de
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_IT   0x111df
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_NL   0x111e0
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_PL   0x111e1
#define IDCLS_ATTACH_RAW_RETRO_REPLAY_CART_SV   0x111e2

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_IDE64_CART             0x111e3
#define IDCLS_ATTACH_RAW_IDE64_CART_DE          0x111e4
#define IDCLS_ATTACH_RAW_IDE64_CART_FR          0x111e5
#define IDCLS_ATTACH_RAW_IDE64_CART_IT          0x111e6
#define IDCLS_ATTACH_RAW_IDE64_CART_NL          0x111e7
#define IDCLS_ATTACH_RAW_IDE64_CART_PL          0x111e8
#define IDCLS_ATTACH_RAW_IDE64_CART_SV          0x111e9

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART      0x111ea
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_DE   0x111eb
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_FR   0x111ec
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_IT   0x111ed
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_NL   0x111ee
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_PL   0x111ef
#define IDCLS_ATTACH_RAW_ATOMIC_POWER_CART_SV   0x111f0

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART     0x111f1
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_DE  0x111f2
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_FR  0x111f3
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_IT  0x111f4
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_NL  0x111f5
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_PL  0x111f6
#define IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART_SV  0x111f7

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_SS4_CART               0x111f8
#define IDCLS_ATTACH_RAW_SS4_CART_DE            0x111f9
#define IDCLS_ATTACH_RAW_SS4_CART_FR            0x111fa
#define IDCLS_ATTACH_RAW_SS4_CART_IT            0x111fb
#define IDCLS_ATTACH_RAW_SS4_CART_NL            0x111fc
#define IDCLS_ATTACH_RAW_SS4_CART_PL            0x111fd
#define IDCLS_ATTACH_RAW_SS4_CART_SV            0x111fe

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_SS5_CART               0x111ff
#define IDCLS_ATTACH_RAW_SS5_CART_DE            0x11200
#define IDCLS_ATTACH_RAW_SS5_CART_FR            0x11201
#define IDCLS_ATTACH_RAW_SS5_CART_IT            0x11202
#define IDCLS_ATTACH_RAW_SS5_CART_NL            0x11203
#define IDCLS_ATTACH_RAW_SS5_CART_PL            0x11204
#define IDCLS_ATTACH_RAW_SS5_CART_SV            0x11205

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_CBM_IEEE488_CART           0x11206
#define IDCLS_ATTACH_CBM_IEEE488_CART_DE        0x11207
#define IDCLS_ATTACH_CBM_IEEE488_CART_FR        0x11208
#define IDCLS_ATTACH_CBM_IEEE488_CART_IT        0x11209
#define IDCLS_ATTACH_CBM_IEEE488_CART_NL        0x1120a
#define IDCLS_ATTACH_CBM_IEEE488_CART_PL        0x1120b
#define IDCLS_ATTACH_CBM_IEEE488_CART_SV        0x1120c

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_WESTERMANN_CART        0x1120d
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_DE     0x1120e
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_FR     0x1120f
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_IT     0x11210
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_NL     0x11211
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_PL     0x11212
#define IDCLS_ATTACH_RAW_WESTERMANN_CART_SV     0x11213

/* c64/cart/c64cart.c */
#define IDCLS_ENABLE_EXPERT_CART                0x11214
#define IDCLS_ENABLE_EXPERT_CART_DE             0x11215
#define IDCLS_ENABLE_EXPERT_CART_FR             0x11216
#define IDCLS_ENABLE_EXPERT_CART_IT             0x11217
#define IDCLS_ENABLE_EXPERT_CART_NL             0x11218
#define IDCLS_ENABLE_EXPERT_CART_PL             0x11219
#define IDCLS_ENABLE_EXPERT_CART_SV             0x1121a

/* c64/cart/ide64.c */
#define IDCLS_SPECIFY_IDE64_NAME                0x1121b
#define IDCLS_SPECIFY_IDE64_NAME_DE             0x1121c
#define IDCLS_SPECIFY_IDE64_NAME_FR             0x1121d
#define IDCLS_SPECIFY_IDE64_NAME_IT             0x1121e
#define IDCLS_SPECIFY_IDE64_NAME_NL             0x1121f
#define IDCLS_SPECIFY_IDE64_NAME_PL             0x11220
#define IDCLS_SPECIFY_IDE64_NAME_SV             0x11221

/* c64/cart/ide64.c, gfxoutputdrv/ffmpegdrv.c, datasette.c,
   debug.c, mouse.c, ram.c, sound.c, vsync.c */
#define IDCLS_P_VALUE                           0x11222
#define IDCLS_P_VALUE_DE                        0x11223
#define IDCLS_P_VALUE_FR                        0x11224
#define IDCLS_P_VALUE_IT                        0x11225
#define IDCLS_P_VALUE_NL                        0x11226
#define IDCLS_P_VALUE_PL                        0x11227
#define IDCLS_P_VALUE_SV                        0x11228

/* c64/cart/ide64.c */
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64        0x11229
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_DE     0x1122a
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_FR     0x1122b
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_IT     0x1122c
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_NL     0x1122d
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_PL     0x1122e
#define IDCLS_SET_AMOUNT_CYLINDERS_IDE64_SV     0x1122f

/* c64/cart/ide64.c */
#define IDCLS_SET_AMOUNT_HEADS_IDE64            0x11230
#define IDCLS_SET_AMOUNT_HEADS_IDE64_DE         0x11231
#define IDCLS_SET_AMOUNT_HEADS_IDE64_FR         0x11232
#define IDCLS_SET_AMOUNT_HEADS_IDE64_IT         0x11233
#define IDCLS_SET_AMOUNT_HEADS_IDE64_NL         0x11234
#define IDCLS_SET_AMOUNT_HEADS_IDE64_PL         0x11235
#define IDCLS_SET_AMOUNT_HEADS_IDE64_SV         0x11236

/* c64/cart/ide64.c */
#define IDCLS_SET_AMOUNT_SECTORS_IDE64          0x11237
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_DE       0x11238
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_FR       0x11239
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_IT       0x1123a
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_NL       0x1123b
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_PL       0x1123c
#define IDCLS_SET_AMOUNT_SECTORS_IDE64_SV       0x1123d

/* c64/cart/ide64.c */
#define IDCLS_AUTODETECT_IDE64_GEOMETRY         0x1123e
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_DE      0x1123f
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_FR      0x11240
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_IT      0x11241
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_NL      0x11242
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_PL      0x11243
#define IDCLS_AUTODETECT_IDE64_GEOMETRY_SV      0x11244

/* c64/cart/ide64.c */
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

/* drive/drive-cmdline-options.c, printerdrv/interface-serial.c,
   attach.c */
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

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME         0x11329
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_DE      0x1132a
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_FR      0x1132b
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_IT      0x1132c
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_NL      0x1132d
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_PL      0x1132e
#define IDCLS_SPECIFY_1541_DOS_ROM_NAME_SV      0x1132f

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME      0x11330
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_DE   0x11331
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_FR   0x11332
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_IT   0x11333
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_NL   0x11334
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_PL   0x11335
#define IDCLS_SPECIFY_1541_II_DOS_ROM_NAME_SV   0x11336

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME         0x11337
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_DE      0x11338
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_FR      0x11339
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_IT      0x1133a
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_NL      0x1133b
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_PL      0x1133c
#define IDCLS_SPECIFY_1570_DOS_ROM_NAME_SV      0x1133d

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME         0x1133e
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_DE      0x1133f
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_FR      0x11340
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_IT      0x11341
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_NL      0x11342
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_PL      0x11343
#define IDCLS_SPECIFY_1571_DOS_ROM_NAME_SV      0x11344

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME         0x11345
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_DE      0x11346
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_FR      0x11347
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_IT      0x11348
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_NL      0x11349
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_PL      0x1134a
#define IDCLS_SPECIFY_1581_DOS_ROM_NAME_SV      0x1134b

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_PAR_CABLE                  0x1134c
#define IDCLS_ENABLE_PAR_CABLE_DE               0x1134d
#define IDCLS_ENABLE_PAR_CABLE_FR               0x1134e
#define IDCLS_ENABLE_PAR_CABLE_IT               0x1134f
#define IDCLS_ENABLE_PAR_CABLE_NL               0x11350
#define IDCLS_ENABLE_PAR_CABLE_PL               0x11351
#define IDCLS_ENABLE_PAR_CABLE_SV               0x11352

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_PAR_CABLE                 0x11353
#define IDCLS_DISABLE_PAR_CABLE_DE              0x11354
#define IDCLS_DISABLE_PAR_CABLE_FR              0x11355
#define IDCLS_DISABLE_PAR_CABLE_IT              0x11356
#define IDCLS_DISABLE_PAR_CABLE_NL              0x11357
#define IDCLS_DISABLE_PAR_CABLE_PL              0x11358
#define IDCLS_DISABLE_PAR_CABLE_SV              0x11359

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_SET_IDLE_METHOD                   0x1135a
#define IDCLS_SET_IDLE_METHOD_DE                0x1135b
#define IDCLS_SET_IDLE_METHOD_FR                0x1135c
#define IDCLS_SET_IDLE_METHOD_IT                0x1135d
#define IDCLS_SET_IDLE_METHOD_NL                0x1135e
#define IDCLS_SET_IDLE_METHOD_PL                0x1135f
#define IDCLS_SET_IDLE_METHOD_SV                0x11360

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_2000             0x11361
#define IDCLS_ENABLE_DRIVE_RAM_2000_DE          0x11362
#define IDCLS_ENABLE_DRIVE_RAM_2000_FR          0x11363
#define IDCLS_ENABLE_DRIVE_RAM_2000_IT          0x11364
#define IDCLS_ENABLE_DRIVE_RAM_2000_NL          0x11365
#define IDCLS_ENABLE_DRIVE_RAM_2000_PL          0x11366
#define IDCLS_ENABLE_DRIVE_RAM_2000_SV          0x11367

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_2000            0x11368
#define IDCLS_DISABLE_DRIVE_RAM_2000_DE         0x11369
#define IDCLS_DISABLE_DRIVE_RAM_2000_FR         0x1136a
#define IDCLS_DISABLE_DRIVE_RAM_2000_IT         0x1136b
#define IDCLS_DISABLE_DRIVE_RAM_2000_NL         0x1136c
#define IDCLS_DISABLE_DRIVE_RAM_2000_PL         0x1136d
#define IDCLS_DISABLE_DRIVE_RAM_2000_SV         0x1136e

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_4000             0x1136f
#define IDCLS_ENABLE_DRIVE_RAM_4000_DE          0x11370
#define IDCLS_ENABLE_DRIVE_RAM_4000_FR          0x11371
#define IDCLS_ENABLE_DRIVE_RAM_4000_IT          0x11372
#define IDCLS_ENABLE_DRIVE_RAM_4000_NL          0x11373
#define IDCLS_ENABLE_DRIVE_RAM_4000_PL          0x11374
#define IDCLS_ENABLE_DRIVE_RAM_4000_SV          0x11375

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_4000            0x11376
#define IDCLS_DISABLE_DRIVE_RAM_4000_DE         0x11377
#define IDCLS_DISABLE_DRIVE_RAM_4000_FR         0x11378
#define IDCLS_DISABLE_DRIVE_RAM_4000_IT         0x11379
#define IDCLS_DISABLE_DRIVE_RAM_4000_NL         0x1137a
#define IDCLS_DISABLE_DRIVE_RAM_4000_PL         0x1137b
#define IDCLS_DISABLE_DRIVE_RAM_4000_SV         0x1137c

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_6000             0x1137d
#define IDCLS_ENABLE_DRIVE_RAM_6000_DE          0x1137e
#define IDCLS_ENABLE_DRIVE_RAM_6000_FR          0x1137f
#define IDCLS_ENABLE_DRIVE_RAM_6000_IT          0x11380
#define IDCLS_ENABLE_DRIVE_RAM_6000_NL          0x11381
#define IDCLS_ENABLE_DRIVE_RAM_6000_PL          0x11382
#define IDCLS_ENABLE_DRIVE_RAM_6000_SV          0x11383

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_6000            0x11384
#define IDCLS_DISABLE_DRIVE_RAM_6000_DE         0x11385
#define IDCLS_DISABLE_DRIVE_RAM_6000_FR         0x11386
#define IDCLS_DISABLE_DRIVE_RAM_6000_IT         0x11387
#define IDCLS_DISABLE_DRIVE_RAM_6000_NL         0x11388
#define IDCLS_DISABLE_DRIVE_RAM_6000_PL         0x11389
#define IDCLS_DISABLE_DRIVE_RAM_6000_SV         0x1138a

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_8000             0x1138b
#define IDCLS_ENABLE_DRIVE_RAM_8000_DE          0x1138c
#define IDCLS_ENABLE_DRIVE_RAM_8000_FR          0x1138d
#define IDCLS_ENABLE_DRIVE_RAM_8000_IT          0x1138e
#define IDCLS_ENABLE_DRIVE_RAM_8000_NL          0x1138f
#define IDCLS_ENABLE_DRIVE_RAM_8000_PL          0x11390
#define IDCLS_ENABLE_DRIVE_RAM_8000_SV          0x11391

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_8000            0x11392
#define IDCLS_DISABLE_DRIVE_RAM_8000_DE         0x11393
#define IDCLS_DISABLE_DRIVE_RAM_8000_FR         0x11394
#define IDCLS_DISABLE_DRIVE_RAM_8000_IT         0x11395
#define IDCLS_DISABLE_DRIVE_RAM_8000_NL         0x11396
#define IDCLS_DISABLE_DRIVE_RAM_8000_PL         0x11397
#define IDCLS_DISABLE_DRIVE_RAM_8000_SV         0x11398

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_ENABLE_DRIVE_RAM_A000             0x11399
#define IDCLS_ENABLE_DRIVE_RAM_A000_DE          0x1139a
#define IDCLS_ENABLE_DRIVE_RAM_A000_FR          0x1139b
#define IDCLS_ENABLE_DRIVE_RAM_A000_IT          0x1139c
#define IDCLS_ENABLE_DRIVE_RAM_A000_NL          0x1139d
#define IDCLS_ENABLE_DRIVE_RAM_A000_PL          0x1139e
#define IDCLS_ENABLE_DRIVE_RAM_A000_SV          0x1139f

/* drive/iec/iec-cmdline-options.c */
#define IDCLS_DISABLE_DRIVE_RAM_A000            0x113a0
#define IDCLS_DISABLE_DRIVE_RAM_A000_DE         0x113a1
#define IDCLS_DISABLE_DRIVE_RAM_A000_FR         0x113a2
#define IDCLS_DISABLE_DRIVE_RAM_A000_IT         0x113a3
#define IDCLS_DISABLE_DRIVE_RAM_A000_NL         0x113a4
#define IDCLS_DISABLE_DRIVE_RAM_A000_PL         0x113a5
#define IDCLS_DISABLE_DRIVE_RAM_A000_SV         0x113a6

/* drive/iec128dcr/iec128dcr-cmdline-options.c */
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME       0x113a7
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_DE    0x113a8
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_FR    0x113a9
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_IT    0x113aa
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_NL    0x113ab
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_PL    0x113ac
#define IDCLS_SPECIFY_1571CR_DOS_ROM_NAME_SV    0x113ad

/* drive/ieee/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME         0x113ae
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_DE      0x113af
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_FR      0x113b0
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_IT      0x113b1
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_NL      0x113b2
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_PL      0x113b3
#define IDCLS_SPECIFY_2031_DOS_ROM_NAME_SV      0x113b4

/* drive/ieee/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME         0x113b5
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_DE      0x113b6
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_FR      0x113b7
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_IT      0x113b8
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_NL      0x113b9
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_PL      0x113ba
#define IDCLS_SPECIFY_2040_DOS_ROM_NAME_SV      0x113bb

/* drive/ieee/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME         0x113bc
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_DE      0x113bd
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_FR      0x113be
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_IT      0x113bf
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_NL      0x113c0
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_PL      0x113c1
#define IDCLS_SPECIFY_3040_DOS_ROM_NAME_SV      0x113c2

/* drive/ieee/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME         0x113c3
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_DE      0x113c4
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_FR      0x113c5
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_IT      0x113c6
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_NL      0x113c7
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_PL      0x113c8
#define IDCLS_SPECIFY_4040_DOS_ROM_NAME_SV      0x113c9

/* drive/ieee/ieee-cmdline-options.c */
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME         0x113ca
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_DE      0x113cb
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_FR      0x113cc
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_IT      0x113cd
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_NL      0x113ce
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_PL      0x113cf
#define IDCLS_SPECIFY_1001_DOS_ROM_NAME_SV      0x113d0

/* drive/tcbm/tcbm-cmdline-options.c */
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

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
#define IDCLS_P_0_2000                          0x114b8
#define IDCLS_P_0_2000_DE                       0x114b9
#define IDCLS_P_0_2000_FR                       0x114ba
#define IDCLS_P_0_2000_IT                       0x114bb
#define IDCLS_P_0_2000_NL                       0x114bc
#define IDCLS_P_0_2000_PL                       0x114bd
#define IDCLS_P_0_2000_SV                       0x114be

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
#define IDCLS_SET_SATURATION                    0x114bf
#define IDCLS_SET_SATURATION_DE                 0x114c0
#define IDCLS_SET_SATURATION_FR                 0x114c1
#define IDCLS_SET_SATURATION_IT                 0x114c2
#define IDCLS_SET_SATURATION_NL                 0x114c3
#define IDCLS_SET_SATURATION_PL                 0x114c4
#define IDCLS_SET_SATURATION_SV                 0x114c5

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
#define IDCLS_SET_CONTRAST                      0x114c6
#define IDCLS_SET_CONTRAST_DE                   0x114c7
#define IDCLS_SET_CONTRAST_FR                   0x114c8
#define IDCLS_SET_CONTRAST_IT                   0x114c9
#define IDCLS_SET_CONTRAST_NL                   0x114ca
#define IDCLS_SET_CONTRAST_PL                   0x114cb
#define IDCLS_SET_CONTRAST_SV                   0x114cc

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
#define IDCLS_SET_BRIGHTNESS                    0x114cd
#define IDCLS_SET_BRIGHTNESS_DE                 0x114ce
#define IDCLS_SET_BRIGHTNESS_FR                 0x114cf
#define IDCLS_SET_BRIGHTNESS_IT                 0x114d0
#define IDCLS_SET_BRIGHTNESS_NL                 0x114d1
#define IDCLS_SET_BRIGHTNESS_PL                 0x114d2
#define IDCLS_SET_BRIGHTNESS_SV                 0x114d3

/* plus4/ted-cmdline-options.c, vicii/vicii-cmdline-options.c */
#define IDCLS_SET_GAMMA                         0x114d4
#define IDCLS_SET_GAMMA_DE                      0x114d5
#define IDCLS_SET_GAMMA_FR                      0x114d6
#define IDCLS_SET_GAMMA_IT                      0x114d7
#define IDCLS_SET_GAMMA_NL                      0x114d8
#define IDCLS_SET_GAMMA_PL                      0x114d9
#define IDCLS_SET_GAMMA_SV                      0x114da

/* printerdrv/driver-select.c */
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME         0x114db
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_DE      0x114dc
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_FR      0x114dd
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_IT      0x114de
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_NL      0x114df
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_PL      0x114e0
#define IDCLS_SPECIFY_PRT_DRIVER_4_NAME_SV      0x114e1

/* printerdrv/driver-select.c */
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME         0x114e2
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_DE      0x114e3
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_FR      0x114e4
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_IT      0x114e5
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_NL      0x114e6
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_PL      0x114e7
#define IDCLS_SPECIFY_PRT_DRIVER_5_NAME_SV      0x114e8

/* printerdrv/driver-select.c */
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME       0x114e9
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_DE    0x114ea
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_FR    0x114eb
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_IT    0x114ec
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_NL    0x114ed
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_PL    0x114ee
#define IDCLS_SPECIFY_PRT_DRIVER_USR_NAME_SV    0x114ef

/* printerdrv/interface-serial.c */
#define IDCLS_SET_DEVICE_TYPE_4                 0x114f0
#define IDCLS_SET_DEVICE_TYPE_4_DE              0x114f1
#define IDCLS_SET_DEVICE_TYPE_4_FR              0x114f2
#define IDCLS_SET_DEVICE_TYPE_4_IT              0x114f3
#define IDCLS_SET_DEVICE_TYPE_4_NL              0x114f4
#define IDCLS_SET_DEVICE_TYPE_4_PL              0x114f5
#define IDCLS_SET_DEVICE_TYPE_4_SV              0x114f6

/* printerdrv/interface-serial.c */
#define IDCLS_SET_DEVICE_TYPE_5                 0x114f7
#define IDCLS_SET_DEVICE_TYPE_5_DE              0x114f8
#define IDCLS_SET_DEVICE_TYPE_5_FR              0x114f9
#define IDCLS_SET_DEVICE_TYPE_5_IT              0x114fa
#define IDCLS_SET_DEVICE_TYPE_5_NL              0x114fb
#define IDCLS_SET_DEVICE_TYPE_5_PL              0x114fc
#define IDCLS_SET_DEVICE_TYPE_5_SV              0x114fd

/* printerdrv/interface-userport.c */
#define IDCLS_ENABLE_USERPORT_PRINTER           0x114fe
#define IDCLS_ENABLE_USERPORT_PRINTER_DE        0x114ff
#define IDCLS_ENABLE_USERPORT_PRINTER_FR        0x11500
#define IDCLS_ENABLE_USERPORT_PRINTER_IT        0x11501
#define IDCLS_ENABLE_USERPORT_PRINTER_NL        0x11502
#define IDCLS_ENABLE_USERPORT_PRINTER_PL        0x11503
#define IDCLS_ENABLE_USERPORT_PRINTER_SV        0x11504

/* printerdrv/interface-userport.c */
#define IDCLS_DISABLE_USERPORT_PRINTER          0x11505
#define IDCLS_DISABLE_USERPORT_PRINTER_DE       0x11506
#define IDCLS_DISABLE_USERPORT_PRINTER_FR       0x11507
#define IDCLS_DISABLE_USERPORT_PRINTER_IT       0x11508
#define IDCLS_DISABLE_USERPORT_PRINTER_NL       0x11509
#define IDCLS_DISABLE_USERPORT_PRINTER_PL       0x1150a
#define IDCLS_DISABLE_USERPORT_PRINTER_SV       0x1150b

/* printerdrv/output-select.c */
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME      0x1150c
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_DE   0x1150d
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_FR   0x1150e
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_IT   0x1150f
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_NL   0x11510
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_PL   0x11511
#define IDCLS_SPECIFY_OUTPUT_DEVICE_4_NAME_SV   0x11512

/* printerdrv/output-select.c */
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME      0x11513
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_DE   0x11514
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_FR   0x11515
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_IT   0x11516
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_NL   0x11517
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_PL   0x11518
#define IDCLS_SPECIFY_OUTPUT_DEVICE_5_NAME_SV   0x11519

/* printerdrv/output-select.c */
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME    0x1151a
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_DE 0x1151b
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_FR 0x1151c
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_IT 0x1151d
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_NL 0x1151e
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_PL 0x1151f
#define IDCLS_SPECIFY_OUTPUT_DEVICE_USR_NAME_SV 0x11520

/* printerdrv/output-text.c */
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME     0x11521
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_DE  0x11522
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_FR  0x11523
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_IT  0x11524
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_NL  0x11525
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_PL  0x11526
#define IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME_SV  0x11527

/* printerdrv/output-text.c */
#define IDCLS_P_0_2                             0x11528
#define IDCLS_P_0_2_DE                          0x11529
#define IDCLS_P_0_2_FR                          0x1152a
#define IDCLS_P_0_2_IT                          0x1152b
#define IDCLS_P_0_2_NL                          0x1152c
#define IDCLS_P_0_2_PL                          0x1152d
#define IDCLS_P_0_2_SV                          0x1152e

/* printerdrv/output-text.c */
#define IDCLS_SPECIFY_TEXT_DEVICE_4             0x1152f
#define IDCLS_SPECIFY_TEXT_DEVICE_4_DE          0x11530
#define IDCLS_SPECIFY_TEXT_DEVICE_4_FR          0x11531
#define IDCLS_SPECIFY_TEXT_DEVICE_4_IT          0x11532
#define IDCLS_SPECIFY_TEXT_DEVICE_4_NL          0x11533
#define IDCLS_SPECIFY_TEXT_DEVICE_4_PL          0x11534
#define IDCLS_SPECIFY_TEXT_DEVICE_4_SV          0x11535

/* printerdrv/output-text.c */
#define IDCLS_SPECIFY_TEXT_DEVICE_5             0x11536
#define IDCLS_SPECIFY_TEXT_DEVICE_5_DE          0x11537
#define IDCLS_SPECIFY_TEXT_DEVICE_5_FR          0x11538
#define IDCLS_SPECIFY_TEXT_DEVICE_5_IT          0x11539
#define IDCLS_SPECIFY_TEXT_DEVICE_5_NL          0x1153a
#define IDCLS_SPECIFY_TEXT_DEVICE_5_PL          0x1153b
#define IDCLS_SPECIFY_TEXT_DEVICE_5_SV          0x1153c

/* printerdrv/output-text.c */
#define IDCLS_SPECIFY_TEXT_USERPORT             0x1153d
#define IDCLS_SPECIFY_TEXT_USERPORT_DE          0x1153e
#define IDCLS_SPECIFY_TEXT_USERPORT_FR          0x1153f
#define IDCLS_SPECIFY_TEXT_USERPORT_IT          0x11540
#define IDCLS_SPECIFY_TEXT_USERPORT_NL          0x11541
#define IDCLS_SPECIFY_TEXT_USERPORT_PL          0x11542
#define IDCLS_SPECIFY_TEXT_USERPORT_SV          0x11543

/* raster/raster-cmdline-options.c */
#define IDCLS_ENABLE_VIDEO_CACHE                0x11544
#define IDCLS_ENABLE_VIDEO_CACHE_DE             0x11545
#define IDCLS_ENABLE_VIDEO_CACHE_FR             0x11546
#define IDCLS_ENABLE_VIDEO_CACHE_IT             0x11547
#define IDCLS_ENABLE_VIDEO_CACHE_NL             0x11548
#define IDCLS_ENABLE_VIDEO_CACHE_PL             0x11549
#define IDCLS_ENABLE_VIDEO_CACHE_SV             0x1154a

/* raster/raster-cmdline-options.c */
#define IDCLS_DISABLE_VIDEO_CACHE               0x1154b
#define IDCLS_DISABLE_VIDEO_CACHE_DE            0x1154c
#define IDCLS_DISABLE_VIDEO_CACHE_FR            0x1154d
#define IDCLS_DISABLE_VIDEO_CACHE_IT            0x1154e
#define IDCLS_DISABLE_VIDEO_CACHE_NL            0x1154f
#define IDCLS_DISABLE_VIDEO_CACHE_PL            0x11550
#define IDCLS_DISABLE_VIDEO_CACHE_SV            0x11551

#ifdef HAVE_RS232
/* rs232drv/rs232drv.c */
#define IDCLS_SPECIFY_RS232_1_NAME              0x11552
#define IDCLS_SPECIFY_RS232_1_NAME_DE           0x11553
#define IDCLS_SPECIFY_RS232_1_NAME_FR           0x11554
#define IDCLS_SPECIFY_RS232_1_NAME_IT           0x11555
#define IDCLS_SPECIFY_RS232_1_NAME_NL           0x11556
#define IDCLS_SPECIFY_RS232_1_NAME_PL           0x11557
#define IDCLS_SPECIFY_RS232_1_NAME_SV           0x11558

/* rs232drv/rs232drv.c */
#define IDCLS_SPECIFY_RS232_2_NAME              0x11559
#define IDCLS_SPECIFY_RS232_2_NAME_DE           0x1155a
#define IDCLS_SPECIFY_RS232_2_NAME_FR           0x1155b
#define IDCLS_SPECIFY_RS232_2_NAME_IT           0x1155c
#define IDCLS_SPECIFY_RS232_2_NAME_NL           0x1155d
#define IDCLS_SPECIFY_RS232_2_NAME_PL           0x1155e
#define IDCLS_SPECIFY_RS232_2_NAME_SV           0x1155f

/* rs232drv/rs232drv.c */
#define IDCLS_SPECIFY_RS232_3_NAME              0x11560
#define IDCLS_SPECIFY_RS232_3_NAME_DE           0x11561
#define IDCLS_SPECIFY_RS232_3_NAME_FR           0x11562
#define IDCLS_SPECIFY_RS232_3_NAME_IT           0x11563
#define IDCLS_SPECIFY_RS232_3_NAME_NL           0x11564
#define IDCLS_SPECIFY_RS232_3_NAME_PL           0x11565
#define IDCLS_SPECIFY_RS232_3_NAME_SV           0x11566

/* rs232drv/rs232drv.c */
#define IDCLS_SPECIFY_RS232_4_NAME              0x11567
#define IDCLS_SPECIFY_RS232_4_NAME_DE           0x11568
#define IDCLS_SPECIFY_RS232_4_NAME_FR           0x11569
#define IDCLS_SPECIFY_RS232_4_NAME_IT           0x1156a
#define IDCLS_SPECIFY_RS232_4_NAME_NL           0x1156b
#define IDCLS_SPECIFY_RS232_4_NAME_PL           0x1156c
#define IDCLS_SPECIFY_RS232_4_NAME_SV           0x1156d
#endif

/* rs232drv/rsuser.c */
#define IDCLS_ENABLE_RS232_USERPORT             0x1156e
#define IDCLS_ENABLE_RS232_USERPORT_DE          0x1156f
#define IDCLS_ENABLE_RS232_USERPORT_FR          0x11570
#define IDCLS_ENABLE_RS232_USERPORT_IT          0x11571
#define IDCLS_ENABLE_RS232_USERPORT_NL          0x11572
#define IDCLS_ENABLE_RS232_USERPORT_PL          0x11573
#define IDCLS_ENABLE_RS232_USERPORT_SV          0x11574

/* rs232drv/rsuser.c */
#define IDCLS_DISABLE_RS232_USERPORT            0x11575
#define IDCLS_DISABLE_RS232_USERPORT_DE         0x11576
#define IDCLS_DISABLE_RS232_USERPORT_FR         0x11577
#define IDCLS_DISABLE_RS232_USERPORT_IT         0x11578
#define IDCLS_DISABLE_RS232_USERPORT_NL         0x11579
#define IDCLS_DISABLE_RS232_USERPORT_PL         0x1157a
#define IDCLS_DISABLE_RS232_USERPORT_SV         0x1157b

/* rs232drv/rsuser.c */
#define IDCLS_P_BAUD                            0x1157c
#define IDCLS_P_BAUD_DE                         0x1157d
#define IDCLS_P_BAUD_FR                         0x1157e
#define IDCLS_P_BAUD_IT                         0x1157f
#define IDCLS_P_BAUD_NL                         0x11580
#define IDCLS_P_BAUD_PL                         0x11581
#define IDCLS_P_BAUD_SV                         0x11582

/* rs232drv/rsuser.c */
#define IDCLS_SET_BAUD_RS232_USERPORT           0x11583
#define IDCLS_SET_BAUD_RS232_USERPORT_DE        0x11584
#define IDCLS_SET_BAUD_RS232_USERPORT_FR        0x11585
#define IDCLS_SET_BAUD_RS232_USERPORT_IT        0x11586
#define IDCLS_SET_BAUD_RS232_USERPORT_NL        0x11587
#define IDCLS_SET_BAUD_RS232_USERPORT_PL        0x11588
#define IDCLS_SET_BAUD_RS232_USERPORT_SV        0x11589

/* rs232drv/rsuser.c, aciacore.c */
#define IDCLS_P_0_3                             0x1158a
#define IDCLS_P_0_3_DE                          0x1158b
#define IDCLS_P_0_3_FR                          0x1158c
#define IDCLS_P_0_3_IT                          0x1158d
#define IDCLS_P_0_3_NL                          0x1158e
#define IDCLS_P_0_3_PL                          0x1158f
#define IDCLS_P_0_3_SV                          0x11590

/* rs232drv/rsuser.c */
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT     0x11591
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_DE  0x11592
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_FR  0x11593
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_IT  0x11594
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_NL  0x11595
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_PL  0x11596
#define IDCLS_SPECIFY_RS232_DEVICE_USERPORT_SV  0x11597

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_4                      0x11598
#define IDCLS_ENABLE_IEC_4_DE                   0x11599
#define IDCLS_ENABLE_IEC_4_FR                   0x1159a
#define IDCLS_ENABLE_IEC_4_IT                   0x1159b
#define IDCLS_ENABLE_IEC_4_NL                   0x1159c
#define IDCLS_ENABLE_IEC_4_PL                   0x1159d
#define IDCLS_ENABLE_IEC_4_SV                   0x1159e

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_4                     0x1159f
#define IDCLS_DISABLE_IEC_4_DE                  0x115a0
#define IDCLS_DISABLE_IEC_4_FR                  0x115a1
#define IDCLS_DISABLE_IEC_4_IT                  0x115a2
#define IDCLS_DISABLE_IEC_4_NL                  0x115a3
#define IDCLS_DISABLE_IEC_4_PL                  0x115a4
#define IDCLS_DISABLE_IEC_4_SV                  0x115a5

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_5                      0x115a6
#define IDCLS_ENABLE_IEC_5_DE                   0x115a7
#define IDCLS_ENABLE_IEC_5_FR                   0x115a8
#define IDCLS_ENABLE_IEC_5_IT                   0x115a9
#define IDCLS_ENABLE_IEC_5_NL                   0x115aa
#define IDCLS_ENABLE_IEC_5_PL                   0x115ab
#define IDCLS_ENABLE_IEC_5_SV                   0x115ac

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_5                     0x115ad
#define IDCLS_DISABLE_IEC_5_DE                  0x115ae
#define IDCLS_DISABLE_IEC_5_FR                  0x115af
#define IDCLS_DISABLE_IEC_5_IT                  0x115b0
#define IDCLS_DISABLE_IEC_5_NL                  0x115b1
#define IDCLS_DISABLE_IEC_5_PL                  0x115b2
#define IDCLS_DISABLE_IEC_5_SV                  0x115b3

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_8                      0x115b4
#define IDCLS_ENABLE_IEC_8_DE                   0x115b5
#define IDCLS_ENABLE_IEC_8_FR                   0x115b6
#define IDCLS_ENABLE_IEC_8_IT                   0x115b7
#define IDCLS_ENABLE_IEC_8_NL                   0x115b8
#define IDCLS_ENABLE_IEC_8_PL                   0x115b9
#define IDCLS_ENABLE_IEC_8_SV                   0x115ba

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_8                     0x115bb
#define IDCLS_DISABLE_IEC_8_DE                  0x115bc
#define IDCLS_DISABLE_IEC_8_FR                  0x115bd
#define IDCLS_DISABLE_IEC_8_IT                  0x115be
#define IDCLS_DISABLE_IEC_8_NL                  0x115bf
#define IDCLS_DISABLE_IEC_8_PL                  0x115c0
#define IDCLS_DISABLE_IEC_8_SV                  0x115c1

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_9                      0x115c2
#define IDCLS_ENABLE_IEC_9_DE                   0x115c3
#define IDCLS_ENABLE_IEC_9_FR                   0x115c4
#define IDCLS_ENABLE_IEC_9_IT                   0x115c5
#define IDCLS_ENABLE_IEC_9_NL                   0x115c6
#define IDCLS_ENABLE_IEC_9_PL                   0x115c7
#define IDCLS_ENABLE_IEC_9_SV                   0x115c8

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_9                     0x115c9
#define IDCLS_DISABLE_IEC_9_DE                  0x115ca
#define IDCLS_DISABLE_IEC_9_FR                  0x115cb
#define IDCLS_DISABLE_IEC_9_IT                  0x115cc
#define IDCLS_DISABLE_IEC_9_NL                  0x115cd
#define IDCLS_DISABLE_IEC_9_PL                  0x115ce
#define IDCLS_DISABLE_IEC_9_SV                  0x115cf

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_10                     0x115d0
#define IDCLS_ENABLE_IEC_10_DE                  0x115d1
#define IDCLS_ENABLE_IEC_10_FR                  0x115d2
#define IDCLS_ENABLE_IEC_10_IT                  0x115d3
#define IDCLS_ENABLE_IEC_10_NL                  0x115d4
#define IDCLS_ENABLE_IEC_10_PL                  0x115d5
#define IDCLS_ENABLE_IEC_10_SV                  0x115d6

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_10                    0x115d7
#define IDCLS_DISABLE_IEC_10_DE                 0x115d8
#define IDCLS_DISABLE_IEC_10_FR                 0x115d9
#define IDCLS_DISABLE_IEC_10_IT                 0x115da
#define IDCLS_DISABLE_IEC_10_NL                 0x115db
#define IDCLS_DISABLE_IEC_10_PL                 0x115dc
#define IDCLS_DISABLE_IEC_10_SV                 0x115dd

/* serial/serial-iec-device.c */
#define IDCLS_ENABLE_IEC_11                     0x115de
#define IDCLS_ENABLE_IEC_11_DE                  0x115df
#define IDCLS_ENABLE_IEC_11_FR                  0x115e0
#define IDCLS_ENABLE_IEC_11_IT                  0x115e1
#define IDCLS_ENABLE_IEC_11_NL                  0x115e2
#define IDCLS_ENABLE_IEC_11_PL                  0x115e3
#define IDCLS_ENABLE_IEC_11_SV                  0x115e4

/* serial/serial-iec-device.c */
#define IDCLS_DISABLE_IEC_11                    0x115e5
#define IDCLS_DISABLE_IEC_11_DE                 0x115e6
#define IDCLS_DISABLE_IEC_11_FR                 0x115e7
#define IDCLS_DISABLE_IEC_11_IT                 0x115e8
#define IDCLS_DISABLE_IEC_11_NL                 0x115e9
#define IDCLS_DISABLE_IEC_11_PL                 0x115ea
#define IDCLS_DISABLE_IEC_11_SV                 0x115eb

/* sid/sid-cmdline-options.c */
#define IDCLS_P_ENGINE                          0x115ec
#define IDCLS_P_ENGINE_DE                       0x115ed
#define IDCLS_P_ENGINE_FR                       0x115ee
#define IDCLS_P_ENGINE_IT                       0x115ef
#define IDCLS_P_ENGINE_NL                       0x115f0
#define IDCLS_P_ENGINE_PL                       0x115f1
#define IDCLS_P_ENGINE_SV                       0x115f2

/* sid/sid-cmdline-options.c */
#define IDCLS_SPECIFY_SID_ENGINE                0x115f3
#define IDCLS_SPECIFY_SID_ENGINE_DE             0x115f4
#define IDCLS_SPECIFY_SID_ENGINE_FR             0x115f5
#define IDCLS_SPECIFY_SID_ENGINE_IT             0x115f6
#define IDCLS_SPECIFY_SID_ENGINE_NL             0x115f7
#define IDCLS_SPECIFY_SID_ENGINE_PL             0x115f8
#define IDCLS_SPECIFY_SID_ENGINE_SV             0x115f9

/* sid/sid-cmdline-options.c */
#define IDCLS_ENABLE_SECOND_SID                 0x115fa
#define IDCLS_ENABLE_SECOND_SID_DE              0x115fb
#define IDCLS_ENABLE_SECOND_SID_FR              0x115fc
#define IDCLS_ENABLE_SECOND_SID_IT              0x115fd
#define IDCLS_ENABLE_SECOND_SID_NL              0x115fe
#define IDCLS_ENABLE_SECOND_SID_PL              0x115ff
#define IDCLS_ENABLE_SECOND_SID_SV              0x11600

/* sid/sid-cmdline-options.c, c64/plus60k.c, c64/c64_256k.c */
#define IDCLS_P_BASE_ADDRESS                    0x11601
#define IDCLS_P_BASE_ADDRESS_DE                 0x11602
#define IDCLS_P_BASE_ADDRESS_FR                 0x11603
#define IDCLS_P_BASE_ADDRESS_IT                 0x11604
#define IDCLS_P_BASE_ADDRESS_NL                 0x11605
#define IDCLS_P_BASE_ADDRESS_PL                 0x11606
#define IDCLS_P_BASE_ADDRESS_SV                 0x11607

/* sid/sid-cmdline-options.c */
#define IDCLS_SPECIFY_SID_2_ADDRESS             0x11608
#define IDCLS_SPECIFY_SID_2_ADDRESS_DE          0x11609
#define IDCLS_SPECIFY_SID_2_ADDRESS_FR          0x1160a
#define IDCLS_SPECIFY_SID_2_ADDRESS_IT          0x1160b
#define IDCLS_SPECIFY_SID_2_ADDRESS_NL          0x1160c
#define IDCLS_SPECIFY_SID_2_ADDRESS_PL          0x1160d
#define IDCLS_SPECIFY_SID_2_ADDRESS_SV          0x1160e

/* sid/sid-cmdline-options.c */
#define IDCLS_P_MODEL                           0x1160f
#define IDCLS_P_MODEL_DE                        0x11610
#define IDCLS_P_MODEL_FR                        0x11611
#define IDCLS_P_MODEL_IT                        0x11612
#define IDCLS_P_MODEL_NL                        0x11613
#define IDCLS_P_MODEL_PL                        0x11614
#define IDCLS_P_MODEL_SV                        0x11615

/* sid/sid-cmdline-options.c */
#define IDCLS_SPECIFY_SID_MODEL                 0x11616
#define IDCLS_SPECIFY_SID_MODEL_DE              0x11617
#define IDCLS_SPECIFY_SID_MODEL_FR              0x11618
#define IDCLS_SPECIFY_SID_MODEL_IT              0x11619
#define IDCLS_SPECIFY_SID_MODEL_NL              0x1161a
#define IDCLS_SPECIFY_SID_MODEL_PL              0x1161b
#define IDCLS_SPECIFY_SID_MODEL_SV              0x1161c

/* sid/sid-cmdline-options.c */
#define IDCLS_ENABLE_SID_FILTERS                0x1161d
#define IDCLS_ENABLE_SID_FILTERS_DE             0x1161e
#define IDCLS_ENABLE_SID_FILTERS_FR             0x1161f
#define IDCLS_ENABLE_SID_FILTERS_IT             0x11620
#define IDCLS_ENABLE_SID_FILTERS_NL             0x11621
#define IDCLS_ENABLE_SID_FILTERS_PL             0x11622
#define IDCLS_ENABLE_SID_FILTERS_SV             0x11623

/* sid/sid-cmdline-options.c */
#define IDCLS_DISABLE_SID_FILTERS               0x11624
#define IDCLS_DISABLE_SID_FILTERS_DE            0x11625
#define IDCLS_DISABLE_SID_FILTERS_FR            0x11626
#define IDCLS_DISABLE_SID_FILTERS_IT            0x11627
#define IDCLS_DISABLE_SID_FILTERS_NL            0x11628
#define IDCLS_DISABLE_SID_FILTERS_PL            0x11629
#define IDCLS_DISABLE_SID_FILTERS_SV            0x1162a

#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
#define IDCLS_RESID_SAMPLING_METHOD             0x1162b
#define IDCLS_RESID_SAMPLING_METHOD_DE          0x1162c
#define IDCLS_RESID_SAMPLING_METHOD_FR          0x1162d
#define IDCLS_RESID_SAMPLING_METHOD_IT          0x1162e
#define IDCLS_RESID_SAMPLING_METHOD_NL          0x1162f
#define IDCLS_RESID_SAMPLING_METHOD_PL          0x11630
#define IDCLS_RESID_SAMPLING_METHOD_SV          0x11631

/* sid/sid-cmdline-options.c, vsync.c */
#define IDCLS_P_PERCENT                         0x11632
#define IDCLS_P_PERCENT_DE                      0x11633
#define IDCLS_P_PERCENT_FR                      0x11634
#define IDCLS_P_PERCENT_IT                      0x11635
#define IDCLS_P_PERCENT_NL                      0x11636
#define IDCLS_P_PERCENT_PL                      0x11637
#define IDCLS_P_PERCENT_SV                      0x11638

/* sid/sid-cmdline-options.c */
#define IDCLS_PASSBAND_PERCENTAGE               0x11639
#define IDCLS_PASSBAND_PERCENTAGE_DE            0x1163a
#define IDCLS_PASSBAND_PERCENTAGE_FR            0x1163b
#define IDCLS_PASSBAND_PERCENTAGE_IT            0x1163c
#define IDCLS_PASSBAND_PERCENTAGE_NL            0x1163d
#define IDCLS_PASSBAND_PERCENTAGE_PL            0x1163e
#define IDCLS_PASSBAND_PERCENTAGE_SV            0x1163f
#endif

/* vdc/vdc-cmdline-options.c */
#define IDCLS_SET_VDC_MEMORY_16KB               0x11640
#define IDCLS_SET_VDC_MEMORY_16KB_DE            0x11641
#define IDCLS_SET_VDC_MEMORY_16KB_FR            0x11642
#define IDCLS_SET_VDC_MEMORY_16KB_IT            0x11643
#define IDCLS_SET_VDC_MEMORY_16KB_NL            0x11644
#define IDCLS_SET_VDC_MEMORY_16KB_PL            0x11645
#define IDCLS_SET_VDC_MEMORY_16KB_SV            0x11646

/* vdc/vdc-cmdline-options.c */
#define IDCLS_SET_VDC_MEMORY_64KB               0x11647
#define IDCLS_SET_VDC_MEMORY_64KB_DE            0x11648
#define IDCLS_SET_VDC_MEMORY_64KB_FR            0x11649
#define IDCLS_SET_VDC_MEMORY_64KB_IT            0x1164a
#define IDCLS_SET_VDC_MEMORY_64KB_NL            0x1164b
#define IDCLS_SET_VDC_MEMORY_64KB_PL            0x1164c
#define IDCLS_SET_VDC_MEMORY_64KB_SV            0x1164d

/* vdc/vdc-cmdline-options.c */
#define IDCLS_SET_VDC_REVISION                  0x1164e
#define IDCLS_SET_VDC_REVISION_DE               0x1164f
#define IDCLS_SET_VDC_REVISION_FR               0x11650
#define IDCLS_SET_VDC_REVISION_IT               0x11651
#define IDCLS_SET_VDC_REVISION_NL               0x11652
#define IDCLS_SET_VDC_REVISION_PL               0x11653
#define IDCLS_SET_VDC_REVISION_SV               0x11654

/* vic20/vic20-cmdline-options.c */
#define IDCLS_P_SPEC                            0x11655
#define IDCLS_P_SPEC_DE                         0x11656
#define IDCLS_P_SPEC_FR                         0x11657
#define IDCLS_P_SPEC_IT                         0x11658
#define IDCLS_P_SPEC_NL                         0x11659
#define IDCLS_P_SPEC_PL                         0x1165a
#define IDCLS_P_SPEC_SV                         0x1165b

/* vic20/vic20-cmdline-options.c */
#define IDCLS_SPECIFY_MEMORY_CONFIG             0x1165c
#define IDCLS_SPECIFY_MEMORY_CONFIG_DE          0x1165d
#define IDCLS_SPECIFY_MEMORY_CONFIG_FR          0x1165e
#define IDCLS_SPECIFY_MEMORY_CONFIG_IT          0x1165f
#define IDCLS_SPECIFY_MEMORY_CONFIG_NL          0x11660
#define IDCLS_SPECIFY_MEMORY_CONFIG_PL          0x11661
#define IDCLS_SPECIFY_MEMORY_CONFIG_SV          0x11662

/* vic20/vic20-cmdline-options.c */
#define IDCLS_ENABLE_VIC1112_IEEE488            0x11663
#define IDCLS_ENABLE_VIC1112_IEEE488_DE         0x11664
#define IDCLS_ENABLE_VIC1112_IEEE488_FR         0x11665
#define IDCLS_ENABLE_VIC1112_IEEE488_IT         0x11666
#define IDCLS_ENABLE_VIC1112_IEEE488_NL         0x11667
#define IDCLS_ENABLE_VIC1112_IEEE488_PL         0x11668
#define IDCLS_ENABLE_VIC1112_IEEE488_SV         0x11669

/* vic20/vic20-cmdline-options.c */
#define IDCLS_DISABLE_VIC1112_IEEE488           0x1166a
#define IDCLS_DISABLE_VIC1112_IEEE488_DE        0x1166b
#define IDCLS_DISABLE_VIC1112_IEEE488_FR        0x1166c
#define IDCLS_DISABLE_VIC1112_IEEE488_IT        0x1166d
#define IDCLS_DISABLE_VIC1112_IEEE488_NL        0x1166e
#define IDCLS_DISABLE_VIC1112_IEEE488_PL        0x1166f
#define IDCLS_DISABLE_VIC1112_IEEE488_SV        0x11670

/* vic20/vic20cartridge.c */
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME         0x11671
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_DE      0x11672
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_FR      0x11673
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_IT      0x11674
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_NL      0x11675
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_PL      0x11676
#define IDCLS_SPECIFY_EXT_ROM_2000_NAME_SV      0x11677

/* vic20/vic20cartridge.c */
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME         0x11678
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_DE      0x11679
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_FR      0x1167a
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_IT      0x1167b
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_NL      0x1167c
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_PL      0x1167d
#define IDCLS_SPECIFY_EXT_ROM_4000_NAME_SV      0x1167e

/* vic20/vic20cartridge.c */
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME         0x11680
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_DE      0x11681
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_FR      0x11682
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_IT      0x11683
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_NL      0x11684
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_PL      0x11685
#define IDCLS_SPECIFY_EXT_ROM_6000_NAME_SV      0x11686

/* vic20/vic20cartridge.c */
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME         0x11687
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_DE      0x11688
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_FR      0x11689
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_IT      0x1168a
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_NL      0x1168b
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_PL      0x1168c
#define IDCLS_SPECIFY_EXT_ROM_A000_NAME_SV      0x1168d

/* vic20/vic20cartridge.c */
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME         0x1168e
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_DE      0x1168f
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_FR      0x11690
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_IT      0x11691
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_NL      0x11692
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_PL      0x11693
#define IDCLS_SPECIFY_EXT_ROM_B000_NAME_SV      0x11694

/* vicii/vicii-cmdline-options.c */
#define IDCLS_ENABLE_SPRITE_BACKGROUND          0x11695
#define IDCLS_ENABLE_SPRITE_BACKGROUND_DE       0x11696
#define IDCLS_ENABLE_SPRITE_BACKGROUND_FR       0x11697
#define IDCLS_ENABLE_SPRITE_BACKGROUND_IT       0x11698
#define IDCLS_ENABLE_SPRITE_BACKGROUND_NL       0x11699
#define IDCLS_ENABLE_SPRITE_BACKGROUND_PL       0x1169a
#define IDCLS_ENABLE_SPRITE_BACKGROUND_SV       0x1169b

/* vicii/vicii-cmdline-options.c */
#define IDCLS_DISABLE_SPRITE_BACKGROUND         0x1169c
#define IDCLS_DISABLE_SPRITE_BACKGROUND_DE      0x1169d
#define IDCLS_DISABLE_SPRITE_BACKGROUND_FR      0x1169e
#define IDCLS_DISABLE_SPRITE_BACKGROUND_IT      0x1169f
#define IDCLS_DISABLE_SPRITE_BACKGROUND_NL      0x116a0
#define IDCLS_DISABLE_SPRITE_BACKGROUND_PL      0x116a1
#define IDCLS_DISABLE_SPRITE_BACKGROUND_SV      0x116a2

/* vicii/vicii-cmdline-options.c */
#define IDCLS_ENABLE_SPRITE_SPRITE              0x116a3
#define IDCLS_ENABLE_SPRITE_SPRITE_DE           0x116a4
#define IDCLS_ENABLE_SPRITE_SPRITE_FR           0x116a5
#define IDCLS_ENABLE_SPRITE_SPRITE_IT           0x116a6
#define IDCLS_ENABLE_SPRITE_SPRITE_NL           0x116a7
#define IDCLS_ENABLE_SPRITE_SPRITE_PL           0x116a8
#define IDCLS_ENABLE_SPRITE_SPRITE_SV           0x116a9

/* vicii/vicii-cmdline-options.c */
#define IDCLS_DISABLE_SPRITE_SPRITE             0x116aa
#define IDCLS_DISABLE_SPRITE_SPRITE_DE          0x116ab
#define IDCLS_DISABLE_SPRITE_SPRITE_FR          0x116ac
#define IDCLS_DISABLE_SPRITE_SPRITE_IT          0x116ad
#define IDCLS_DISABLE_SPRITE_SPRITE_NL          0x116ae
#define IDCLS_DISABLE_SPRITE_SPRITE_PL          0x116af
#define IDCLS_DISABLE_SPRITE_SPRITE_SV          0x116b0

/* vicii/vicii-cmdline-options.c */
#define IDCLS_USE_NEW_LUMINANCES                0x116b1
#define IDCLS_USE_NEW_LUMINANCES_DE             0x116b2
#define IDCLS_USE_NEW_LUMINANCES_FR             0x116b3
#define IDCLS_USE_NEW_LUMINANCES_IT             0x116b4
#define IDCLS_USE_NEW_LUMINANCES_NL             0x116b5
#define IDCLS_USE_NEW_LUMINANCES_PL             0x116b6
#define IDCLS_USE_NEW_LUMINANCES_SV             0x116b7

/* vicii/vicii-cmdline-options.c */
#define IDCLS_USE_OLD_LUMINANCES                0x116b8
#define IDCLS_USE_OLD_LUMINANCES_DE             0x116b9
#define IDCLS_USE_OLD_LUMINANCES_FR             0x116ba
#define IDCLS_USE_OLD_LUMINANCES_IT             0x116bb
#define IDCLS_USE_OLD_LUMINANCES_NL             0x116bc
#define IDCLS_USE_OLD_LUMINANCES_PL             0x116bd
#define IDCLS_USE_OLD_LUMINANCES_SV             0x116be

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_DOUBLE_SIZE                0x116bf
#define IDCLS_ENABLE_DOUBLE_SIZE_DE             0x116c0
#define IDCLS_ENABLE_DOUBLE_SIZE_FR             0x116c1
#define IDCLS_ENABLE_DOUBLE_SIZE_IT             0x116c2
#define IDCLS_ENABLE_DOUBLE_SIZE_NL             0x116c3
#define IDCLS_ENABLE_DOUBLE_SIZE_PL             0x116c4
#define IDCLS_ENABLE_DOUBLE_SIZE_SV             0x116c5

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_DOUBLE_SIZE               0x116c6
#define IDCLS_DISABLE_DOUBLE_SIZE_DE            0x116c7
#define IDCLS_DISABLE_DOUBLE_SIZE_FR            0x116c8
#define IDCLS_DISABLE_DOUBLE_SIZE_IT            0x116c9
#define IDCLS_DISABLE_DOUBLE_SIZE_NL            0x116ca
#define IDCLS_DISABLE_DOUBLE_SIZE_PL            0x116cb
#define IDCLS_DISABLE_DOUBLE_SIZE_SV            0x116cc

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_DOUBLE_SCAN                0x116cd
#define IDCLS_ENABLE_DOUBLE_SCAN_DE             0x116ce
#define IDCLS_ENABLE_DOUBLE_SCAN_FR             0x116cf
#define IDCLS_ENABLE_DOUBLE_SCAN_IT             0x116d0
#define IDCLS_ENABLE_DOUBLE_SCAN_NL             0x116d1
#define IDCLS_ENABLE_DOUBLE_SCAN_PL             0x116d2
#define IDCLS_ENABLE_DOUBLE_SCAN_SV             0x116d3

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_DOUBLE_SCAN               0x116d4
#define IDCLS_DISABLE_DOUBLE_SCAN_DE            0x116d5
#define IDCLS_DISABLE_DOUBLE_SCAN_FR            0x116d6
#define IDCLS_DISABLE_DOUBLE_SCAN_IT            0x116d7
#define IDCLS_DISABLE_DOUBLE_SCAN_NL            0x116d8
#define IDCLS_DISABLE_DOUBLE_SCAN_PL            0x116d9
#define IDCLS_DISABLE_DOUBLE_SCAN_SV            0x116da

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_HARDWARE_SCALING           0x116db
#define IDCLS_ENABLE_HARDWARE_SCALING_DE        0x116dc
#define IDCLS_ENABLE_HARDWARE_SCALING_FR        0x116dd
#define IDCLS_ENABLE_HARDWARE_SCALING_IT        0x116de
#define IDCLS_ENABLE_HARDWARE_SCALING_NL        0x116df
#define IDCLS_ENABLE_HARDWARE_SCALING_PL        0x116e0
#define IDCLS_ENABLE_HARDWARE_SCALING_SV        0x116e1

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_HARDWARE_SCALING          0x116e2
#define IDCLS_DISABLE_HARDWARE_SCALING_DE       0x116e3
#define IDCLS_DISABLE_HARDWARE_SCALING_FR       0x116e4
#define IDCLS_DISABLE_HARDWARE_SCALING_IT       0x116e5
#define IDCLS_DISABLE_HARDWARE_SCALING_NL       0x116e6
#define IDCLS_DISABLE_HARDWARE_SCALING_PL       0x116e7
#define IDCLS_DISABLE_HARDWARE_SCALING_SV       0x116e8

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_SCALE2X                    0x116e9
#define IDCLS_ENABLE_SCALE2X_DE                 0x116ea
#define IDCLS_ENABLE_SCALE2X_FR                 0x116eb
#define IDCLS_ENABLE_SCALE2X_IT                 0x116ec
#define IDCLS_ENABLE_SCALE2X_NL                 0x116ed
#define IDCLS_ENABLE_SCALE2X_PL                 0x116ee
#define IDCLS_ENABLE_SCALE2X_SV                 0x116ef

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_SCALE2X                   0x116f0
#define IDCLS_DISABLE_SCALE2X_DE                0x116f1
#define IDCLS_DISABLE_SCALE2X_FR                0x116f2
#define IDCLS_DISABLE_SCALE2X_IT                0x116f3
#define IDCLS_DISABLE_SCALE2X_NL                0x116f4
#define IDCLS_DISABLE_SCALE2X_PL                0x116f5
#define IDCLS_DISABLE_SCALE2X_SV                0x116f6

/* video/video-cmdline-options.c */
#define IDCLS_USE_INTERNAL_CALC_PALETTE         0x116f7
#define IDCLS_USE_INTERNAL_CALC_PALETTE_DE      0x116f8
#define IDCLS_USE_INTERNAL_CALC_PALETTE_FR      0x116f9
#define IDCLS_USE_INTERNAL_CALC_PALETTE_IT      0x116fa
#define IDCLS_USE_INTERNAL_CALC_PALETTE_NL      0x116fb
#define IDCLS_USE_INTERNAL_CALC_PALETTE_PL      0x116fc
#define IDCLS_USE_INTERNAL_CALC_PALETTE_SV      0x116fd

/* video/video-cmdline-options.c */
#define IDCLS_USE_EXTERNAL_FILE_PALETTE         0x116fe
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_DE      0x116ff
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_FR      0x11700
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_IT      0x11701
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_NL      0x11702
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_PL      0x11703
#define IDCLS_USE_EXTERNAL_FILE_PALETTE_SV      0x11704

/* video/video-cmdline-options.c */
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME     0x11705
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_DE  0x11706
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_FR  0x11707
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_IT  0x11708
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_NL  0x11709
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_PL  0x1170a
#define IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME_SV  0x1170b

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_FULLSCREEN_MODE            0x1170c
#define IDCLS_ENABLE_FULLSCREEN_MODE_DE         0x1170d
#define IDCLS_ENABLE_FULLSCREEN_MODE_FR         0x1170e
#define IDCLS_ENABLE_FULLSCREEN_MODE_IT         0x1170f
#define IDCLS_ENABLE_FULLSCREEN_MODE_NL         0x11710
#define IDCLS_ENABLE_FULLSCREEN_MODE_PL         0x11711
#define IDCLS_ENABLE_FULLSCREEN_MODE_SV         0x11712

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_FULLSCREEN_MODE           0x11713
#define IDCLS_DISABLE_FULLSCREEN_MODE_DE        0x11714
#define IDCLS_DISABLE_FULLSCREEN_MODE_FR        0x11715
#define IDCLS_DISABLE_FULLSCREEN_MODE_IT        0x11716
#define IDCLS_DISABLE_FULLSCREEN_MODE_NL        0x11717
#define IDCLS_DISABLE_FULLSCREEN_MODE_PL        0x11718
#define IDCLS_DISABLE_FULLSCREEN_MODE_SV        0x11719

/* video/video-cmdline-options.c */
#define IDCLS_P_DEVICE                          0x1171a
#define IDCLS_P_DEVICE_DE                       0x1171b
#define IDCLS_P_DEVICE_FR                       0x1171c
#define IDCLS_P_DEVICE_IT                       0x1171d
#define IDCLS_P_DEVICE_NL                       0x1171e
#define IDCLS_P_DEVICE_PL                       0x1171f
#define IDCLS_P_DEVICE_SV                       0x11720

/* video/video-cmdline-options.c */
#define IDCLS_SELECT_FULLSCREEN_DEVICE          0x11721
#define IDCLS_SELECT_FULLSCREEN_DEVICE_DE       0x11722
#define IDCLS_SELECT_FULLSCREEN_DEVICE_FR       0x11723
#define IDCLS_SELECT_FULLSCREEN_DEVICE_IT       0x11724
#define IDCLS_SELECT_FULLSCREEN_DEVICE_NL       0x11725
#define IDCLS_SELECT_FULLSCREEN_DEVICE_PL       0x11726
#define IDCLS_SELECT_FULLSCREEN_DEVICE_SV       0x11727

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN     0x11728
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_DE  0x11729
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_FR  0x1172a
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_IT  0x1172b
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_NL  0x1172c
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_PL  0x1172d
#define IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN_SV  0x1172e

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN    0x1172f
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_DE 0x11730
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_FR 0x11731
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_IT 0x11732
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_NL 0x11733
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_PL 0x11734
#define IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN_SV 0x11735

/* video/video-cmdline-options.c */
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN     0x11736
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_DE  0x11737
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_FR  0x11738
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_IT  0x11739
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_NL  0x1173a
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_PL  0x1173b
#define IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN_SV  0x1173c

/* video/video-cmdline-options.c */
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN    0x1173d
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_DE 0x1173e
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_FR 0x1173f
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_IT 0x11740
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_NL 0x11741
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_PL 0x11742
#define IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN_SV 0x11743

/* video/video-cmdline-options.c */
#define IDCLS_P_MODE                            0x11744
#define IDCLS_P_MODE_DE                         0x11745
#define IDCLS_P_MODE_FR                         0x11746
#define IDCLS_P_MODE_IT                         0x11747
#define IDCLS_P_MODE_NL                         0x11748
#define IDCLS_P_MODE_PL                         0x11749
#define IDCLS_P_MODE_SV                         0x1174a

/* video/video-cmdline-options.c */
#define IDCLS_SELECT_FULLSCREEN_MODE            0x1174b
#define IDCLS_SELECT_FULLSCREEN_MODE_DE         0x1174c
#define IDCLS_SELECT_FULLSCREEN_MODE_FR         0x1174d
#define IDCLS_SELECT_FULLSCREEN_MODE_IT         0x1174e
#define IDCLS_SELECT_FULLSCREEN_MODE_NL         0x1174f
#define IDCLS_SELECT_FULLSCREEN_MODE_PL         0x11750
#define IDCLS_SELECT_FULLSCREEN_MODE_SV         0x11751

/* aciacore.c */
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE         0x11752
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_DE      0x11753
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_FR      0x11754
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_IT      0x11755
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_NL      0x11756
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_PL      0x11757
#define IDCLS_SPECIFY_ACIA_RS232_DEVICE_SV      0x11758

/* attach.c */
#define IDCLS_SET_DEVICE_TYPE_8                 0x11759
#define IDCLS_SET_DEVICE_TYPE_8_DE              0x1175a
#define IDCLS_SET_DEVICE_TYPE_8_FR              0x1175b
#define IDCLS_SET_DEVICE_TYPE_8_IT              0x1175c
#define IDCLS_SET_DEVICE_TYPE_8_NL              0x1175d
#define IDCLS_SET_DEVICE_TYPE_8_PL              0x1175e
#define IDCLS_SET_DEVICE_TYPE_8_SV              0x1175f

/* attach.c */
#define IDCLS_SET_DEVICE_TYPE_9                 0x11760
#define IDCLS_SET_DEVICE_TYPE_9_DE              0x11761
#define IDCLS_SET_DEVICE_TYPE_9_FR              0x11762
#define IDCLS_SET_DEVICE_TYPE_9_IT              0x11763
#define IDCLS_SET_DEVICE_TYPE_9_NL              0x11764
#define IDCLS_SET_DEVICE_TYPE_9_PL              0x11765
#define IDCLS_SET_DEVICE_TYPE_9_SV              0x11766

/* attach.c */
#define IDCLS_SET_DEVICE_TYPE_10                0x11767
#define IDCLS_SET_DEVICE_TYPE_10_DE             0x11768
#define IDCLS_SET_DEVICE_TYPE_10_FR             0x11769
#define IDCLS_SET_DEVICE_TYPE_10_IT             0x1176a
#define IDCLS_SET_DEVICE_TYPE_10_NL             0x1176b
#define IDCLS_SET_DEVICE_TYPE_10_PL             0x1176c
#define IDCLS_SET_DEVICE_TYPE_10_SV             0x1176d

/* attach.c */
#define IDCLS_SET_DEVICE_TYPE_11                0x1176e
#define IDCLS_SET_DEVICE_TYPE_11_DE             0x1176f
#define IDCLS_SET_DEVICE_TYPE_11_FR             0x11770
#define IDCLS_SET_DEVICE_TYPE_11_IT             0x11771
#define IDCLS_SET_DEVICE_TYPE_11_NL             0x11772
#define IDCLS_SET_DEVICE_TYPE_11_PL             0x11773
#define IDCLS_SET_DEVICE_TYPE_11_SV             0x11774

/* attach.c */
#define IDCLS_ATTACH_READ_ONLY_8                0x11775
#define IDCLS_ATTACH_READ_ONLY_8_DE             0x11776
#define IDCLS_ATTACH_READ_ONLY_8_FR             0x11777
#define IDCLS_ATTACH_READ_ONLY_8_IT             0x11778
#define IDCLS_ATTACH_READ_ONLY_8_NL             0x11779
#define IDCLS_ATTACH_READ_ONLY_8_PL             0x1177a
#define IDCLS_ATTACH_READ_ONLY_8_SV             0x1177b

/* attach.c */
#define IDCLS_ATTACH_READ_WRITE_8               0x1177c
#define IDCLS_ATTACH_READ_WRITE_8_DE            0x1177d
#define IDCLS_ATTACH_READ_WRITE_8_FR            0x1177e
#define IDCLS_ATTACH_READ_WRITE_8_IT            0x1177f
#define IDCLS_ATTACH_READ_WRITE_8_NL            0x11780
#define IDCLS_ATTACH_READ_WRITE_8_PL            0x11781
#define IDCLS_ATTACH_READ_WRITE_8_SV            0x11782

/* attach.c */
#define IDCLS_ATTACH_READ_ONLY_9                0x11783
#define IDCLS_ATTACH_READ_ONLY_9_DE             0x11784
#define IDCLS_ATTACH_READ_ONLY_9_FR             0x11785
#define IDCLS_ATTACH_READ_ONLY_9_IT             0x11786
#define IDCLS_ATTACH_READ_ONLY_9_NL             0x11787
#define IDCLS_ATTACH_READ_ONLY_9_PL             0x11788
#define IDCLS_ATTACH_READ_ONLY_9_SV             0x11789

/* attach.c */
#define IDCLS_ATTACH_READ_WRITE_9               0x1178a
#define IDCLS_ATTACH_READ_WRITE_9_DE            0x1178b
#define IDCLS_ATTACH_READ_WRITE_9_FR            0x1178c
#define IDCLS_ATTACH_READ_WRITE_9_IT            0x1178d
#define IDCLS_ATTACH_READ_WRITE_9_NL            0x1178e
#define IDCLS_ATTACH_READ_WRITE_9_PL            0x1178f
#define IDCLS_ATTACH_READ_WRITE_9_SV            0x11790

/* attach.c */
#define IDCLS_ATTACH_READ_ONLY_10               0x11791
#define IDCLS_ATTACH_READ_ONLY_10_DE            0x11792
#define IDCLS_ATTACH_READ_ONLY_10_FR            0x11793
#define IDCLS_ATTACH_READ_ONLY_10_IT            0x11794
#define IDCLS_ATTACH_READ_ONLY_10_NL            0x11795
#define IDCLS_ATTACH_READ_ONLY_10_PL            0x11796
#define IDCLS_ATTACH_READ_ONLY_10_SV            0x11797

/* attach.c */
#define IDCLS_ATTACH_READ_WRITE_10              0x11798
#define IDCLS_ATTACH_READ_WRITE_10_DE           0x11799
#define IDCLS_ATTACH_READ_WRITE_10_FR           0x1179a
#define IDCLS_ATTACH_READ_WRITE_10_IT           0x1179b
#define IDCLS_ATTACH_READ_WRITE_10_NL           0x1179c
#define IDCLS_ATTACH_READ_WRITE_10_PL           0x1179d
#define IDCLS_ATTACH_READ_WRITE_10_SV           0x1179e

/* attach.c */
#define IDCLS_ATTACH_READ_ONLY_11               0x1179f
#define IDCLS_ATTACH_READ_ONLY_11_DE            0x117a0
#define IDCLS_ATTACH_READ_ONLY_11_FR            0x117a1
#define IDCLS_ATTACH_READ_ONLY_11_IT            0x117a2
#define IDCLS_ATTACH_READ_ONLY_11_NL            0x117a3
#define IDCLS_ATTACH_READ_ONLY_11_PL            0x117a4
#define IDCLS_ATTACH_READ_ONLY_11_SV            0x117a5

/* attach.c */
#define IDCLS_ATTACH_READ_WRITE_11              0x117a6
#define IDCLS_ATTACH_READ_WRITE_11_DE           0x117a7
#define IDCLS_ATTACH_READ_WRITE_11_FR           0x117a8
#define IDCLS_ATTACH_READ_WRITE_11_IT           0x117a9
#define IDCLS_ATTACH_READ_WRITE_11_NL           0x117aa
#define IDCLS_ATTACH_READ_WRITE_11_PL           0x117ab
#define IDCLS_ATTACH_READ_WRITE_11_SV           0x117ac

/* datasette.c */
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET       0x117ad
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_DE    0x117ae
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_FR    0x117af
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_IT    0x117b0
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_NL    0x117b1
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_PL    0x117b2
#define IDCLS_ENABLE_AUTO_DATASETTE_RESET_SV    0x117b3

/* datasette.c */
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET      0x117b4
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_DE   0x117b5
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_FR   0x117b6
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_IT   0x117b7
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_NL   0x117b8
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_PL   0x117b9
#define IDCLS_DISABLE_AUTO_DATASETTE_RESET_SV   0x117ba

/* datasette.c */
#define IDCLS_SET_ZERO_TAP_DELAY                0x117bb
#define IDCLS_SET_ZERO_TAP_DELAY_DE             0x117bc
#define IDCLS_SET_ZERO_TAP_DELAY_FR             0x117bd
#define IDCLS_SET_ZERO_TAP_DELAY_IT             0x117be
#define IDCLS_SET_ZERO_TAP_DELAY_NL             0x117bf
#define IDCLS_SET_ZERO_TAP_DELAY_PL             0x117c0
#define IDCLS_SET_ZERO_TAP_DELAY_SV             0x117c1

/* datasette.c */
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP          0x117c2
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_DE       0x117c3
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_FR       0x117c4
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_IT       0x117c5
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_NL       0x117c6
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_PL       0x117c7
#define IDCLS_SET_CYCLES_ADDED_GAP_TAP_SV       0x117c8

#ifdef DEBUG
/* debug.c */
#define IDCLS_TRACE_MAIN_CPU                    0x117c9
#define IDCLS_TRACE_MAIN_CPU_DE                 0x117ca
#define IDCLS_TRACE_MAIN_CPU_FR                 0x117cb
#define IDCLS_TRACE_MAIN_CPU_IT                 0x117cc
#define IDCLS_TRACE_MAIN_CPU_NL                 0x117cd
#define IDCLS_TRACE_MAIN_CPU_PL                 0x117ce
#define IDCLS_TRACE_MAIN_CPU_SV                 0x117cf

/* debug.c */
#define IDCLS_DONT_TRACE_MAIN_CPU               0x117d0
#define IDCLS_DONT_TRACE_MAIN_CPU_DE            0x117d1
#define IDCLS_DONT_TRACE_MAIN_CPU_FR            0x117d2
#define IDCLS_DONT_TRACE_MAIN_CPU_IT            0x117d3
#define IDCLS_DONT_TRACE_MAIN_CPU_NL            0x117d4
#define IDCLS_DONT_TRACE_MAIN_CPU_PL            0x117d5
#define IDCLS_DONT_TRACE_MAIN_CPU_SV            0x117d6

/* debug.c */
#define IDCLS_TRACE_DRIVE0_CPU                  0x117d7
#define IDCLS_TRACE_DRIVE0_CPU_DE               0x117d8
#define IDCLS_TRACE_DRIVE0_CPU_FR               0x117d9
#define IDCLS_TRACE_DRIVE0_CPU_IT               0x117da
#define IDCLS_TRACE_DRIVE0_CPU_NL               0x117db
#define IDCLS_TRACE_DRIVE0_CPU_PL               0x117dc
#define IDCLS_TRACE_DRIVE0_CPU_SV               0x117dd

/* debug.c */
#define IDCLS_DONT_TRACE_DRIVE0_CPU             0x117de
#define IDCLS_DONT_TRACE_DRIVE0_CPU_DE          0x117df
#define IDCLS_DONT_TRACE_DRIVE0_CPU_FR          0x117e0
#define IDCLS_DONT_TRACE_DRIVE0_CPU_IT          0x117e1
#define IDCLS_DONT_TRACE_DRIVE0_CPU_NL          0x117e2
#define IDCLS_DONT_TRACE_DRIVE0_CPU_PL          0x117e3
#define IDCLS_DONT_TRACE_DRIVE0_CPU_SV          0x117e4

/* debug.c */
#define IDCLS_TRACE_DRIVE1_CPU                  0x117e5
#define IDCLS_TRACE_DRIVE1_CPU_DE               0x117e6
#define IDCLS_TRACE_DRIVE1_CPU_FR               0x117e7
#define IDCLS_TRACE_DRIVE1_CPU_IT               0x117e8
#define IDCLS_TRACE_DRIVE1_CPU_NL               0x117e9
#define IDCLS_TRACE_DRIVE1_CPU_PL               0x117ea
#define IDCLS_TRACE_DRIVE1_CPU_SV               0x117eb

/* debug.c */
#define IDCLS_DONT_TRACE_DRIVE1_CPU             0x117ec
#define IDCLS_DONT_TRACE_DRIVE1_CPU_DE          0x117ed
#define IDCLS_DONT_TRACE_DRIVE1_CPU_FR          0x117ee
#define IDCLS_DONT_TRACE_DRIVE1_CPU_IT          0x117ef
#define IDCLS_DONT_TRACE_DRIVE1_CPU_NL          0x117f0
#define IDCLS_DONT_TRACE_DRIVE1_CPU_PL          0x117f1
#define IDCLS_DONT_TRACE_DRIVE1_CPU_SV          0x117f2
#endif

#if DRIVE_NUM > 2
/* debug.c */
#define IDCLS_TRACE_DRIVE2_CPU                  0x117f3
#define IDCLS_TRACE_DRIVE2_CPU_DE               0x117f4
#define IDCLS_TRACE_DRIVE2_CPU_FR               0x117f5
#define IDCLS_TRACE_DRIVE2_CPU_IT               0x117f6
#define IDCLS_TRACE_DRIVE2_CPU_NL               0x117f7
#define IDCLS_TRACE_DRIVE2_CPU_PL               0x117f8
#define IDCLS_TRACE_DRIVE2_CPU_SV               0x117f9

/* debug.c */
#define IDCLS_DONT_TRACE_DRIVE2_CPU             0x117fa
#define IDCLS_DONT_TRACE_DRIVE2_CPU_DE          0x117fb
#define IDCLS_DONT_TRACE_DRIVE2_CPU_FR          0x117fc
#define IDCLS_DONT_TRACE_DRIVE2_CPU_IT          0x117fd
#define IDCLS_DONT_TRACE_DRIVE2_CPU_NL          0x117fe
#define IDCLS_DONT_TRACE_DRIVE2_CPU_PL          0x117ff
#define IDCLS_DONT_TRACE_DRIVE2_CPU_SV          0x11800
#endif

#if DRIVE_NUM > 3
/* debug.c */
#define IDCLS_TRACE_DRIVE3_CPU                  0x11801
#define IDCLS_TRACE_DRIVE3_CPU_DE               0x11802
#define IDCLS_TRACE_DRIVE3_CPU_FR               0x11803
#define IDCLS_TRACE_DRIVE3_CPU_IT               0x11804
#define IDCLS_TRACE_DRIVE3_CPU_NL               0x11805
#define IDCLS_TRACE_DRIVE3_CPU_PL               0x11806
#define IDCLS_TRACE_DRIVE3_CPU_SV               0x11807

/* debug.c */
#define IDCLS_DONT_TRACE_DRIVE3_CPU             0x11808
#define IDCLS_DONT_TRACE_DRIVE3_CPU_DE          0x11809
#define IDCLS_DONT_TRACE_DRIVE3_CPU_FR          0x1180a
#define IDCLS_DONT_TRACE_DRIVE3_CPU_IT          0x1180b
#define IDCLS_DONT_TRACE_DRIVE3_CPU_NL          0x1180c
#define IDCLS_DONT_TRACE_DRIVE3_CPU_PL          0x1180d
#define IDCLS_DONT_TRACE_DRIVE3_CPU_SV          0x1180e
#endif

/* debug.c */
#define IDCLS_TRACE_MODE                        0x1180f
#define IDCLS_TRACE_MODE_DE                     0x11810
#define IDCLS_TRACE_MODE_FR                     0x11811
#define IDCLS_TRACE_MODE_IT                     0x11812
#define IDCLS_TRACE_MODE_NL                     0x11813
#define IDCLS_TRACE_MODE_PL                     0x11814
#define IDCLS_TRACE_MODE_SV                     0x11815
#endif

/* event.c */
#define IDCLS_PLAYBACK_RECORDED_EVENTS          0x11816
#define IDCLS_PLAYBACK_RECORDED_EVENTS_DE       0x11817
#define IDCLS_PLAYBACK_RECORDED_EVENTS_FR       0x11818
#define IDCLS_PLAYBACK_RECORDED_EVENTS_IT       0x11819
#define IDCLS_PLAYBACK_RECORDED_EVENTS_NL       0x1181a
#define IDCLS_PLAYBACK_RECORDED_EVENTS_PL       0x1181b
#define IDCLS_PLAYBACK_RECORDED_EVENTS_SV       0x1181c

/* fliplist.c */
#define IDCLS_SPECIFY_FLIP_LIST_NAME            0x1181d
#define IDCLS_SPECIFY_FLIP_LIST_NAME_DE         0x1181e
#define IDCLS_SPECIFY_FLIP_LIST_NAME_FR         0x1181f
#define IDCLS_SPECIFY_FLIP_LIST_NAME_IT         0x11820
#define IDCLS_SPECIFY_FLIP_LIST_NAME_NL         0x11821
#define IDCLS_SPECIFY_FLIP_LIST_NAME_PL         0x11822
#define IDCLS_SPECIFY_FLIP_LIST_NAME_SV         0x11823

/* initcmdline.c */
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS         0x11824
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_DE      0x11825
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_FR      0x11826
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_IT      0x11827
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_NL      0x11828
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_PL      0x11829
#define IDCLS_SHOW_COMMAND_LINE_OPTIONS_SV      0x1182a

#if (!defined  __OS2__ && !defined __BEOS__)
/* initcmdline.c */
#define IDCLS_CONSOLE_MODE                      0x1182b
#define IDCLS_CONSOLE_MODE_DE                   0x1182c
#define IDCLS_CONSOLE_MODE_FR                   0x1182d
#define IDCLS_CONSOLE_MODE_IT                   0x1182e
#define IDCLS_CONSOLE_MODE_NL                   0x1182f
#define IDCLS_CONSOLE_MODE_PL                   0x11830
#define IDCLS_CONSOLE_MODE_SV                   0x11831

/* initcmdline.c */
#define IDCLS_ALLOW_CORE_DUMPS                  0x11832
#define IDCLS_ALLOW_CORE_DUMPS_DE               0x11833
#define IDCLS_ALLOW_CORE_DUMPS_FR               0x11834
#define IDCLS_ALLOW_CORE_DUMPS_IT               0x11835
#define IDCLS_ALLOW_CORE_DUMPS_NL               0x11836
#define IDCLS_ALLOW_CORE_DUMPS_PL               0x11837
#define IDCLS_ALLOW_CORE_DUMPS_SV               0x11838

/* initcmdline.c */
#define IDCLS_DONT_ALLOW_CORE_DUMPS             0x11839
#define IDCLS_DONT_ALLOW_CORE_DUMPS_DE          0x1183a
#define IDCLS_DONT_ALLOW_CORE_DUMPS_FR          0x1183b
#define IDCLS_DONT_ALLOW_CORE_DUMPS_IT          0x1183c
#define IDCLS_DONT_ALLOW_CORE_DUMPS_NL          0x1183d
#define IDCLS_DONT_ALLOW_CORE_DUMPS_PL          0x1183e
#define IDCLS_DONT_ALLOW_CORE_DUMPS_SV          0x1183f
#else
/* initcmdline.c */
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER       0x11840
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_DE    0x11841
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_FR    0x11842
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_IT    0x11843
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_NL    0x11844
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_PL    0x11845
#define IDCLS_DONT_CALL_EXCEPTION_HANDLER_SV    0x11846

/* initcmdline.c */
#define IDCLS_CALL_EXCEPTION_HANDLER            0x11847
#define IDCLS_CALL_EXCEPTION_HANDLER_DE         0x11848
#define IDCLS_CALL_EXCEPTION_HANDLER_FR         0x11849
#define IDCLS_CALL_EXCEPTION_HANDLER_IT         0x1184a
#define IDCLS_CALL_EXCEPTION_HANDLER_NL         0x1184b
#define IDCLS_CALL_EXCEPTION_HANDLER_PL         0x1184c
#define IDCLS_CALL_EXCEPTION_HANDLER_SV         0x1184d
#endif

/* initcmdline.c */
#define IDCLS_RESTORE_DEFAULT_SETTINGS          0x1184e
#define IDCLS_RESTORE_DEFAULT_SETTINGS_DE       0x1184f
#define IDCLS_RESTORE_DEFAULT_SETTINGS_FR       0x11850
#define IDCLS_RESTORE_DEFAULT_SETTINGS_IT       0x11851
#define IDCLS_RESTORE_DEFAULT_SETTINGS_NL       0x11852
#define IDCLS_RESTORE_DEFAULT_SETTINGS_PL       0x11853
#define IDCLS_RESTORE_DEFAULT_SETTINGS_SV       0x11854

/* initcmdline.c */
#define IDCLS_ATTACH_AND_AUTOSTART              0x11855
#define IDCLS_ATTACH_AND_AUTOSTART_DE           0x11856
#define IDCLS_ATTACH_AND_AUTOSTART_FR           0x11857
#define IDCLS_ATTACH_AND_AUTOSTART_IT           0x11858
#define IDCLS_ATTACH_AND_AUTOSTART_NL           0x11859
#define IDCLS_ATTACH_AND_AUTOSTART_PL           0x1185a
#define IDCLS_ATTACH_AND_AUTOSTART_SV           0x1185b

/* initcmdline.c */
#define IDCLS_ATTACH_AND_AUTOLOAD               0x1185c
#define IDCLS_ATTACH_AND_AUTOLOAD_DE            0x1185d
#define IDCLS_ATTACH_AND_AUTOLOAD_FR            0x1185e
#define IDCLS_ATTACH_AND_AUTOLOAD_IT            0x1185f
#define IDCLS_ATTACH_AND_AUTOLOAD_NL            0x11860
#define IDCLS_ATTACH_AND_AUTOLOAD_PL            0x11861
#define IDCLS_ATTACH_AND_AUTOLOAD_SV            0x11862

/* initcmdline.c */
#define IDCLS_ATTACH_AS_TAPE                    0x11863
#define IDCLS_ATTACH_AS_TAPE_DE                 0x11864
#define IDCLS_ATTACH_AS_TAPE_FR                 0x11865
#define IDCLS_ATTACH_AS_TAPE_IT                 0x11866
#define IDCLS_ATTACH_AS_TAPE_NL                 0x11867
#define IDCLS_ATTACH_AS_TAPE_PL                 0x11868
#define IDCLS_ATTACH_AS_TAPE_SV                 0x11869

/* initcmdline.c */
#define IDCLS_ATTACH_AS_DISK_8                  0x1186a
#define IDCLS_ATTACH_AS_DISK_8_DE               0x1186b
#define IDCLS_ATTACH_AS_DISK_8_FR               0x1186c
#define IDCLS_ATTACH_AS_DISK_8_IT               0x1186d
#define IDCLS_ATTACH_AS_DISK_8_NL               0x1186e
#define IDCLS_ATTACH_AS_DISK_8_PL               0x1186f
#define IDCLS_ATTACH_AS_DISK_8_SV               0x11870

/* initcmdline.c */
#define IDCLS_ATTACH_AS_DISK_9                  0x11871
#define IDCLS_ATTACH_AS_DISK_9_DE               0x11872
#define IDCLS_ATTACH_AS_DISK_9_FR               0x11873
#define IDCLS_ATTACH_AS_DISK_9_IT               0x11874
#define IDCLS_ATTACH_AS_DISK_9_NL               0x11875
#define IDCLS_ATTACH_AS_DISK_9_PL               0x11876
#define IDCLS_ATTACH_AS_DISK_9_SV               0x11877

/* initcmdline.c */
#define IDCLS_ATTACH_AS_DISK_10                 0x11878
#define IDCLS_ATTACH_AS_DISK_10_DE              0x11879
#define IDCLS_ATTACH_AS_DISK_10_FR              0x1187a
#define IDCLS_ATTACH_AS_DISK_10_IT              0x1187b
#define IDCLS_ATTACH_AS_DISK_10_NL              0x1187c
#define IDCLS_ATTACH_AS_DISK_10_PL              0x1187d
#define IDCLS_ATTACH_AS_DISK_10_SV              0x1187e

/* initcmdline.c */
#define IDCLS_ATTACH_AS_DISK_11                 0x1187f
#define IDCLS_ATTACH_AS_DISK_11_DE              0x11880
#define IDCLS_ATTACH_AS_DISK_11_FR              0x11881
#define IDCLS_ATTACH_AS_DISK_11_IT              0x11882
#define IDCLS_ATTACH_AS_DISK_11_NL              0x11883
#define IDCLS_ATTACH_AS_DISK_11_PL              0x11884
#define IDCLS_ATTACH_AS_DISK_11_SV              0x11885

/* kbdbuf.c */
#define IDCLS_P_STRING                          0x11886
#define IDCLS_P_STRING_DE                       0x11887
#define IDCLS_P_STRING_FR                       0x11888
#define IDCLS_P_STRING_IT                       0x11889
#define IDCLS_P_STRING_NL                       0x1188a
#define IDCLS_P_STRING_PL                       0x1188b
#define IDCLS_P_STRING_SV                       0x1188c

/* kbdbuf.c */
#define IDCLS_PUT_STRING_INTO_KEYBUF            0x1188d
#define IDCLS_PUT_STRING_INTO_KEYBUF_DE         0x1188e
#define IDCLS_PUT_STRING_INTO_KEYBUF_FR         0x1188f
#define IDCLS_PUT_STRING_INTO_KEYBUF_IT         0x11890
#define IDCLS_PUT_STRING_INTO_KEYBUF_NL         0x11891
#define IDCLS_PUT_STRING_INTO_KEYBUF_PL         0x11892
#define IDCLS_PUT_STRING_INTO_KEYBUF_SV         0x11893

/* log.c */
#define IDCLS_SPECIFY_LOG_FILE_NAME             0x11894
#define IDCLS_SPECIFY_LOG_FILE_NAME_DE          0x11895
#define IDCLS_SPECIFY_LOG_FILE_NAME_FR          0x11896
#define IDCLS_SPECIFY_LOG_FILE_NAME_IT          0x11897
#define IDCLS_SPECIFY_LOG_FILE_NAME_NL          0x11898
#define IDCLS_SPECIFY_LOG_FILE_NAME_PL          0x11899
#define IDCLS_SPECIFY_LOG_FILE_NAME_SV          0x1189a

/* mouse.c */
#define IDCLS_ENABLE_1351_MOUSE                 0x1189b
#define IDCLS_ENABLE_1351_MOUSE_DE              0x1189c
#define IDCLS_ENABLE_1351_MOUSE_FR              0x1189d
#define IDCLS_ENABLE_1351_MOUSE_IT              0x1189e
#define IDCLS_ENABLE_1351_MOUSE_NL              0x1189f
#define IDCLS_ENABLE_1351_MOUSE_PL              0x118a0
#define IDCLS_ENABLE_1351_MOUSE_SV              0x118a1

/* mouse.c */
#define IDCLS_DISABLE_1351_MOUSE                0x118a2
#define IDCLS_DISABLE_1351_MOUSE_DE             0x118a3
#define IDCLS_DISABLE_1351_MOUSE_FR             0x118a4
#define IDCLS_DISABLE_1351_MOUSE_IT             0x118a5
#define IDCLS_DISABLE_1351_MOUSE_NL             0x118a6
#define IDCLS_DISABLE_1351_MOUSE_PL             0x118a7
#define IDCLS_DISABLE_1351_MOUSE_SV             0x118a8

/* mouse.c */
#define IDCLS_SELECT_MOUSE_JOY_PORT             0x118a9
#define IDCLS_SELECT_MOUSE_JOY_PORT_DE          0x118aa
#define IDCLS_SELECT_MOUSE_JOY_PORT_FR          0x118ab
#define IDCLS_SELECT_MOUSE_JOY_PORT_IT          0x118ac
#define IDCLS_SELECT_MOUSE_JOY_PORT_NL          0x118ad
#define IDCLS_SELECT_MOUSE_JOY_PORT_PL          0x118ae
#define IDCLS_SELECT_MOUSE_JOY_PORT_SV          0x118af

/* ram.c */
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE       0x118b0
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_DE    0x118b1
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_FR    0x118b2
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_IT    0x118b3
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_NL    0x118b4
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_PL    0x118b5
#define IDCLS_SET_FIRST_RAM_ADDRESS_VALUE_SV    0x118b6

/* ram.c */
#define IDCLS_P_NUM_OF_BYTES                    0x118b7
#define IDCLS_P_NUM_OF_BYTES_DE                 0x118b8
#define IDCLS_P_NUM_OF_BYTES_FR                 0x118b9
#define IDCLS_P_NUM_OF_BYTES_IT                 0x118ba
#define IDCLS_P_NUM_OF_BYTES_NL                 0x118bb
#define IDCLS_P_NUM_OF_BYTES_PL                 0x118bc
#define IDCLS_P_NUM_OF_BYTES_SV                 0x118bd

/* ram.c */
#define IDCLS_LENGTH_BLOCK_SAME_VALUE           0x118be
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_DE        0x118bf
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_FR        0x118c0
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_IT        0x118c1
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_NL        0x118c2
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_PL        0x118c3
#define IDCLS_LENGTH_BLOCK_SAME_VALUE_SV        0x118c4

/* ram.c */
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN         0x118c5
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_DE      0x118c6
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_FR      0x118c7
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_IT      0x118c8
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_NL      0x118c9
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_PL      0x118ca
#define IDCLS_LENGTH_BLOCK_SAME_PATTERN_SV      0x118cb

/* sound.c */
#define IDCLS_ENABLE_SOUND_PLAYBACK             0x118cc
#define IDCLS_ENABLE_SOUND_PLAYBACK_DE          0x118cd
#define IDCLS_ENABLE_SOUND_PLAYBACK_FR          0x118ce
#define IDCLS_ENABLE_SOUND_PLAYBACK_IT          0x118cf
#define IDCLS_ENABLE_SOUND_PLAYBACK_NL          0x118d0
#define IDCLS_ENABLE_SOUND_PLAYBACK_PL          0x118d1
#define IDCLS_ENABLE_SOUND_PLAYBACK_SV          0x118d2

/* sound.c */
#define IDCLS_DISABLE_SOUND_PLAYBACK            0x118d3
#define IDCLS_DISABLE_SOUND_PLAYBACK_DE         0x118d4
#define IDCLS_DISABLE_SOUND_PLAYBACK_FR         0x118d5
#define IDCLS_DISABLE_SOUND_PLAYBACK_IT         0x118d6
#define IDCLS_DISABLE_SOUND_PLAYBACK_NL         0x118d7
#define IDCLS_DISABLE_SOUND_PLAYBACK_PL         0x118d8
#define IDCLS_DISABLE_SOUND_PLAYBACK_SV         0x118d9

/* sound.c */
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ          0x118da
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_DE       0x118db
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_FR       0x118dc
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_IT       0x118dd
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_NL       0x118de
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_PL       0x118df
#define IDCLS_SET_SAMPLE_RATE_VALUE_HZ_SV       0x118e0

/* sound.c */
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC        0x118e1
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_DE     0x118e2
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_FR     0x118e3
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_IT     0x118e4
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_NL     0x118e5
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_PL     0x118e6
#define IDCLS_SET_SOUND_BUFFER_SIZE_MSEC_SV     0x118e7

/* sound.c */
#define IDCLS_SPECIFY_SOUND_DRIVER              0x118e8
#define IDCLS_SPECIFY_SOUND_DRIVER_DE           0x118e9
#define IDCLS_SPECIFY_SOUND_DRIVER_FR           0x118ea
#define IDCLS_SPECIFY_SOUND_DRIVER_IT           0x118eb
#define IDCLS_SPECIFY_SOUND_DRIVER_NL           0x118ec
#define IDCLS_SPECIFY_SOUND_DRIVER_PL           0x118ed
#define IDCLS_SPECIFY_SOUND_DRIVER_SV           0x118ee

/* sound.c */
#define IDCLS_P_ARGS                            0x118ef
#define IDCLS_P_ARGS_DE                         0x118f0
#define IDCLS_P_ARGS_FR                         0x118f1
#define IDCLS_P_ARGS_IT                         0x118f2
#define IDCLS_P_ARGS_NL                         0x118f3
#define IDCLS_P_ARGS_PL                         0x118f4
#define IDCLS_P_ARGS_SV                         0x118f5

/* sound.c */
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM        0x118f6
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_DE     0x118f7
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_FR     0x118f8
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_IT     0x118f9
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_NL     0x118fa
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_PL     0x118fb
#define IDCLS_SPECIFY_SOUND_DRIVER_PARAM_SV     0x118fc

/* sound.c */
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER    0x118fd
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_DE 0x118fe
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_FR 0x118ff
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_IT 0x11900
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_NL 0x11901
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_PL 0x11902
#define IDCLS_SPECIFY_RECORDING_SOUND_DRIVER_SV 0x11903

/* sound.c */
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM    0x11904
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_DE 0x11905
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_FR 0x11906
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_IT 0x11907
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_NL 0x11908
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_PL 0x11909
#define IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM_SV 0x1190a

/* sound.c */
#define IDCLS_P_SYNC                            0x1190b
#define IDCLS_P_SYNC_DE                         0x1190c
#define IDCLS_P_SYNC_FR                         0x1190d
#define IDCLS_P_SYNC_IT                         0x1190e
#define IDCLS_P_SYNC_NL                         0x1190f
#define IDCLS_P_SYNC_PL                         0x11910
#define IDCLS_P_SYNC_SV                         0x11911

/* sound.c */
#define IDCLS_SET_SOUND_SPEED_ADJUST            0x11912
#define IDCLS_SET_SOUND_SPEED_ADJUST_DE         0x11913
#define IDCLS_SET_SOUND_SPEED_ADJUST_FR         0x11914
#define IDCLS_SET_SOUND_SPEED_ADJUST_IT         0x11915
#define IDCLS_SET_SOUND_SPEED_ADJUST_NL         0x11916
#define IDCLS_SET_SOUND_SPEED_ADJUST_PL         0x11917
#define IDCLS_SET_SOUND_SPEED_ADJUST_SV         0x11918

/* sysfile.c */
#define IDCLS_P_PATH                            0x11919
#define IDCLS_P_PATH_DE                         0x1191a
#define IDCLS_P_PATH_FR                         0x1191b
#define IDCLS_P_PATH_IT                         0x1191c
#define IDCLS_P_PATH_NL                         0x1191d
#define IDCLS_P_PATH_PL                         0x1191e
#define IDCLS_P_PATH_SV                         0x1191f

/* sysfile.c */
#define IDCLS_DEFINE_SYSTEM_FILES_PATH          0x11920
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_DE       0x11921
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_FR       0x11922
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_IT       0x11923
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_NL       0x11924
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_PL       0x11925
#define IDCLS_DEFINE_SYSTEM_FILES_PATH_SV       0x11926

/* traps.c */
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION       0x11927
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_DE    0x11928
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_FR    0x11929
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_IT    0x1192a
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_NL    0x1192b
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_PL    0x1192c
#define IDCLS_ENABLE_TRAPS_FAST_EMULATION_SV    0x1192d

/* traps.c */
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION      0x1192e
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_DE   0x1192f
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_FR   0x11930
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_IT   0x11931
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_NL   0x11932
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_PL   0x11933
#define IDCLS_DISABLE_TRAPS_FAST_EMULATION_SV   0x11934

/* vsync.c */
#define IDCLS_LIMIT_SPEED_TO_VALUE              0x11935
#define IDCLS_LIMIT_SPEED_TO_VALUE_DE           0x11936
#define IDCLS_LIMIT_SPEED_TO_VALUE_FR           0x11937
#define IDCLS_LIMIT_SPEED_TO_VALUE_IT           0x11938
#define IDCLS_LIMIT_SPEED_TO_VALUE_NL           0x11939
#define IDCLS_LIMIT_SPEED_TO_VALUE_PL           0x1193a
#define IDCLS_LIMIT_SPEED_TO_VALUE_SV           0x1193b

/* vsync.c */
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES         0x1193c
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_DE      0x1193d
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_FR      0x1193e
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_IT      0x1193f
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_NL      0x11940
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_PL      0x11941
#define IDCLS_UPDATE_EVERY_VALUE_FRAMES_SV      0x11942

/* vsync.c */
#define IDCLS_ENABLE_WARP_MODE                  0x11943
#define IDCLS_ENABLE_WARP_MODE_DE               0x11944
#define IDCLS_ENABLE_WARP_MODE_FR               0x11945
#define IDCLS_ENABLE_WARP_MODE_IT               0x11946
#define IDCLS_ENABLE_WARP_MODE_NL               0x11947
#define IDCLS_ENABLE_WARP_MODE_PL               0x11948
#define IDCLS_ENABLE_WARP_MODE_SV               0x11949

/* vsync.c */
#define IDCLS_DISABLE_WARP_MODE                 0x1194a
#define IDCLS_DISABLE_WARP_MODE_DE              0x1194b
#define IDCLS_DISABLE_WARP_MODE_FR              0x1194c
#define IDCLS_DISABLE_WARP_MODE_IT              0x1194d
#define IDCLS_DISABLE_WARP_MODE_NL              0x1194e
#define IDCLS_DISABLE_WARP_MODE_PL              0x1194f
#define IDCLS_DISABLE_WARP_MODE_SV              0x11950

/* translate.c */
#define IDCLS_P_ISO_LANGUAGE_CODE               0x11951
#define IDCLS_P_ISO_LANGUAGE_CODE_DE            0x11952
#define IDCLS_P_ISO_LANGUAGE_CODE_FR            0x11953
#define IDCLS_P_ISO_LANGUAGE_CODE_IT            0x11954
#define IDCLS_P_ISO_LANGUAGE_CODE_NL            0x11955
#define IDCLS_P_ISO_LANGUAGE_CODE_PL            0x11956
#define IDCLS_P_ISO_LANGUAGE_CODE_SV            0x11957

/* translate.c */
#define IDCLS_SPECIFY_ISO_LANG_CODE             0x11958
#define IDCLS_SPECIFY_ISO_LANG_CODE_DE          0x11959
#define IDCLS_SPECIFY_ISO_LANG_CODE_FR          0x1195a
#define IDCLS_SPECIFY_ISO_LANG_CODE_IT          0x1195b
#define IDCLS_SPECIFY_ISO_LANG_CODE_NL          0x1195c
#define IDCLS_SPECIFY_ISO_LANG_CODE_PL          0x1195d
#define IDCLS_SPECIFY_ISO_LANG_CODE_SV          0x1195e

/* c64/plus60k.c */
#define IDCLS_ENABLE_PLUS60K_EXPANSION          0x1195f
#define IDCLS_ENABLE_PLUS60K_EXPANSION_DE       0x11960
#define IDCLS_ENABLE_PLUS60K_EXPANSION_FR       0x11961
#define IDCLS_ENABLE_PLUS60K_EXPANSION_IT       0x11962
#define IDCLS_ENABLE_PLUS60K_EXPANSION_NL       0x11963
#define IDCLS_ENABLE_PLUS60K_EXPANSION_PL       0x11964
#define IDCLS_ENABLE_PLUS60K_EXPANSION_SV       0x11965

/* c64/plus60k.c */
#define IDCLS_DISABLE_PLUS60K_EXPANSION         0x11966
#define IDCLS_DISABLE_PLUS60K_EXPANSION_DE      0x11967
#define IDCLS_DISABLE_PLUS60K_EXPANSION_FR      0x11968
#define IDCLS_DISABLE_PLUS60K_EXPANSION_IT      0x11969
#define IDCLS_DISABLE_PLUS60K_EXPANSION_NL      0x1196a
#define IDCLS_DISABLE_PLUS60K_EXPANSION_PL      0x1196b
#define IDCLS_DISABLE_PLUS60K_EXPANSION_SV      0x1196c

/* c64/plus60k.c */
#define IDCLS_SPECIFY_PLUS60K_NAME              0x1196d
#define IDCLS_SPECIFY_PLUS60K_NAME_DE           0x1196e
#define IDCLS_SPECIFY_PLUS60K_NAME_FR           0x1196f
#define IDCLS_SPECIFY_PLUS60K_NAME_IT           0x11970
#define IDCLS_SPECIFY_PLUS60K_NAME_NL           0x11971
#define IDCLS_SPECIFY_PLUS60K_NAME_PL           0x11972
#define IDCLS_SPECIFY_PLUS60K_NAME_SV           0x11973

/* sid/sid-cmdline-options.c */
#ifdef HAVE_RESID
/* sid/sid-cmdline-options.c */
#define IDCLS_RESID_GAIN_PERCENTAGE             0x11974
#define IDCLS_RESID_GAIN_PERCENTAGE_DE          0x11975
#define IDCLS_RESID_GAIN_PERCENTAGE_FR          0x11976
#define IDCLS_RESID_GAIN_PERCENTAGE_IT          0x11977
#define IDCLS_RESID_GAIN_PERCENTAGE_NL          0x11978
#define IDCLS_RESID_GAIN_PERCENTAGE_PL          0x11979
#define IDCLS_RESID_GAIN_PERCENTAGE_SV          0x1197a
#endif

/* c64/cart/c64cart.c */
#define IDCLS_ATTACH_RAW_STB_CART               0x1197b
#define IDCLS_ATTACH_RAW_STB_CART_DE            0x1197c
#define IDCLS_ATTACH_RAW_STB_CART_FR            0x1197d
#define IDCLS_ATTACH_RAW_STB_CART_IT            0x1197e
#define IDCLS_ATTACH_RAW_STB_CART_NL            0x1197f
#define IDCLS_ATTACH_RAW_STB_CART_PL            0x11980
#define IDCLS_ATTACH_RAW_STB_CART_SV            0x11981

/* c64/plus60k.c */
#define IDCLS_PLUS60K_BASE                      0x11982
#define IDCLS_PLUS60K_BASE_DE                   0x11983
#define IDCLS_PLUS60K_BASE_FR                   0x11984
#define IDCLS_PLUS60K_BASE_IT                   0x11985
#define IDCLS_PLUS60K_BASE_NL                   0x11986
#define IDCLS_PLUS60K_BASE_PL                   0x11987
#define IDCLS_PLUS60K_BASE_SV                   0x11988

/* c64/c64_256k.c */
#define IDCLS_ENABLE_C64_256K_EXPANSION         0x11989
#define IDCLS_ENABLE_C64_256K_EXPANSION_DE      0x1198a
#define IDCLS_ENABLE_C64_256K_EXPANSION_FR      0x1198b
#define IDCLS_ENABLE_C64_256K_EXPANSION_IT      0x1198c
#define IDCLS_ENABLE_C64_256K_EXPANSION_NL      0x1198d
#define IDCLS_ENABLE_C64_256K_EXPANSION_PL      0x1198e
#define IDCLS_ENABLE_C64_256K_EXPANSION_SV      0x1198f

/* c64/c64_256k.c */
#define IDCLS_DISABLE_C64_256K_EXPANSION        0x11990
#define IDCLS_DISABLE_C64_256K_EXPANSION_DE     0x11991
#define IDCLS_DISABLE_C64_256K_EXPANSION_FR     0x11992
#define IDCLS_DISABLE_C64_256K_EXPANSION_IT     0x11993
#define IDCLS_DISABLE_C64_256K_EXPANSION_NL     0x11994
#define IDCLS_DISABLE_C64_256K_EXPANSION_PL     0x11995
#define IDCLS_DISABLE_C64_256K_EXPANSION_SV     0x11996

/* c64/c64_256k.c */
#define IDCLS_SPECIFY_C64_256K_NAME             0x11997
#define IDCLS_SPECIFY_C64_256K_NAME_DE          0x11998
#define IDCLS_SPECIFY_C64_256K_NAME_FR          0x11999
#define IDCLS_SPECIFY_C64_256K_NAME_IT          0x1199a
#define IDCLS_SPECIFY_C64_256K_NAME_NL          0x1199b
#define IDCLS_SPECIFY_C64_256K_NAME_PL          0x1199c
#define IDCLS_SPECIFY_C64_256K_NAME_SV          0x1199d

/* c64/c64_256k.c */
#define IDCLS_C64_256K_BASE                     0x1199e
#define IDCLS_C64_256K_BASE_DE                  0x1199f
#define IDCLS_C64_256K_BASE_FR                  0x119a0
#define IDCLS_C64_256K_BASE_IT                  0x119a1
#define IDCLS_C64_256K_BASE_NL                  0x119a2
#define IDCLS_C64_256K_BASE_PL                  0x119a3
#define IDCLS_C64_256K_BASE_SV                  0x119a4

#ifdef HAVE_NETWORK
/* network.c */
#define IDGS_TESTING_BEST_FRAME_DELAY           0x119a5
#define IDGS_TESTING_BEST_FRAME_DELAY_DE        0x119a6
#define IDGS_TESTING_BEST_FRAME_DELAY_FR        0x119a7
#define IDGS_TESTING_BEST_FRAME_DELAY_IT        0x119a8
#define IDGS_TESTING_BEST_FRAME_DELAY_NL        0x119a9
#define IDGS_TESTING_BEST_FRAME_DELAY_PL        0x119aa
#define IDGS_TESTING_BEST_FRAME_DELAY_SV        0x119ab

/* network.c */
#define IDGS_USING_D_FRAMES_DELAY               0x119ac
#define IDGS_USING_D_FRAMES_DELAY_DE            0x119ad
#define IDGS_USING_D_FRAMES_DELAY_FR            0x119ae
#define IDGS_USING_D_FRAMES_DELAY_IT            0x119af
#define IDGS_USING_D_FRAMES_DELAY_NL            0x119b0
#define IDGS_USING_D_FRAMES_DELAY_PL            0x119b1
#define IDGS_USING_D_FRAMES_DELAY_SV            0x119b2

/* network.c */
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER      0x119b3
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_DE   0x119b4
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_FR   0x119b5
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_IT   0x119b6
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_NL   0x119b7
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_PL   0x119b8
#define IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER_SV   0x119b9

/* network.c */
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT         0x119ba
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_DE      0x119bb
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_FR      0x119bc
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_IT      0x119bd
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_NL      0x119be
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_PL      0x119bf
#define IDGS_SENDING_SNAPSHOT_TO_CLIENT_SV      0x119c0

/* network.c */
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT     0x119c1
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_DE  0x119c2
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_FR  0x119c3
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_IT  0x119c4
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_NL  0x119c5
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_PL  0x119c6
#define IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT_SV  0x119c7

/* network.c */
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S      0x119c8
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_DE   0x119c9
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_FR   0x119ca
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_IT   0x119cb
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_NL   0x119cc
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_PL   0x119cd
#define IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S_SV   0x119ce

/* network.c */
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S        0x119cf
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_DE     0x119d0
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_FR     0x119d1
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_IT     0x119d2
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_NL     0x119d3
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_PL     0x119d4
#define IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S_SV     0x119d5

/* network.c */
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT       0x119d6
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_DE    0x119d7
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_FR    0x119d8
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_IT    0x119d9
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_NL    0x119da
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_PL    0x119db
#define IDGS_SERVER_IS_WAITING_FOR_CLIENT_SV    0x119dc

/* network.c */
#define IDGS_CANNOT_RESOLVE_S                   0x119dd
#define IDGS_CANNOT_RESOLVE_S_DE                0x119de
#define IDGS_CANNOT_RESOLVE_S_FR                0x119df
#define IDGS_CANNOT_RESOLVE_S_IT                0x119e0
#define IDGS_CANNOT_RESOLVE_S_NL                0x119e1
#define IDGS_CANNOT_RESOLVE_S_PL                0x119e2
#define IDGS_CANNOT_RESOLVE_S_SV                0x119e3

/* network.c */
#define IDGS_CANNOT_CONNECT_TO_S                0x119e4
#define IDGS_CANNOT_CONNECT_TO_S_DE             0x119e5
#define IDGS_CANNOT_CONNECT_TO_S_FR             0x119e6
#define IDGS_CANNOT_CONNECT_TO_S_IT             0x119e7
#define IDGS_CANNOT_CONNECT_TO_S_NL             0x119e8
#define IDGS_CANNOT_CONNECT_TO_S_PL             0x119e9
#define IDGS_CANNOT_CONNECT_TO_S_SV             0x119ea

/* network.c */
#define IDGS_RECEIVING_SNAPSHOT_SERVER          0x119eb
#define IDGS_RECEIVING_SNAPSHOT_SERVER_DE       0x119ec
#define IDGS_RECEIVING_SNAPSHOT_SERVER_FR       0x119ed
#define IDGS_RECEIVING_SNAPSHOT_SERVER_IT       0x119ee
#define IDGS_RECEIVING_SNAPSHOT_SERVER_NL       0x119ef
#define IDGS_RECEIVING_SNAPSHOT_SERVER_PL       0x119f0
#define IDGS_RECEIVING_SNAPSHOT_SERVER_SV       0x119f1

/* network.c */
#define IDGS_NETWORK_OUT_OF_SYNC                0x11a00
#define IDGS_NETWORK_OUT_OF_SYNC_DE             0x11a01
#define IDGS_NETWORK_OUT_OF_SYNC_FR             0x11a02
#define IDGS_NETWORK_OUT_OF_SYNC_IT             0x11a03
#define IDGS_NETWORK_OUT_OF_SYNC_NL             0x11a04
#define IDGS_NETWORK_OUT_OF_SYNC_PL             0x11a05
#define IDGS_NETWORK_OUT_OF_SYNC_SV             0x11a06
#endif

/* gfxoutputdrv/ffmpegdrv.c */
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM         0x119f2
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_DE      0x119f3
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_FR      0x119f4
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_IT      0x119f5
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_NL      0x119f6
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_PL      0x119f7
#define IDGS_FFMPEG_CANNOT_OPEN_VSTREAM_SV      0x119f8

/* gfxoutputdrv/ffmpegdrv.c */
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM         0x119f9
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_DE      0x119fa
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_FR      0x119fb
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_IT      0x119fc
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_NL      0x119fd
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_PL      0x119fe
#define IDGS_FFMPEG_CANNOT_OPEN_ASTREAM_SV      0x119ff

/* gfxoutputdrv/ffmpegdrv.c */
#define IDGS_FFMPEG_CANNOT_OPEN_S               0x11a07
#define IDGS_FFMPEG_CANNOT_OPEN_S_DE            0x11a08
#define IDGS_FFMPEG_CANNOT_OPEN_S_FR            0x11a09
#define IDGS_FFMPEG_CANNOT_OPEN_S_IT            0x11a0a
#define IDGS_FFMPEG_CANNOT_OPEN_S_NL            0x11a0b
#define IDGS_FFMPEG_CANNOT_OPEN_S_PL            0x11a0c
#define IDGS_FFMPEG_CANNOT_OPEN_S_SV            0x11a0d

#ifdef HAVE_NETWORK
/* network.c */
#define IDGS_REMOTE_HOST_DISCONNECTED           0x11a0e
#define IDGS_REMOTE_HOST_DISCONNECTED_DE        0x11a0f
#define IDGS_REMOTE_HOST_DISCONNECTED_FR        0x11a10
#define IDGS_REMOTE_HOST_DISCONNECTED_IT        0x11a11
#define IDGS_REMOTE_HOST_DISCONNECTED_NL        0x11a12
#define IDGS_REMOTE_HOST_DISCONNECTED_PL        0x11a13
#define IDGS_REMOTE_HOST_DISCONNECTED_SV        0x11a14

/* network.c */
#define IDGS_REMOTE_HOST_SUSPENDING             0x11a15
#define IDGS_REMOTE_HOST_SUSPENDING_DE          0x11a16
#define IDGS_REMOTE_HOST_SUSPENDING_FR          0x11a17
#define IDGS_REMOTE_HOST_SUSPENDING_IT          0x11a18
#define IDGS_REMOTE_HOST_SUSPENDING_NL          0x11a19
#define IDGS_REMOTE_HOST_SUSPENDING_PL          0x11a1a
#define IDGS_REMOTE_HOST_SUSPENDING_SV          0x11a1b

#ifdef HAVE_IPV6
/* network.c */
#define IDGS_CANNOT_SWITCH_IPV4_IPV6            0x11a1c
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_DE         0x11a1d
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_FR         0x11a1e
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_IT         0x11a1f
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_NL         0x11a20
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_PL         0x11a21
#define IDGS_CANNOT_SWITCH_IPV4_IPV6_SV         0x11a22
#endif
#endif

#endif
