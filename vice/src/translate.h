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

#endif
#endif
