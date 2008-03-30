/*
 * intl.h - Localization routines for Amiga.
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

#ifndef _INTL_H
#define _INTL_H

extern char *intl_speed_at_text;

extern char *intl_translate_text(int en_resource);
extern int intl_translate_res(int en_resource);

extern void intl_init(void);
extern void intl_shutdown(void);
extern char *intl_arch_language_init(void);
extern void intl_update_ui(void);
extern char *intl_convert_cp(char *text, int cp);

extern void intl_convert_mui_table(int x[], char *y[]);

/* ----------------------- AmigaOS Menu Strings ----------------------- */

#define IDMS_FILE                         1
#define IDMS_FILE_DE                      2
#define IDMS_FILE_FR                      3
#define IDMS_FILE_IT                      4
#define IDMS_FILE_NL                      5
#define IDMS_FILE_PL                      6
#define IDMS_FILE_SV                      7

#define IDMS_AUTOSTART_IMAGE              8
#define IDMS_AUTOSTART_IMAGE_DE           9
#define IDMS_AUTOSTART_IMAGE_FR           0xa
#define IDMS_AUTOSTART_IMAGE_IT           0xb
#define IDMS_AUTOSTART_IMAGE_NL           0xc
#define IDMS_AUTOSTART_IMAGE_PL           0xd
#define IDMS_AUTOSTART_IMAGE_SV           0xe

#define IDMS_ATTACH_DISK_IMAGE            0xf
#define IDMS_ATTACH_DISK_IMAGE_DE         0x10
#define IDMS_ATTACH_DISK_IMAGE_FR         0x11
#define IDMS_ATTACH_DISK_IMAGE_IT         0x12
#define IDMS_ATTACH_DISK_IMAGE_NL         0x13
#define IDMS_ATTACH_DISK_IMAGE_PL         0x14
#define IDMS_ATTACH_DISK_IMAGE_SV         0x15

#define IDMS_DRIVE_8                      0x16
#define IDMS_DRIVE_8_DE                   0x17
#define IDMS_DRIVE_8_FR                   0x18
#define IDMS_DRIVE_8_IT                   0x19
#define IDMS_DRIVE_8_NL                   0x1a
#define IDMS_DRIVE_8_PL                   0x1b
#define IDMS_DRIVE_8_SV                   0x1c

#define IDMS_DRIVE_9                      0x1d
#define IDMS_DRIVE_9_DE                   0x1e
#define IDMS_DRIVE_9_FR                   0x1f
#define IDMS_DRIVE_9_IT                   0x20
#define IDMS_DRIVE_9_NL                   0x21
#define IDMS_DRIVE_9_PL                   0x22
#define IDMS_DRIVE_9_SV                   0x23

#define IDMS_DRIVE_10                     0x24
#define IDMS_DRIVE_10_DE                  0x25
#define IDMS_DRIVE_10_FR                  0x26
#define IDMS_DRIVE_10_IT                  0x27
#define IDMS_DRIVE_10_NL                  0x28
#define IDMS_DRIVE_10_PL                  0x29
#define IDMS_DRIVE_10_SV                  0x2a

#define IDMS_DRIVE_11                     0x2b
#define IDMS_DRIVE_11_DE                  0x2c
#define IDMS_DRIVE_11_FR                  0x2d
#define IDMS_DRIVE_11_IT                  0x2e
#define IDMS_DRIVE_11_NL                  0x2f
#define IDMS_DRIVE_11_PL                  0x30
#define IDMS_DRIVE_11_SV                  0x31

#define IDMS_DETACH_DISK_IMAGE            0x32
#define IDMS_DETACH_DISK_IMAGE_DE         0x33
#define IDMS_DETACH_DISK_IMAGE_FR         0x34
#define IDMS_DETACH_DISK_IMAGE_IT         0x35
#define IDMS_DETACH_DISK_IMAGE_NL         0x36
#define IDMS_DETACH_DISK_IMAGE_PL         0x37
#define IDMS_DETACH_DISK_IMAGE_SV         0x38

#define IDMS_ALL                          0x39
#define IDMS_ALL_DE                       0x3a
#define IDMS_ALL_FR                       0x3b
#define IDMS_ALL_IT                       0x3c
#define IDMS_ALL_NL                       0x3d
#define IDMS_ALL_PL                       0x3e
#define IDMS_ALL_SV                       0x3f

#define IDMS_FLIP_LIST                    0x40
#define IDMS_FLIP_LIST_DE                 0x41
#define IDMS_FLIP_LIST_FR                 0x42
#define IDMS_FLIP_LIST_IT                 0x43
#define IDMS_FLIP_LIST_NL                 0x44
#define IDMS_FLIP_LIST_PL                 0x45
#define IDMS_FLIP_LIST_SV                 0x46

#define IDMS_ADD_CURRENT_IMAGE            0x47
#define IDMS_ADD_CURRENT_IMAGE_DE         0x48
#define IDMS_ADD_CURRENT_IMAGE_FR         0x49
#define IDMS_ADD_CURRENT_IMAGE_IT         0x4a
#define IDMS_ADD_CURRENT_IMAGE_NL         0x4b
#define IDMS_ADD_CURRENT_IMAGE_PL         0x4c
#define IDMS_ADD_CURRENT_IMAGE_SV         0x4d

#define IDMS_REMOVE_CURRENT_IMAGE         0x4e
#define IDMS_REMOVE_CURRENT_IMAGE_DE      0x4f
#define IDMS_REMOVE_CURRENT_IMAGE_FR      0x50
#define IDMS_REMOVE_CURRENT_IMAGE_IT      0x51
#define IDMS_REMOVE_CURRENT_IMAGE_NL      0x52
#define IDMS_REMOVE_CURRENT_IMAGE_PL      0x53
#define IDMS_REMOVE_CURRENT_IMAGE_SV      0x54

#define IDMS_ATTACH_NEXT_IMAGE            0x55
#define IDMS_ATTACH_NEXT_IMAGE_DE         0x56
#define IDMS_ATTACH_NEXT_IMAGE_FR         0x57
#define IDMS_ATTACH_NEXT_IMAGE_IT         0x58
#define IDMS_ATTACH_NEXT_IMAGE_NL         0x59
#define IDMS_ATTACH_NEXT_IMAGE_PL         0x5a
#define IDMS_ATTACH_NEXT_IMAGE_SV         0x5b

#define IDMS_ATTACH_PREVIOUS_IMAGE        0x5c
#define IDMS_ATTACH_PREVIOUS_IMAGE_DE     0x5d
#define IDMS_ATTACH_PREVIOUS_IMAGE_FR     0x5e
#define IDMS_ATTACH_PREVIOUS_IMAGE_IT     0x5f
#define IDMS_ATTACH_PREVIOUS_IMAGE_NL     0x60
#define IDMS_ATTACH_PREVIOUS_IMAGE_PL     0x61
#define IDMS_ATTACH_PREVIOUS_IMAGE_SV     0x62

#define IDMS_LOAD_FLIP_LIST               0x63
#define IDMS_LOAD_FLIP_LIST_DE            0x64
#define IDMS_LOAD_FLIP_LIST_FR            0x65
#define IDMS_LOAD_FLIP_LIST_IT            0x66
#define IDMS_LOAD_FLIP_LIST_NL            0x67
#define IDMS_LOAD_FLIP_LIST_PL            0x68
#define IDMS_LOAD_FLIP_LIST_SV            0x69

#define IDMS_SAVE_FLIP_LIST               0x6a
#define IDMS_SAVE_FLIP_LIST_DE            0x6b
#define IDMS_SAVE_FLIP_LIST_FR            0x6c
#define IDMS_SAVE_FLIP_LIST_IT            0x6d
#define IDMS_SAVE_FLIP_LIST_NL            0x6e
#define IDMS_SAVE_FLIP_LIST_PL            0x6f
#define IDMS_SAVE_FLIP_LIST_SV            0x70

#define IDMS_ATTACH_TAPE_IMAGE            0x71
#define IDMS_ATTACH_TAPE_IMAGE_DE         0x72
#define IDMS_ATTACH_TAPE_IMAGE_FR         0x73
#define IDMS_ATTACH_TAPE_IMAGE_IT         0x74
#define IDMS_ATTACH_TAPE_IMAGE_NL         0x75
#define IDMS_ATTACH_TAPE_IMAGE_PL         0x76
#define IDMS_ATTACH_TAPE_IMAGE_SV         0x77

#define IDMS_DETACH_TAPE_IMAGE            0x78
#define IDMS_DETACH_TAPE_IMAGE_DE         0x79
#define IDMS_DETACH_TAPE_IMAGE_FR         0x7a
#define IDMS_DETACH_TAPE_IMAGE_IT         0x7b
#define IDMS_DETACH_TAPE_IMAGE_NL         0x7c
#define IDMS_DETACH_TAPE_IMAGE_PL         0x7d
#define IDMS_DETACH_TAPE_IMAGE_SV         0x7e

#define IDMS_DATASSETTE_CONTROL           0x7f
#define IDMS_DATASSETTE_CONTROL_DE        0x80
#define IDMS_DATASSETTE_CONTROL_FR        0x81
#define IDMS_DATASSETTE_CONTROL_IT        0x82
#define IDMS_DATASSETTE_CONTROL_NL        0x83
#define IDMS_DATASSETTE_CONTROL_PL        0x84
#define IDMS_DATASSETTE_CONTROL_SV        0x85

#define IDMS_STOP                         0x86
#define IDMS_STOP_DE                      0x87
#define IDMS_STOP_FR                      0x88
#define IDMS_STOP_IT                      0x89
#define IDMS_STOP_NL                      0x8a
#define IDMS_STOP_PL                      0x8b
#define IDMS_STOP_SV                      0x8c

#define IDMS_START                        0x8d
#define IDMS_START_DE                     0x8e
#define IDMS_START_FR                     0x8f
#define IDMS_START_IT                     0x90
#define IDMS_START_NL                     0x91
#define IDMS_START_PL                     0x92
#define IDMS_START_SV                     0x93

#define IDMS_FORWARD                      0x94
#define IDMS_FORWARD_DE                   0x95
#define IDMS_FORWARD_FR                   0x96
#define IDMS_FORWARD_IT                   0x97
#define IDMS_FORWARD_NL                   0x98
#define IDMS_FORWARD_PL                   0x99
#define IDMS_FORWARD_SV                   0x9a

#define IDMS_REWIND                       0x9b
#define IDMS_REWIND_DE                    0x9c
#define IDMS_REWIND_FR                    0x9d
#define IDMS_REWIND_IT                    0x9e
#define IDMS_REWIND_NL                    0x9f
#define IDMS_REWIND_PL                    0xa0
#define IDMS_REWIND_SV                    0xa1

#define IDMS_RECORD                       0xa2
#define IDMS_RECORD_DE                    0xa3
#define IDMS_RECORD_FR                    0xa4
#define IDMS_RECORD_IT                    0xa5
#define IDMS_RECORD_NL                    0xa6
#define IDMS_RECORD_PL                    0xa7
#define IDMS_RECORD_SV                    0xa8

#define IDMS_RESET                        0xa9
#define IDMS_RESET_DE                     0xaa
#define IDMS_RESET_FR                     0xab
#define IDMS_RESET_IT                     0xac
#define IDMS_RESET_NL                     0xad
#define IDMS_RESET_PL                     0xae
#define IDMS_RESET_SV                     0xaf

#define IDMS_RESET_COUNTER                0xb0
#define IDMS_RESET_COUNTER_DE             0xb1
#define IDMS_RESET_COUNTER_FR             0xb2
#define IDMS_RESET_COUNTER_IT             0xb3
#define IDMS_RESET_COUNTER_NL             0xb4
#define IDMS_RESET_COUNTER_PL             0xb5
#define IDMS_RESET_COUNTER_SV             0xb6

#define IDMS_ATTACH_CART_IMAGE            0xb7
#define IDMS_ATTACH_CART_IMAGE_DE         0xb8
#define IDMS_ATTACH_CART_IMAGE_FR         0xb9
#define IDMS_ATTACH_CART_IMAGE_IT         0xba
#define IDMS_ATTACH_CART_IMAGE_NL         0xbb
#define IDMS_ATTACH_CART_IMAGE_PL         0xbc
#define IDMS_ATTACH_CART_IMAGE_SV         0xbd

#define IDMS_4_8_16KB_AT_2000             0xbe
#define IDMS_4_8_16KB_AT_2000_DE          0xbf
#define IDMS_4_8_16KB_AT_2000_FR          0xc0
#define IDMS_4_8_16KB_AT_2000_IT          0xc1
#define IDMS_4_8_16KB_AT_2000_NL          0xc2
#define IDMS_4_8_16KB_AT_2000_PL          0xc3
#define IDMS_4_8_16KB_AT_2000_SV          0xc4

#define IDMS_4_8_16KB_AT_4000             0xc5
#define IDMS_4_8_16KB_AT_4000_DE          0xc6
#define IDMS_4_8_16KB_AT_4000_FR          0xc7
#define IDMS_4_8_16KB_AT_4000_IT          0xc8
#define IDMS_4_8_16KB_AT_4000_NL          0xc9
#define IDMS_4_8_16KB_AT_4000_PL          0xca
#define IDMS_4_8_16KB_AT_4000_SV          0xcb

#define IDMS_4_8_16KB_AT_6000             0xcc
#define IDMS_4_8_16KB_AT_6000_DE          0xcd
#define IDMS_4_8_16KB_AT_6000_FR          0xce
#define IDMS_4_8_16KB_AT_6000_IT          0xcf
#define IDMS_4_8_16KB_AT_6000_NL          0xd0
#define IDMS_4_8_16KB_AT_6000_PL          0xd1
#define IDMS_4_8_16KB_AT_6000_SV          0xd2

#define IDMS_4_8KB_AT_A000                0xd3
#define IDMS_4_8KB_AT_A000_DE             0xd4
#define IDMS_4_8KB_AT_A000_FR             0xd5
#define IDMS_4_8KB_AT_A000_IT             0xd6
#define IDMS_4_8KB_AT_A000_NL             0xd7
#define IDMS_4_8KB_AT_A000_PL             0xd8
#define IDMS_4_8KB_AT_A000_SV             0xd9

#define IDMS_4KB_AT_B000                  0xda
#define IDMS_4KB_AT_B000_DE               0xdb
#define IDMS_4KB_AT_B000_FR               0xdc
#define IDMS_4KB_AT_B000_IT               0xdd
#define IDMS_4KB_AT_B000_NL               0xde
#define IDMS_4KB_AT_B000_PL               0xdf
#define IDMS_4KB_AT_B000_SV               0xe0

#define IDMS_DETACH_CART_IMAGE            0xe1
#define IDMS_DETACH_CART_IMAGE_DE         0xe2
#define IDMS_DETACH_CART_IMAGE_FR         0xe3
#define IDMS_DETACH_CART_IMAGE_IT         0xe4
#define IDMS_DETACH_CART_IMAGE_NL         0xe5
#define IDMS_DETACH_CART_IMAGE_PL         0xe6
#define IDMS_DETACH_CART_IMAGE_SV         0xe7

#define IDMS_C1_LOW_IMAGE                 0xe8
#define IDMS_C1_LOW_IMAGE_DE              0xe9
#define IDMS_C1_LOW_IMAGE_FR              0xea
#define IDMS_C1_LOW_IMAGE_IT              0xeb
#define IDMS_C1_LOW_IMAGE_NL              0xec
#define IDMS_C1_LOW_IMAGE_PL              0xed
#define IDMS_C1_LOW_IMAGE_SV              0xee

#define IDMS_C1_HIGH_IMAGE                0xef
#define IDMS_C1_HIGH_IMAGE_DE             0xf0
#define IDMS_C1_HIGH_IMAGE_FR             0xf1
#define IDMS_C1_HIGH_IMAGE_IT             0xf2
#define IDMS_C1_HIGH_IMAGE_NL             0xf3
#define IDMS_C1_HIGH_IMAGE_PL             0xf4
#define IDMS_C1_HIGH_IMAGE_SV             0xf5

#define IDMS_C2_LOW_IMAGE                 0xf6
#define IDMS_C2_LOW_IMAGE_DE              0xf7
#define IDMS_C2_LOW_IMAGE_FR              0xf8
#define IDMS_C2_LOW_IMAGE_IT              0xf9
#define IDMS_C2_LOW_IMAGE_NL              0xfa
#define IDMS_C2_LOW_IMAGE_PL              0xfb
#define IDMS_C2_LOW_IMAGE_SV              0xfc

#define IDMS_C2_HIGH_IMAGE                0xfd
#define IDMS_C2_HIGH_IMAGE_DE             0xfe
#define IDMS_C2_HIGH_IMAGE_FR             0xff
#define IDMS_C2_HIGH_IMAGE_IT             0x100
#define IDMS_C2_HIGH_IMAGE_NL             0x101
#define IDMS_C2_HIGH_IMAGE_PL             0x102
#define IDMS_C2_HIGH_IMAGE_SV             0x103

#define IDMS_FUNCTION_LOW_3PLUS1          0x104
#define IDMS_FUNCTION_LOW_3PLUS1_DE       0x105
#define IDMS_FUNCTION_LOW_3PLUS1_FR       0x106
#define IDMS_FUNCTION_LOW_3PLUS1_IT       0x107
#define IDMS_FUNCTION_LOW_3PLUS1_NL       0x108
#define IDMS_FUNCTION_LOW_3PLUS1_PL       0x109
#define IDMS_FUNCTION_LOW_3PLUS1_SV       0x10a

#define IDMS_FUNCTION_HIGH_3PLUS1         0x10b
#define IDMS_FUNCTION_HIGH_3PLUS1_DE      0x10c
#define IDMS_FUNCTION_HIGH_3PLUS1_FR      0x10d
#define IDMS_FUNCTION_HIGH_3PLUS1_IT      0x10e
#define IDMS_FUNCTION_HIGH_3PLUS1_NL      0x10f
#define IDMS_FUNCTION_HIGH_3PLUS1_PL      0x110
#define IDMS_FUNCTION_HIGH_3PLUS1_SV      0x111

#define IDMS_CRT_IMAGE                    0x112
#define IDMS_CRT_IMAGE_DE                 0x113
#define IDMS_CRT_IMAGE_FR                 0x114
#define IDMS_CRT_IMAGE_IT                 0x115
#define IDMS_CRT_IMAGE_NL                 0x116
#define IDMS_CRT_IMAGE_PL                 0x117
#define IDMS_CRT_IMAGE_SV                 0x118

#define IDMS_GENERIC_8KB_IMAGE            0x119
#define IDMS_GENERIC_8KB_IMAGE_DE         0x11a
#define IDMS_GENERIC_8KB_IMAGE_FR         0x11b
#define IDMS_GENERIC_8KB_IMAGE_IT         0x11c
#define IDMS_GENERIC_8KB_IMAGE_NL         0x11d
#define IDMS_GENERIC_8KB_IMAGE_PL         0x11e
#define IDMS_GENERIC_8KB_IMAGE_SV         0x11f

#define IDMS_GENERIC_16KB_IMAGE           0x120
#define IDMS_GENERIC_16KB_IMAGE_DE        0x121
#define IDMS_GENERIC_16KB_IMAGE_FR        0x122
#define IDMS_GENERIC_16KB_IMAGE_IT        0x123
#define IDMS_GENERIC_16KB_IMAGE_NL        0x124
#define IDMS_GENERIC_16KB_IMAGE_PL        0x125
#define IDMS_GENERIC_16KB_IMAGE_SV        0x126

#define IDMS_ACTION_REPLAY_IMAGE          0x127
#define IDMS_ACTION_REPLAY_IMAGE_DE       0x128
#define IDMS_ACTION_REPLAY_IMAGE_FR       0x129
#define IDMS_ACTION_REPLAY_IMAGE_IT       0x12a
#define IDMS_ACTION_REPLAY_IMAGE_NL       0x12b
#define IDMS_ACTION_REPLAY_IMAGE_PL       0x12c
#define IDMS_ACTION_REPLAY_IMAGE_SV       0x12d

#define IDMS_ATOMIC_POWER_IMAGE           0x12e
#define IDMS_ATOMIC_POWER_IMAGE_DE        0x12f
#define IDMS_ATOMIC_POWER_IMAGE_FR        0x130
#define IDMS_ATOMIC_POWER_IMAGE_IT        0x131
#define IDMS_ATOMIC_POWER_IMAGE_NL        0x132
#define IDMS_ATOMIC_POWER_IMAGE_PL        0x133
#define IDMS_ATOMIC_POWER_IMAGE_SV        0x134

#define IDMS_EPYX_FASTLOAD_IMAGE          0x135
#define IDMS_EPYX_FASTLOAD_IMAGE_DE       0x136
#define IDMS_EPYX_FASTLOAD_IMAGE_FR       0x137
#define IDMS_EPYX_FASTLOAD_IMAGE_IT       0x138
#define IDMS_EPYX_FASTLOAD_IMAGE_NL       0x139
#define IDMS_EPYX_FASTLOAD_IMAGE_PL       0x13a
#define IDMS_EPYX_FASTLOAD_IMAGE_SV       0x13b

#define IDMS_IEEE488_INTERFACE_IMAGE      0x13c
#define IDMS_IEEE488_INTERFACE_IMAGE_DE   0x13d
#define IDMS_IEEE488_INTERFACE_IMAGE_FR   0x13e
#define IDMS_IEEE488_INTERFACE_IMAGE_IT   0x13f
#define IDMS_IEEE488_INTERFACE_IMAGE_NL   0x140
#define IDMS_IEEE488_INTERFACE_IMAGE_PL   0x141
#define IDMS_IEEE488_INTERFACE_IMAGE_SV   0x142

#define IDMS_RETRO_REPLAY_IMAGE           0x143
#define IDMS_RETRO_REPLAY_IMAGE_DE        0x144
#define IDMS_RETRO_REPLAY_IMAGE_FR        0x145
#define IDMS_RETRO_REPLAY_IMAGE_IT        0x146
#define IDMS_RETRO_REPLAY_IMAGE_NL        0x147
#define IDMS_RETRO_REPLAY_IMAGE_PL        0x148
#define IDMS_RETRO_REPLAY_IMAGE_SV        0x149

#define IDMS_IDE64_INTERFACE_IMAGE        0x14a
#define IDMS_IDE64_INTERFACE_IMAGE_DE     0x14b
#define IDMS_IDE64_INTERFACE_IMAGE_FR     0x14c
#define IDMS_IDE64_INTERFACE_IMAGE_IT     0x14d
#define IDMS_IDE64_INTERFACE_IMAGE_NL     0x14e
#define IDMS_IDE64_INTERFACE_IMAGE_PL     0x14f
#define IDMS_IDE64_INTERFACE_IMAGE_SV     0x150

#define IDMS_SUPER_SNAPSHOT_4_IMAGE       0x151
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_DE    0x152
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_FR    0x153
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_IT    0x154
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_NL    0x155
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_PL    0x156
#define IDMS_SUPER_SNAPSHOT_4_IMAGE_SV    0x157

#define IDMS_SUPER_SNAPSHOT_5_IMAGE       0x158
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_DE    0x159
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_FR    0x15a
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_IT    0x15b
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_NL    0x15c
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_PL    0x15d
#define IDMS_SUPER_SNAPSHOT_5_IMAGE_SV    0x15e

#define IDMS_STRUCTURED_BASIC_IMAGE       0x15f
#define IDMS_STRUCTURED_BASIC_IMAGE_DE    0x160
#define IDMS_STRUCTURED_BASIC_IMAGE_FR    0x161
#define IDMS_STRUCTURED_BASIC_IMAGE_IT    0x162
#define IDMS_STRUCTURED_BASIC_IMAGE_NL    0x163
#define IDMS_STRUCTURED_BASIC_IMAGE_PL    0x164
#define IDMS_STRUCTURED_BASIC_IMAGE_SV    0x165

#define IDMS_EXPERT_CART                  0x166
#define IDMS_EXPERT_CART_DE               0x167
#define IDMS_EXPERT_CART_FR               0x168
#define IDMS_EXPERT_CART_IT               0x169
#define IDMS_EXPERT_CART_NL               0x16a
#define IDMS_EXPERT_CART_PL               0x16b
#define IDMS_EXPERT_CART_SV               0x16c

#define IDMS_ENABLE                       0x16d
#define IDMS_ENABLE_DE                    0x16e
#define IDMS_ENABLE_FR                    0x16f
#define IDMS_ENABLE_IT                    0x170
#define IDMS_ENABLE_NL                    0x171
#define IDMS_ENABLE_PL                    0x172
#define IDMS_ENABLE_SV                    0x173

#define IDMS_OFF                          0x174
#define IDMS_OFF_DE                       0x175
#define IDMS_OFF_FR                       0x176
#define IDMS_OFF_IT                       0x177
#define IDMS_OFF_NL                       0x178
#define IDMS_OFF_PL                       0x179
#define IDMS_OFF_SV                       0x17a

#define IDMS_PRG                          0x17b
#define IDMS_PRG_DE                       0x17c
#define IDMS_PRG_FR                       0x17d
#define IDMS_PRG_IT                       0x17e
#define IDMS_PRG_NL                       0x17f
#define IDMS_PRG_PL                       0x180
#define IDMS_PRG_SV                       0x181

#define IDMS_ON                           0x182
#define IDMS_ON_DE                        0x183
#define IDMS_ON_FR                        0x184
#define IDMS_ON_IT                        0x185
#define IDMS_ON_NL                        0x186
#define IDMS_ON_PL                        0x187
#define IDMS_ON_SV                        0x188

#define IDMS_SET_CART_AS_DEFAULT          0x189
#define IDMS_SET_CART_AS_DEFAULT_DE       0x18a
#define IDMS_SET_CART_AS_DEFAULT_FR       0x18b
#define IDMS_SET_CART_AS_DEFAULT_IT       0x18c
#define IDMS_SET_CART_AS_DEFAULT_NL       0x18d
#define IDMS_SET_CART_AS_DEFAULT_PL       0x18e
#define IDMS_SET_CART_AS_DEFAULT_SV       0x18f

#define IDMS_RESET_ON_CART_CHANGE         0x190
#define IDMS_RESET_ON_CART_CHANGE_DE      0x191
#define IDMS_RESET_ON_CART_CHANGE_FR      0x192
#define IDMS_RESET_ON_CART_CHANGE_IT      0x193
#define IDMS_RESET_ON_CART_CHANGE_NL      0x194
#define IDMS_RESET_ON_CART_CHANGE_PL      0x195
#define IDMS_RESET_ON_CART_CHANGE_SV      0x196

#define IDMS_CART_FREEZE                  0x197
#define IDMS_CART_FREEZE_DE               0x198
#define IDMS_CART_FREEZE_FR               0x199
#define IDMS_CART_FREEZE_IT               0x19a
#define IDMS_CART_FREEZE_NL               0x19b
#define IDMS_CART_FREEZE_PL               0x19c
#define IDMS_CART_FREEZE_SV               0x19d

#define IDMS_PAUSE                        0x19e
#define IDMS_PAUSE_DE                     0x19f
#define IDMS_PAUSE_FR                     0x1a0
#define IDMS_PAUSE_IT                     0x1a1
#define IDMS_PAUSE_NL                     0x1a2
#define IDMS_PAUSE_PL                     0x1a3
#define IDMS_PAUSE_SV                     0x1a4

#define IDMS_MONITOR                      0x1a5
#define IDMS_MONITOR_DE                   0x1a6
#define IDMS_MONITOR_FR                   0x1a7
#define IDMS_MONITOR_IT                   0x1a8
#define IDMS_MONITOR_NL                   0x1a9
#define IDMS_MONITOR_PL                   0x1aa
#define IDMS_MONITOR_SV                   0x1ab

#define IDMS_HARD                         0x1ac
#define IDMS_HARD_DE                      0x1ad
#define IDMS_HARD_FR                      0x1ae
#define IDMS_HARD_IT                      0x1af
#define IDMS_HARD_NL                      0x1b0
#define IDMS_HARD_PL                      0x1b1
#define IDMS_HARD_SV                      0x1b2

#define IDMS_SOFT                         0x1b3
#define IDMS_SOFT_DE                      0x1b4
#define IDMS_SOFT_FR                      0x1b5
#define IDMS_SOFT_IT                      0x1b6
#define IDMS_SOFT_NL                      0x1b7
#define IDMS_SOFT_PL                      0x1b8
#define IDMS_SOFT_SV                      0x1b9

#define IDMS_EXIT                         0x1ba
#define IDMS_EXIT_DE                      0x1bb
#define IDMS_EXIT_FR                      0x1bc
#define IDMS_EXIT_IT                      0x1bd
#define IDMS_EXIT_NL                      0x1be
#define IDMS_EXIT_PL                      0x1bf
#define IDMS_EXIT_SV                      0x1c0

#define IDMS_SNAPSHOT                     0x1c1
#define IDMS_SNAPSHOT_DE                  0x1c2
#define IDMS_SNAPSHOT_FR                  0x1c3
#define IDMS_SNAPSHOT_IT                  0x1c4
#define IDMS_SNAPSHOT_NL                  0x1c5
#define IDMS_SNAPSHOT_PL                  0x1c6
#define IDMS_SNAPSHOT_SV                  0x1c7

#define IDMS_LOAD_SNAPSHOT_IMAGE          0x1c8
#define IDMS_LOAD_SNAPSHOT_IMAGE_DE       0x1c9
#define IDMS_LOAD_SNAPSHOT_IMAGE_FR       0x1ca
#define IDMS_LOAD_SNAPSHOT_IMAGE_IT       0x1cb
#define IDMS_LOAD_SNAPSHOT_IMAGE_NL       0x1cc
#define IDMS_LOAD_SNAPSHOT_IMAGE_PL       0x1cd
#define IDMS_LOAD_SNAPSHOT_IMAGE_SV       0x1ce

#define IDMS_SAVE_SNAPSHOT_IMAGE          0x1cf
#define IDMS_SAVE_SNAPSHOT_IMAGE_DE       0x1d0
#define IDMS_SAVE_SNAPSHOT_IMAGE_FR       0x1d1
#define IDMS_SAVE_SNAPSHOT_IMAGE_IT       0x1d2
#define IDMS_SAVE_SNAPSHOT_IMAGE_NL       0x1d3
#define IDMS_SAVE_SNAPSHOT_IMAGE_PL       0x1d4
#define IDMS_SAVE_SNAPSHOT_IMAGE_SV       0x1d5

#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE     0x1d6
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_DE  0x1d7
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_FR  0x1d8
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_IT  0x1d9
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_NL  0x1da
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_PL  0x1db
#define IDMS_LOAD_QUICKSNAPSHOT_IMAGE_SV  0x1dc

#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE     0x1dd
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_DE  0x1de
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_FR  0x1df
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_IT  0x1e0
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_NL  0x1e1
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_PL  0x1e2
#define IDMS_SAVE_QUICKSNAPSHOT_IMAGE_SV  0x1e3

#define IDMS_START_STOP_RECORDING         0x1e4
#define IDMS_START_STOP_RECORDING_DE      0x1e5
#define IDMS_START_STOP_RECORDING_FR      0x1e6
#define IDMS_START_STOP_RECORDING_IT      0x1e7
#define IDMS_START_STOP_RECORDING_NL      0x1e8
#define IDMS_START_STOP_RECORDING_PL      0x1e9
#define IDMS_START_STOP_RECORDING_SV      0x1ea

#define IDMS_START_STOP_PLAYBACK          0x1eb
#define IDMS_START_STOP_PLAYBACK_DE       0x1ec
#define IDMS_START_STOP_PLAYBACK_FR       0x1ed
#define IDMS_START_STOP_PLAYBACK_IT       0x1ee
#define IDMS_START_STOP_PLAYBACK_NL       0x1ef
#define IDMS_START_STOP_PLAYBACK_PL       0x1f0
#define IDMS_START_STOP_PLAYBACK_SV       0x1f1

#define IDMS_SET_MILESTONE                0x1f2
#define IDMS_SET_MILESTONE_DE             0x1f3
#define IDMS_SET_MILESTONE_FR             0x1f4
#define IDMS_SET_MILESTONE_IT             0x1f5
#define IDMS_SET_MILESTONE_NL             0x1f6
#define IDMS_SET_MILESTONE_PL             0x1f7
#define IDMS_SET_MILESTONE_SV             0x1f8

#define IDMS_RETURN_TO_MILESTONE          0x1f9
#define IDMS_RETURN_TO_MILESTONE_DE       0x1fa
#define IDMS_RETURN_TO_MILESTONE_FR       0x1fb
#define IDMS_RETURN_TO_MILESTONE_IT       0x1fc
#define IDMS_RETURN_TO_MILESTONE_NL       0x1fd
#define IDMS_RETURN_TO_MILESTONE_PL       0x1fe
#define IDMS_RETURN_TO_MILESTONE_SV       0x1ff

#define IDMS_RECORDING_START_MODE         0x200
#define IDMS_RECORDING_START_MODE_DE      0x201
#define IDMS_RECORDING_START_MODE_FR      0x202
#define IDMS_RECORDING_START_MODE_IT      0x203
#define IDMS_RECORDING_START_MODE_NL      0x204
#define IDMS_RECORDING_START_MODE_PL      0x205
#define IDMS_RECORDING_START_MODE_SV      0x206

#define IDMS_SAVE_NEW_SNAPSHOT            0x207
#define IDMS_SAVE_NEW_SNAPSHOT_DE         0x208
#define IDMS_SAVE_NEW_SNAPSHOT_FR         0x209
#define IDMS_SAVE_NEW_SNAPSHOT_IT         0x20a
#define IDMS_SAVE_NEW_SNAPSHOT_NL         0x20b
#define IDMS_SAVE_NEW_SNAPSHOT_PL         0x20c
#define IDMS_SAVE_NEW_SNAPSHOT_SV         0x20d

#define IDMS_LOAD_EXISTING_SNAPSHOT       0x20e
#define IDMS_LOAD_EXISTING_SNAPSHOT_DE    0x20f
#define IDMS_LOAD_EXISTING_SNAPSHOT_FR    0x210
#define IDMS_LOAD_EXISTING_SNAPSHOT_IT    0x211
#define IDMS_LOAD_EXISTING_SNAPSHOT_NL    0x212
#define IDMS_LOAD_EXISTING_SNAPSHOT_PL    0x213
#define IDMS_LOAD_EXISTING_SNAPSHOT_SV    0x214

#define IDMS_START_WITH_RESET             0x215
#define IDMS_START_WITH_RESET_DE          0x216
#define IDMS_START_WITH_RESET_FR          0x217
#define IDMS_START_WITH_RESET_IT          0x218
#define IDMS_START_WITH_RESET_NL          0x219
#define IDMS_START_WITH_RESET_PL          0x21a
#define IDMS_START_WITH_RESET_SV          0x21b

#define IDMS_OVERWRITE_PLAYBACK           0x21c
#define IDMS_OVERWRITE_PLAYBACK_DE        0x21d
#define IDMS_OVERWRITE_PLAYBACK_FR        0x21e
#define IDMS_OVERWRITE_PLAYBACK_IT        0x21f
#define IDMS_OVERWRITE_PLAYBACK_NL        0x220
#define IDMS_OVERWRITE_PLAYBACK_PL        0x221
#define IDMS_OVERWRITE_PLAYBACK_SV        0x222

#define IDMS_SELECT_HISTORY_DIR           0x223
#define IDMS_SELECT_HISTORY_DIR_DE        0x224
#define IDMS_SELECT_HISTORY_DIR_FR        0x225
#define IDMS_SELECT_HISTORY_DIR_IT        0x226
#define IDMS_SELECT_HISTORY_DIR_NL        0x227
#define IDMS_SELECT_HISTORY_DIR_PL        0x228
#define IDMS_SELECT_HISTORY_DIR_SV        0x229

#define IDMS_SAVE_STOP_MEDIA_FILE         0x22a
#define IDMS_SAVE_STOP_MEDIA_FILE_DE      0x22b
#define IDMS_SAVE_STOP_MEDIA_FILE_FR      0x22c
#define IDMS_SAVE_STOP_MEDIA_FILE_IT      0x22d
#define IDMS_SAVE_STOP_MEDIA_FILE_NL      0x22e
#define IDMS_SAVE_STOP_MEDIA_FILE_PL      0x22f
#define IDMS_SAVE_STOP_MEDIA_FILE_SV      0x230

#define IDMS_NETPLAY                      0x231
#define IDMS_NETPLAY_DE                   0x232
#define IDMS_NETPLAY_FR                   0x233
#define IDMS_NETPLAY_IT                   0x234
#define IDMS_NETPLAY_NL                   0x235
#define IDMS_NETPLAY_PL                   0x236
#define IDMS_NETPLAY_SV                   0x237

#define IDMS_OPTIONS                      0x238
#define IDMS_OPTIONS_DE                   0x239
#define IDMS_OPTIONS_FR                   0x23a
#define IDMS_OPTIONS_IT                   0x23b
#define IDMS_OPTIONS_NL                   0x23c
#define IDMS_OPTIONS_PL                   0x23d
#define IDMS_OPTIONS_SV                   0x23e

#define IDMS_REFRESH_RATE                 0x23f
#define IDMS_REFRESH_RATE_DE              0x240
#define IDMS_REFRESH_RATE_FR              0x241
#define IDMS_REFRESH_RATE_IT              0x242
#define IDMS_REFRESH_RATE_NL              0x243
#define IDMS_REFRESH_RATE_PL              0x244
#define IDMS_REFRESH_RATE_SV              0x245

#define IDMS_AUTO                         0x246
#define IDMS_AUTO_DE                      0x247
#define IDMS_AUTO_FR                      0x248
#define IDMS_AUTO_IT                      0x249
#define IDMS_AUTO_NL                      0x24a
#define IDMS_AUTO_PL                      0x24b
#define IDMS_AUTO_SV                      0x24c

#define IDMS_1_1                          0x24d
#define IDMS_1_1_DE                       0x24e
#define IDMS_1_1_FR                       0x24f
#define IDMS_1_1_IT                       0x250
#define IDMS_1_1_NL                       0x251
#define IDMS_1_1_PL                       0x252
#define IDMS_1_1_SV                       0x253

#define IDMS_1_2                          0x254
#define IDMS_1_2_DE                       0x255
#define IDMS_1_2_FR                       0x256
#define IDMS_1_2_IT                       0x257
#define IDMS_1_2_NL                       0x258
#define IDMS_1_2_PL                       0x259
#define IDMS_1_2_SV                       0x25a

#define IDMS_1_3                          0x25b
#define IDMS_1_3_DE                       0x25c
#define IDMS_1_3_FR                       0x25d
#define IDMS_1_3_IT                       0x25e
#define IDMS_1_3_NL                       0x25f
#define IDMS_1_3_PL                       0x260
#define IDMS_1_3_SV                       0x261

#define IDMS_1_4                          0x262
#define IDMS_1_4_DE                       0x263
#define IDMS_1_4_FR                       0x264
#define IDMS_1_4_IT                       0x265
#define IDMS_1_4_NL                       0x266
#define IDMS_1_4_PL                       0x267
#define IDMS_1_4_SV                       0x268

#define IDMS_1_5                          0x269
#define IDMS_1_5_DE                       0x26a
#define IDMS_1_5_FR                       0x26b
#define IDMS_1_5_IT                       0x26c
#define IDMS_1_5_NL                       0x26d
#define IDMS_1_5_PL                       0x26e
#define IDMS_1_5_SV                       0x26f

#define IDMS_1_6                          0x270
#define IDMS_1_6_DE                       0x271
#define IDMS_1_6_FR                       0x272
#define IDMS_1_6_IT                       0x273
#define IDMS_1_6_NL                       0x274
#define IDMS_1_6_PL                       0x275
#define IDMS_1_6_SV                       0x276

#define IDMS_1_7                          0x277
#define IDMS_1_7_DE                       0x278
#define IDMS_1_7_FR                       0x279
#define IDMS_1_7_IT                       0x27a
#define IDMS_1_7_NL                       0x27b
#define IDMS_1_7_PL                       0x27c
#define IDMS_1_7_SV                       0x27d

#define IDMS_1_8                          0x27e
#define IDMS_1_8_DE                       0x27f
#define IDMS_1_8_FR                       0x280
#define IDMS_1_8_IT                       0x281
#define IDMS_1_8_NL                       0x282
#define IDMS_1_8_PL                       0x283
#define IDMS_1_8_SV                       0x284

#define IDMS_1_9                          0x285
#define IDMS_1_9_DE                       0x286
#define IDMS_1_9_FR                       0x287
#define IDMS_1_9_IT                       0x288
#define IDMS_1_9_NL                       0x289
#define IDMS_1_9_PL                       0x28a
#define IDMS_1_9_SV                       0x28b

#define IDMS_1_10                         0x28c
#define IDMS_1_10_DE                      0x28d
#define IDMS_1_10_FR                      0x28e
#define IDMS_1_10_IT                      0x28f
#define IDMS_1_10_NL                      0x290
#define IDMS_1_10_PL                      0x291
#define IDMS_1_10_SV                      0x292

#define IDMS_MAXIMUM_SPEED                0x293
#define IDMS_MAXIMUM_SPEED_DE             0x294
#define IDMS_MAXIMUM_SPEED_FR             0x295
#define IDMS_MAXIMUM_SPEED_IT             0x296
#define IDMS_MAXIMUM_SPEED_NL             0x297
#define IDMS_MAXIMUM_SPEED_PL             0x298
#define IDMS_MAXIMUM_SPEED_SV             0x299

#define IDMS_200_PERCENT                  0x29a
#define IDMS_200_PERCENT_DE               0x29b
#define IDMS_200_PERCENT_FR               0x29c
#define IDMS_200_PERCENT_IT               0x29d
#define IDMS_200_PERCENT_NL               0x29e
#define IDMS_200_PERCENT_PL               0x29f
#define IDMS_200_PERCENT_SV               0x2a0

#define IDMS_100_PERCENT                  0x2a1
#define IDMS_100_PERCENT_DE               0x2a2
#define IDMS_100_PERCENT_FR               0x2a3
#define IDMS_100_PERCENT_IT               0x2a4
#define IDMS_100_PERCENT_NL               0x2a5
#define IDMS_100_PERCENT_PL               0x2a6
#define IDMS_100_PERCENT_SV               0x2a7

#define IDMS_50_PERCENT                   0x2a8
#define IDMS_50_PERCENT_DE                0x2a9
#define IDMS_50_PERCENT_FR                0x2aa
#define IDMS_50_PERCENT_IT                0x2ab
#define IDMS_50_PERCENT_NL                0x2ac
#define IDMS_50_PERCENT_PL                0x2ad
#define IDMS_50_PERCENT_SV                0x2ae

#define IDMS_20_PERCENT                   0x2af
#define IDMS_20_PERCENT_DE                0x2b0
#define IDMS_20_PERCENT_FR                0x2b1
#define IDMS_20_PERCENT_IT                0x2b2
#define IDMS_20_PERCENT_NL                0x2b3
#define IDMS_20_PERCENT_PL                0x2b4
#define IDMS_20_PERCENT_SV                0x2b5

#define IDMS_10_PERCENT                   0x2b6
#define IDMS_10_PERCENT_DE                0x2b7
#define IDMS_10_PERCENT_FR                0x2b8
#define IDMS_10_PERCENT_IT                0x2b9
#define IDMS_10_PERCENT_NL                0x2ba
#define IDMS_10_PERCENT_PL                0x2bb
#define IDMS_10_PERCENT_SV                0x2bc

#define IDMS_NO_LIMIT                     0x2bd
#define IDMS_NO_LIMIT_DE                  0x2be
#define IDMS_NO_LIMIT_FR                  0x2bf
#define IDMS_NO_LIMIT_IT                  0x2c0
#define IDMS_NO_LIMIT_NL                  0x2c1
#define IDMS_NO_LIMIT_PL                  0x2c2
#define IDMS_NO_LIMIT_SV                  0x2c3

#define IDMS_CUSTOM                       0x2c4
#define IDMS_CUSTOM_DE                    0x2c5
#define IDMS_CUSTOM_FR                    0x2c6
#define IDMS_CUSTOM_IT                    0x2c7
#define IDMS_CUSTOM_NL                    0x2c8
#define IDMS_CUSTOM_PL                    0x2c9
#define IDMS_CUSTOM_SV                    0x2ca

#define IDMS_WARP_MODE                    0x2cb
#define IDMS_WARP_MODE_DE                 0x2cc
#define IDMS_WARP_MODE_FR                 0x2cd
#define IDMS_WARP_MODE_IT                 0x2ce
#define IDMS_WARP_MODE_NL                 0x2cf
#define IDMS_WARP_MODE_PL                 0x2d0
#define IDMS_WARP_MODE_SV                 0x2d1

#define IDMS_FULLSCREEN                   0x2d2
#define IDMS_FULLSCREEN_DE                0x2d3
#define IDMS_FULLSCREEN_FR                0x2d4
#define IDMS_FULLSCREEN_IT                0x2d5
#define IDMS_FULLSCREEN_NL                0x2d6
#define IDMS_FULLSCREEN_PL                0x2d7
#define IDMS_FULLSCREEN_SV                0x2d8

#define IDMS_VIDEO_CACHE                  0x2d9
#define IDMS_VIDEO_CACHE_DE               0x2da
#define IDMS_VIDEO_CACHE_FR               0x2db
#define IDMS_VIDEO_CACHE_IT               0x2dc
#define IDMS_VIDEO_CACHE_NL               0x2dd
#define IDMS_VIDEO_CACHE_PL               0x2de
#define IDMS_VIDEO_CACHE_SV               0x2df

#define IDMS_DOUBLE_SIZE                  0x2e0
#define IDMS_DOUBLE_SIZE_DE               0x2e1
#define IDMS_DOUBLE_SIZE_FR               0x2e2
#define IDMS_DOUBLE_SIZE_IT               0x2e3
#define IDMS_DOUBLE_SIZE_NL               0x2e4
#define IDMS_DOUBLE_SIZE_PL               0x2e5
#define IDMS_DOUBLE_SIZE_SV               0x2e6

#define IDMS_DOUBLE_SCAN                  0x2e7
#define IDMS_DOUBLE_SCAN_DE               0x2e8
#define IDMS_DOUBLE_SCAN_FR               0x2e9
#define IDMS_DOUBLE_SCAN_IT               0x2ea
#define IDMS_DOUBLE_SCAN_NL               0x2eb
#define IDMS_DOUBLE_SCAN_PL               0x2ec
#define IDMS_DOUBLE_SCAN_SV               0x2ed

#define IDMS_PAL_EMULATION                0x2ee
#define IDMS_PAL_EMULATION_DE             0x2ef
#define IDMS_PAL_EMULATION_FR             0x2f0
#define IDMS_PAL_EMULATION_IT             0x2f1
#define IDMS_PAL_EMULATION_NL             0x2f2
#define IDMS_PAL_EMULATION_PL             0x2f3
#define IDMS_PAL_EMULATION_SV             0x2f4

#define IDMS_SCALE2X                      0x2f5
#define IDMS_SCALE2X_DE                   0x2f6
#define IDMS_SCALE2X_FR                   0x2f7
#define IDMS_SCALE2X_IT                   0x2f8
#define IDMS_SCALE2X_NL                   0x2f9
#define IDMS_SCALE2X_PL                   0x2fa
#define IDMS_SCALE2X_SV                   0x2fb

#define IDMS_VDC_SETTINGS                 0x2fc
#define IDMS_VDC_SETTINGS_DE              0x2fd
#define IDMS_VDC_SETTINGS_FR              0x2fe
#define IDMS_VDC_SETTINGS_IT              0x2ff
#define IDMS_VDC_SETTINGS_NL              0x300
#define IDMS_VDC_SETTINGS_PL              0x301
#define IDMS_VDC_SETTINGS_SV              0x302

#define IDMS_64KB_VIDEO_MEMORY            0x303
#define IDMS_64KB_VIDEO_MEMORY_DE         0x304
#define IDMS_64KB_VIDEO_MEMORY_FR         0x305
#define IDMS_64KB_VIDEO_MEMORY_IT         0x306
#define IDMS_64KB_VIDEO_MEMORY_NL         0x307
#define IDMS_64KB_VIDEO_MEMORY_PL         0x308
#define IDMS_64KB_VIDEO_MEMORY_SV         0x309

#define IDMS_SWAP_JOYSTICKS               0x30a
#define IDMS_SWAP_JOYSTICKS_DE            0x30b
#define IDMS_SWAP_JOYSTICKS_FR            0x30c
#define IDMS_SWAP_JOYSTICKS_IT            0x30d
#define IDMS_SWAP_JOYSTICKS_NL            0x30e
#define IDMS_SWAP_JOYSTICKS_PL            0x30f
#define IDMS_SWAP_JOYSTICKS_SV            0x310

#define IDMS_SOUND_PLAYBACK               0x311
#define IDMS_SOUND_PLAYBACK_DE            0x312
#define IDMS_SOUND_PLAYBACK_FR            0x313
#define IDMS_SOUND_PLAYBACK_IT            0x314
#define IDMS_SOUND_PLAYBACK_NL            0x315
#define IDMS_SOUND_PLAYBACK_PL            0x316
#define IDMS_SOUND_PLAYBACK_SV            0x317

#define IDMS_TRUE_DRIVE_EMU               0x318
#define IDMS_TRUE_DRIVE_EMU_DE            0x319
#define IDMS_TRUE_DRIVE_EMU_FR            0x31a
#define IDMS_TRUE_DRIVE_EMU_IT            0x31b
#define IDMS_TRUE_DRIVE_EMU_NL            0x31c
#define IDMS_TRUE_DRIVE_EMU_PL            0x31d
#define IDMS_TRUE_DRIVE_EMU_SV            0x31e

#define IDMS_VIRTUAL_DEVICE_TRAPS         0x31f
#define IDMS_VIRTUAL_DEVICE_TRAPS_DE      0x320
#define IDMS_VIRTUAL_DEVICE_TRAPS_FR      0x321
#define IDMS_VIRTUAL_DEVICE_TRAPS_IT      0x322
#define IDMS_VIRTUAL_DEVICE_TRAPS_NL      0x323
#define IDMS_VIRTUAL_DEVICE_TRAPS_PL      0x324
#define IDMS_VIRTUAL_DEVICE_TRAPS_SV      0x325

#define IDMS_DRIVE_SYNC_FACTOR            0x326
#define IDMS_DRIVE_SYNC_FACTOR_DE         0x327
#define IDMS_DRIVE_SYNC_FACTOR_FR         0x328
#define IDMS_DRIVE_SYNC_FACTOR_IT         0x329
#define IDMS_DRIVE_SYNC_FACTOR_NL         0x32a
#define IDMS_DRIVE_SYNC_FACTOR_PL         0x32b
#define IDMS_DRIVE_SYNC_FACTOR_SV         0x32c

#define IDMS_PAL                          0x32d
#define IDMS_PAL_DE                       0x32e
#define IDMS_PAL_FR                       0x32f
#define IDMS_PAL_IT                       0x330
#define IDMS_PAL_NL                       0x331
#define IDMS_PAL_PL                       0x332
#define IDMS_PAL_SV                       0x333

#define IDMS_NTSC                         0x334
#define IDMS_NTSC_DE                      0x335
#define IDMS_NTSC_FR                      0x336
#define IDMS_NTSC_IT                      0x337
#define IDMS_NTSC_NL                      0x338
#define IDMS_NTSC_PL                      0x339
#define IDMS_NTSC_SV                      0x33a

#define IDMS_VIDEO_STANDARD               0x33b
#define IDMS_VIDEO_STANDARD_DE            0x33c
#define IDMS_VIDEO_STANDARD_FR            0x33d
#define IDMS_VIDEO_STANDARD_IT            0x33e
#define IDMS_VIDEO_STANDARD_NL            0x33f
#define IDMS_VIDEO_STANDARD_PL            0x340
#define IDMS_VIDEO_STANDARD_SV            0x341

#define IDMS_PAL_G                        0x342
#define IDMS_PAL_G_DE                     0x343
#define IDMS_PAL_G_FR                     0x344
#define IDMS_PAL_G_IT                     0x345
#define IDMS_PAL_G_NL                     0x346
#define IDMS_PAL_G_PL                     0x347
#define IDMS_PAL_G_SV                     0x348

#define IDMS_NTSC_M                       0x349
#define IDMS_NTSC_M_DE                    0x34a
#define IDMS_NTSC_M_FR                    0x34b
#define IDMS_NTSC_M_IT                    0x34c
#define IDMS_NTSC_M_NL                    0x34d
#define IDMS_NTSC_M_PL                    0x34e
#define IDMS_NTSC_M_SV                    0x34f

#define IDMS_OLD_NTSC_M                   0x350
#define IDMS_OLD_NTSC_M_DE                0x351
#define IDMS_OLD_NTSC_M_FR                0x352
#define IDMS_OLD_NTSC_M_IT                0x353
#define IDMS_OLD_NTSC_M_NL                0x354
#define IDMS_OLD_NTSC_M_PL                0x355
#define IDMS_OLD_NTSC_M_SV                0x356

#define IDMS_EMU_ID                       0x357
#define IDMS_EMU_ID_DE                    0x358
#define IDMS_EMU_ID_FR                    0x359
#define IDMS_EMU_ID_IT                    0x35a
#define IDMS_EMU_ID_NL                    0x35b
#define IDMS_EMU_ID_PL                    0x35c
#define IDMS_EMU_ID_SV                    0x35d

#define IDMS_VIC_1112_IEEE_488            0x35e
#define IDMS_VIC_1112_IEEE_488_DE         0x35f
#define IDMS_VIC_1112_IEEE_488_FR         0x360
#define IDMS_VIC_1112_IEEE_488_IT         0x361
#define IDMS_VIC_1112_IEEE_488_NL         0x362
#define IDMS_VIC_1112_IEEE_488_PL         0x363
#define IDMS_VIC_1112_IEEE_488_SV         0x364

#define IDMS_IEEE_488_INTERFACE           0x365
#define IDMS_IEEE_488_INTERFACE_DE        0x366
#define IDMS_IEEE_488_INTERFACE_FR        0x367
#define IDMS_IEEE_488_INTERFACE_IT        0x368
#define IDMS_IEEE_488_INTERFACE_NL        0x369
#define IDMS_IEEE_488_INTERFACE_PL        0x36a
#define IDMS_IEEE_488_INTERFACE_SV        0x36b

#define IDMS_1351_MOUSE                   0x36c
#define IDMS_1351_MOUSE_DE                0x36d
#define IDMS_1351_MOUSE_FR                0x36e
#define IDMS_1351_MOUSE_IT                0x36f
#define IDMS_1351_MOUSE_NL                0x370
#define IDMS_1351_MOUSE_PL                0x371
#define IDMS_1351_MOUSE_SV                0x372

#define IDMS_SETTINGS                     0x373
#define IDMS_SETTINGS_DE                  0x374
#define IDMS_SETTINGS_FR                  0x375
#define IDMS_SETTINGS_IT                  0x376
#define IDMS_SETTINGS_NL                  0x377
#define IDMS_SETTINGS_PL                  0x378
#define IDMS_SETTINGS_SV                  0x379

#define IDMS_VIDEO_SETTINGS               0x37a
#define IDMS_VIDEO_SETTINGS_DE            0x37b
#define IDMS_VIDEO_SETTINGS_FR            0x37c
#define IDMS_VIDEO_SETTINGS_IT            0x37d
#define IDMS_VIDEO_SETTINGS_NL            0x37e
#define IDMS_VIDEO_SETTINGS_PL            0x37f
#define IDMS_VIDEO_SETTINGS_SV            0x380

#define IDMS_VIC_SETTINGS                 0x381
#define IDMS_VIC_SETTINGS_DE              0x382
#define IDMS_VIC_SETTINGS_FR              0x383
#define IDMS_VIC_SETTINGS_IT              0x384
#define IDMS_VIC_SETTINGS_NL              0x385
#define IDMS_VIC_SETTINGS_PL              0x386
#define IDMS_VIC_SETTINGS_SV              0x387

#define IDMS_CBM2_SETTINGS                0x388
#define IDMS_CBM2_SETTINGS_DE             0x389
#define IDMS_CBM2_SETTINGS_FR             0x38a
#define IDMS_CBM2_SETTINGS_IT             0x38b
#define IDMS_CBM2_SETTINGS_NL             0x38c
#define IDMS_CBM2_SETTINGS_PL             0x38d
#define IDMS_CBM2_SETTINGS_SV             0x38e

#define IDMS_PET_SETTINGS                 0x38f
#define IDMS_PET_SETTINGS_DE              0x390
#define IDMS_PET_SETTINGS_FR              0x391
#define IDMS_PET_SETTINGS_IT              0x392
#define IDMS_PET_SETTINGS_NL              0x393
#define IDMS_PET_SETTINGS_PL              0x394
#define IDMS_PET_SETTINGS_SV              0x395

#define IDMS_PERIPHERAL_SETTINGS          0x396
#define IDMS_PERIPHERAL_SETTINGS_DE       0x397
#define IDMS_PERIPHERAL_SETTINGS_FR       0x398
#define IDMS_PERIPHERAL_SETTINGS_IT       0x399
#define IDMS_PERIPHERAL_SETTINGS_NL       0x39a
#define IDMS_PERIPHERAL_SETTINGS_PL       0x39b
#define IDMS_PERIPHERAL_SETTINGS_SV       0x39c

#define IDMS_DRIVE_SETTINGS               0x39d
#define IDMS_DRIVE_SETTINGS_DE            0x39e
#define IDMS_DRIVE_SETTINGS_FR            0x39f
#define IDMS_DRIVE_SETTINGS_IT            0x3a0
#define IDMS_DRIVE_SETTINGS_NL            0x3a1
#define IDMS_DRIVE_SETTINGS_PL            0x3a2
#define IDMS_DRIVE_SETTINGS_SV            0x3a3

#define IDMS_DATASETTE_SETTINGS           0x3a4
#define IDMS_DATASETTE_SETTINGS_DE        0x3a5
#define IDMS_DATASETTE_SETTINGS_FR        0x3a6
#define IDMS_DATASETTE_SETTINGS_IT        0x3a7
#define IDMS_DATASETTE_SETTINGS_NL        0x3a8
#define IDMS_DATASETTE_SETTINGS_PL        0x3a9
#define IDMS_DATASETTE_SETTINGS_SV        0x3aa

#define IDMS_PLUS4_SETTINGS               0x3ab
#define IDMS_PLUS4_SETTINGS_DE            0x3ac
#define IDMS_PLUS4_SETTINGS_FR            0x3ad
#define IDMS_PLUS4_SETTINGS_IT            0x3ae
#define IDMS_PLUS4_SETTINGS_NL            0x3af
#define IDMS_PLUS4_SETTINGS_PL            0x3b0
#define IDMS_PLUS4_SETTINGS_SV            0x3b1

#define IDMS_VICII_SETTINGS               0x3b2
#define IDMS_VICII_SETTINGS_DE            0x3b3
#define IDMS_VICII_SETTINGS_FR            0x3b4
#define IDMS_VICII_SETTINGS_IT            0x3b5
#define IDMS_VICII_SETTINGS_NL            0x3b6
#define IDMS_VICII_SETTINGS_PL            0x3b7
#define IDMS_VICII_SETTINGS_SV            0x3b8

#define IDMS_JOYSTICK_SETTINGS            0x3b9
#define IDMS_JOYSTICK_SETTINGS_DE         0x3ba
#define IDMS_JOYSTICK_SETTINGS_FR         0x3bb
#define IDMS_JOYSTICK_SETTINGS_IT         0x3bc
#define IDMS_JOYSTICK_SETTINGS_NL         0x3bd
#define IDMS_JOYSTICK_SETTINGS_PL         0x3be
#define IDMS_JOYSTICK_SETTINGS_SV         0x3bf

#define IDMS_JOYSTICK_DEVICE_SELECT       0x3c0
#define IDMS_JOYSTICK_DEVICE_SELECT_DE    0x3c1
#define IDMS_JOYSTICK_DEVICE_SELECT_FR    0x3c2
#define IDMS_JOYSTICK_DEVICE_SELECT_IT    0x3c3
#define IDMS_JOYSTICK_DEVICE_SELECT_NL    0x3c4
#define IDMS_JOYSTICK_DEVICE_SELECT_PL    0x3c5
#define IDMS_JOYSTICK_DEVICE_SELECT_SV    0x3c6

#define IDMS_JOYSTICK_FIRE_SELECT         0x3c7
#define IDMS_JOYSTICK_FIRE_SELECT_DE      0x3c8
#define IDMS_JOYSTICK_FIRE_SELECT_FR      0x3c9
#define IDMS_JOYSTICK_FIRE_SELECT_IT      0x3ca
#define IDMS_JOYSTICK_FIRE_SELECT_NL      0x3cb
#define IDMS_JOYSTICK_FIRE_SELECT_PL      0x3cc
#define IDMS_JOYSTICK_FIRE_SELECT_SV      0x3cd

#define IDMS_JOYSTICK_FIRE_SELECT         0x3c7
#define IDMS_JOYSTICK_FIRE_SELECT_DE      0x3c8
#define IDMS_JOYSTICK_FIRE_SELECT_FR      0x3c9
#define IDMS_JOYSTICK_FIRE_SELECT_IT      0x3ca
#define IDMS_JOYSTICK_FIRE_SELECT_NL      0x3cb
#define IDMS_JOYSTICK_FIRE_SELECT_PL      0x3cc
#define IDMS_JOYSTICK_FIRE_SELECT_SV      0x3cd

#define IDMS_KEYBOARD_SETTINGS            0x3ce
#define IDMS_KEYBOARD_SETTINGS_DE         0x3cf
#define IDMS_KEYBOARD_SETTINGS_FR         0x3d0
#define IDMS_KEYBOARD_SETTINGS_IT         0x3d1
#define IDMS_KEYBOARD_SETTINGS_NL         0x3d2
#define IDMS_KEYBOARD_SETTINGS_PL         0x3d3
#define IDMS_KEYBOARD_SETTINGS_SV         0x3d4

#define IDMS_SOUND_SETTINGS               0x3d5
#define IDMS_SOUND_SETTINGS_DE            0x3d6
#define IDMS_SOUND_SETTINGS_FR            0x3d7
#define IDMS_SOUND_SETTINGS_IT            0x3d8
#define IDMS_SOUND_SETTINGS_NL            0x3d9
#define IDMS_SOUND_SETTINGS_PL            0x3da
#define IDMS_SOUND_SETTINGS_SV            0x3db

#define IDMS_SID_SETTINGS                 0x3dc
#define IDMS_SID_SETTINGS_DE              0x3dd
#define IDMS_SID_SETTINGS_FR              0x3de
#define IDMS_SID_SETTINGS_IT              0x3df
#define IDMS_SID_SETTINGS_NL              0x3e0
#define IDMS_SID_SETTINGS_PL              0x3e1
#define IDMS_SID_SETTINGS_SV              0x3e2

#define IDMS_ROM_SETTINGS                 0x3e3
#define IDMS_ROM_SETTINGS_DE              0x3e4
#define IDMS_ROM_SETTINGS_FR              0x3e5
#define IDMS_ROM_SETTINGS_IT              0x3e6
#define IDMS_ROM_SETTINGS_NL              0x3e7
#define IDMS_ROM_SETTINGS_PL              0x3e8
#define IDMS_ROM_SETTINGS_SV              0x3e9

#define IDMS_RAM_SETTINGS                 0x3ea
#define IDMS_RAM_SETTINGS_DE              0x3eb
#define IDMS_RAM_SETTINGS_FR              0x3ec
#define IDMS_RAM_SETTINGS_IT              0x3ed
#define IDMS_RAM_SETTINGS_NL              0x3ee
#define IDMS_RAM_SETTINGS_PL              0x3ef
#define IDMS_RAM_SETTINGS_SV              0x3f0

#define IDMS_RS232_SETTINGS               0x3f1
#define IDMS_RS232_SETTINGS_DE            0x3f2
#define IDMS_RS232_SETTINGS_FR            0x3f3
#define IDMS_RS232_SETTINGS_IT            0x3f4
#define IDMS_RS232_SETTINGS_NL            0x3f5
#define IDMS_RS232_SETTINGS_PL            0x3f6
#define IDMS_RS232_SETTINGS_SV            0x3f7

#define IDMS_C128_SETTINGS                0x3f8
#define IDMS_C128_SETTINGS_DE             0x3f9
#define IDMS_C128_SETTINGS_FR             0x3fa
#define IDMS_C128_SETTINGS_IT             0x3fb
#define IDMS_C128_SETTINGS_NL             0x3fc
#define IDMS_C128_SETTINGS_PL             0x3fd
#define IDMS_C128_SETTINGS_SV             0x3fe

#define IDMS_CART_IO_SETTINGS             0x3ff
#define IDMS_CART_IO_SETTINGS_DE          0x400
#define IDMS_CART_IO_SETTINGS_FR          0x401
#define IDMS_CART_IO_SETTINGS_IT          0x402
#define IDMS_CART_IO_SETTINGS_NL          0x403
#define IDMS_CART_IO_SETTINGS_PL          0x404
#define IDMS_CART_IO_SETTINGS_SV          0x405

#define IDMS_REU_SETTINGS                 0x406
#define IDMS_REU_SETTINGS_DE              0x407
#define IDMS_REU_SETTINGS_FR              0x408
#define IDMS_REU_SETTINGS_IT              0x409
#define IDMS_REU_SETTINGS_NL              0x40a
#define IDMS_REU_SETTINGS_PL              0x40b
#define IDMS_REU_SETTINGS_SV              0x40c

#define IDMS_GEORAM_SETTINGS              0x40d
#define IDMS_GEORAM_SETTINGS_DE           0x40e
#define IDMS_GEORAM_SETTINGS_FR           0x40f
#define IDMS_GEORAM_SETTINGS_IT           0x410
#define IDMS_GEORAM_SETTINGS_NL           0x411
#define IDMS_GEORAM_SETTINGS_PL           0x412
#define IDMS_GEORAM_SETTINGS_SV           0x413

#define IDMS_RAMCART_SETTINGS             0x414
#define IDMS_RAMCART_SETTINGS_DE          0x415
#define IDMS_RAMCART_SETTINGS_FR          0x416
#define IDMS_RAMCART_SETTINGS_IT          0x417
#define IDMS_RAMCART_SETTINGS_NL          0x418
#define IDMS_RAMCART_SETTINGS_PL          0x419
#define IDMS_RAMCART_SETTINGS_SV          0x41a

#define IDMS_PLUS60K_SETTINGS             0x41b
#define IDMS_PLUS60K_SETTINGS_DE          0x41c
#define IDMS_PLUS60K_SETTINGS_FR          0x41d
#define IDMS_PLUS60K_SETTINGS_IT          0x41e
#define IDMS_PLUS60K_SETTINGS_NL          0x41f
#define IDMS_PLUS60K_SETTINGS_PL          0x420
#define IDMS_PLUS60K_SETTINGS_SV          0x421

#define IDMS_PLUS256K_SETTINGS            0x422
#define IDMS_PLUS256K_SETTINGS_DE         0x423
#define IDMS_PLUS256K_SETTINGS_FR         0x424
#define IDMS_PLUS256K_SETTINGS_IT         0x425
#define IDMS_PLUS256K_SETTINGS_NL         0x426
#define IDMS_PLUS256K_SETTINGS_PL         0x427
#define IDMS_PLUS256K_SETTINGS_SV         0x428

#define IDMS_256K_SETTINGS                0x429
#define IDMS_256K_SETTINGS_DE             0x42a
#define IDMS_256K_SETTINGS_FR             0x42b
#define IDMS_256K_SETTINGS_IT             0x42c
#define IDMS_256K_SETTINGS_NL             0x42d
#define IDMS_256K_SETTINGS_PL             0x42e
#define IDMS_256K_SETTINGS_SV             0x42f

#define IDMS_IDE64_SETTINGS               0x430
#define IDMS_IDE64_SETTINGS_DE            0x431
#define IDMS_IDE64_SETTINGS_FR            0x432
#define IDMS_IDE64_SETTINGS_IT            0x433
#define IDMS_IDE64_SETTINGS_NL            0x434
#define IDMS_IDE64_SETTINGS_PL            0x435
#define IDMS_IDE64_SETTINGS_SV            0x436

#define IDMS_ETHERNET_SETTINGS            0x437
#define IDMS_ETHERNET_SETTINGS_DE         0x438
#define IDMS_ETHERNET_SETTINGS_FR         0x439
#define IDMS_ETHERNET_SETTINGS_IT         0x43a
#define IDMS_ETHERNET_SETTINGS_NL         0x43b
#define IDMS_ETHERNET_SETTINGS_PL         0x43c
#define IDMS_ETHERNET_SETTINGS_SV         0x43d

#define IDMS_ACIA_SETTINGS                0x43e
#define IDMS_ACIA_SETTINGS_DE             0x43f
#define IDMS_ACIA_SETTINGS_FR             0x440
#define IDMS_ACIA_SETTINGS_IT             0x441
#define IDMS_ACIA_SETTINGS_NL             0x442
#define IDMS_ACIA_SETTINGS_PL             0x443
#define IDMS_ACIA_SETTINGS_SV             0x444

#define IDMS_PETREU_SETTINGS              0x445
#define IDMS_PETREU_SETTINGS_DE           0x446
#define IDMS_PETREU_SETTINGS_FR           0x447
#define IDMS_PETREU_SETTINGS_IT           0x448
#define IDMS_PETREU_SETTINGS_NL           0x449
#define IDMS_PETREU_SETTINGS_PL           0x44a
#define IDMS_PETREU_SETTINGS_SV           0x44b

#define IDMS_RS232_USERPORT_SETTINGS      0x44c
#define IDMS_RS232_USERPORT_SETTINGS_DE   0x44d
#define IDMS_RS232_USERPORT_SETTINGS_FR   0x44e
#define IDMS_RS232_USERPORT_SETTINGS_IT   0x44f
#define IDMS_RS232_USERPORT_SETTINGS_NL   0x450
#define IDMS_RS232_USERPORT_SETTINGS_PL   0x451
#define IDMS_RS232_USERPORT_SETTINGS_SV   0x452

#define IDMS_SAVE_CURRENT_SETTINGS        0x453
#define IDMS_SAVE_CURRENT_SETTINGS_DE     0x454
#define IDMS_SAVE_CURRENT_SETTINGS_FR     0x455
#define IDMS_SAVE_CURRENT_SETTINGS_IT     0x456
#define IDMS_SAVE_CURRENT_SETTINGS_NL     0x457
#define IDMS_SAVE_CURRENT_SETTINGS_PL     0x458
#define IDMS_SAVE_CURRENT_SETTINGS_SV     0x459

#define IDMS_LOAD_SAVED_SETTINGS          0x45a
#define IDMS_LOAD_SAVED_SETTINGS_DE       0x45b
#define IDMS_LOAD_SAVED_SETTINGS_FR       0x45c
#define IDMS_LOAD_SAVED_SETTINGS_IT       0x45d
#define IDMS_LOAD_SAVED_SETTINGS_NL       0x45e
#define IDMS_LOAD_SAVED_SETTINGS_PL       0x45f
#define IDMS_LOAD_SAVED_SETTINGS_SV       0x460

#define IDMS_SET_DEFAULT_SETTINGS         0x461
#define IDMS_SET_DEFAULT_SETTINGS_DE      0x462
#define IDMS_SET_DEFAULT_SETTINGS_FR      0x463
#define IDMS_SET_DEFAULT_SETTINGS_IT      0x464
#define IDMS_SET_DEFAULT_SETTINGS_NL      0x465
#define IDMS_SET_DEFAULT_SETTINGS_PL      0x466
#define IDMS_SET_DEFAULT_SETTINGS_SV      0x467

#define IDMS_SAVE_SETTING_ON_EXIT         0x468
#define IDMS_SAVE_SETTING_ON_EXIT_DE      0x469
#define IDMS_SAVE_SETTING_ON_EXIT_FR      0x46a
#define IDMS_SAVE_SETTING_ON_EXIT_IT      0x46b
#define IDMS_SAVE_SETTING_ON_EXIT_NL      0x46c
#define IDMS_SAVE_SETTING_ON_EXIT_PL      0x46d
#define IDMS_SAVE_SETTING_ON_EXIT_SV      0x46e

#define IDMS_CONFIRM_ON_EXIT              0x46f
#define IDMS_CONFIRM_ON_EXIT_DE           0x470
#define IDMS_CONFIRM_ON_EXIT_FR           0x471
#define IDMS_CONFIRM_ON_EXIT_IT           0x472
#define IDMS_CONFIRM_ON_EXIT_NL           0x473
#define IDMS_CONFIRM_ON_EXIT_PL           0x474
#define IDMS_CONFIRM_ON_EXIT_SV           0x475

#define IDMS_HELP                         0x476
#define IDMS_HELP_DE                      0x477
#define IDMS_HELP_FR                      0x478
#define IDMS_HELP_IT                      0x479
#define IDMS_HELP_NL                      0x47a
#define IDMS_HELP_PL                      0x47b
#define IDMS_HELP_SV                      0x47c

#define IDMS_ABOUT                        0x47d
#define IDMS_ABOUT_DE                     0x47e
#define IDMS_ABOUT_FR                     0x47f
#define IDMS_ABOUT_IT                     0x480
#define IDMS_ABOUT_NL                     0x481
#define IDMS_ABOUT_PL                     0x482
#define IDMS_ABOUT_SV                     0x483

#define IDMS_COMMAND_LINE_OPTIONS         0x484
#define IDMS_COMMAND_LINE_OPTIONS_DE      0x485
#define IDMS_COMMAND_LINE_OPTIONS_FR      0x486
#define IDMS_COMMAND_LINE_OPTIONS_IT      0x487
#define IDMS_COMMAND_LINE_OPTIONS_NL      0x488
#define IDMS_COMMAND_LINE_OPTIONS_PL      0x489
#define IDMS_COMMAND_LINE_OPTIONS_SV      0x48a

#define IDMS_CONTRIBUTORS                 0x48b
#define IDMS_CONTRIBUTORS_DE              0x48c
#define IDMS_CONTRIBUTORS_FR              0x48d
#define IDMS_CONTRIBUTORS_IT              0x48e
#define IDMS_CONTRIBUTORS_NL              0x48f
#define IDMS_CONTRIBUTORS_PL              0x490
#define IDMS_CONTRIBUTORS_SV              0x491

#define IDMS_LICENSE                      0x492
#define IDMS_LICENSE_DE                   0x493
#define IDMS_LICENSE_FR                   0x494
#define IDMS_LICENSE_IT                   0x495
#define IDMS_LICENSE_NL                   0x496
#define IDMS_LICENSE_PL                   0x497
#define IDMS_LICENSE_SV                   0x498

#define IDMS_NO_WARRANTY                  0x499
#define IDMS_NO_WARRANTY_DE               0x49a
#define IDMS_NO_WARRANTY_FR               0x49b
#define IDMS_NO_WARRANTY_IT               0x49c
#define IDMS_NO_WARRANTY_NL               0x49d
#define IDMS_NO_WARRANTY_PL               0x49e
#define IDMS_NO_WARRANTY_SV               0x49f

#define IDMS_LANGUAGE_ENGLISH             0x4a0
#define IDMS_LANGUAGE_ENGLISH_DE          0x4a1
#define IDMS_LANGUAGE_ENGLISH_FR          0x4a2
#define IDMS_LANGUAGE_ENGLISH_IT          0x4a3
#define IDMS_LANGUAGE_ENGLISH_NL          0x4a4
#define IDMS_LANGUAGE_ENGLISH_PL          0x4a5
#define IDMS_LANGUAGE_ENGLISH_SV          0x4a6

#define IDMS_LANGUAGE_GERMAN              0x4a7
#define IDMS_LANGUAGE_GERMAN_DE           0x4a8
#define IDMS_LANGUAGE_GERMAN_FR           0x4a9
#define IDMS_LANGUAGE_GERMAN_IT           0x4aa
#define IDMS_LANGUAGE_GERMAN_NL           0x4ab
#define IDMS_LANGUAGE_GERMAN_PL           0x4ac
#define IDMS_LANGUAGE_GERMAN_SV           0x4ad

#define IDMS_LANGUAGE_FRENCH              0x4ae
#define IDMS_LANGUAGE_FRENCH_DE           0x4af
#define IDMS_LANGUAGE_FRENCH_FR           0x4b0
#define IDMS_LANGUAGE_FRENCH_IT           0x4b1
#define IDMS_LANGUAGE_FRENCH_NL           0x4b2
#define IDMS_LANGUAGE_FRENCH_PL           0x4b3
#define IDMS_LANGUAGE_FRENCH_SV           0x4b4

#define IDMS_LANGUAGE_ITALIAN             0x4b5
#define IDMS_LANGUAGE_ITALIAN_DE          0x4b6
#define IDMS_LANGUAGE_ITALIAN_FR          0x4b7
#define IDMS_LANGUAGE_ITALIAN_IT          0x4b8
#define IDMS_LANGUAGE_ITALIAN_NL          0x4b9
#define IDMS_LANGUAGE_ITALIAN_PL          0x4ba
#define IDMS_LANGUAGE_ITALIAN_SV          0x4bb

#define IDMS_LANGUAGE_DUTCH               0x4bc
#define IDMS_LANGUAGE_DUTCH_DE            0x4bd
#define IDMS_LANGUAGE_DUTCH_FR            0x4be
#define IDMS_LANGUAGE_DUTCH_IT            0x4bf
#define IDMS_LANGUAGE_DUTCH_NL            0x4c0
#define IDMS_LANGUAGE_DUTCH_PL            0x4c1
#define IDMS_LANGUAGE_DUTCH_SV            0x4c2

#define IDMS_LANGUAGE_POLISH              0x4c3
#define IDMS_LANGUAGE_POLISH_DE           0x4c4
#define IDMS_LANGUAGE_POLISH_FR           0x4c5
#define IDMS_LANGUAGE_POLISH_IT           0x4c6
#define IDMS_LANGUAGE_POLISH_NL           0x4c7
#define IDMS_LANGUAGE_POLISH_PL           0x4c8
#define IDMS_LANGUAGE_POLISH_SV           0x4c9

#define IDMS_LANGUAGE_SWEDISH             0x4ca
#define IDMS_LANGUAGE_SWEDISH_DE          0x4cb
#define IDMS_LANGUAGE_SWEDISH_FR          0x4cc
#define IDMS_LANGUAGE_SWEDISH_IT          0x4cd
#define IDMS_LANGUAGE_SWEDISH_NL          0x4ce
#define IDMS_LANGUAGE_SWEDISH_PL          0x4cf
#define IDMS_LANGUAGE_SWEDISH_SV          0x4d0

#define IDMS_LANGUAGE                     0x4d1
#define IDMS_LANGUAGE_DE                  0x4d2
#define IDMS_LANGUAGE_FR                  0x4d3
#define IDMS_LANGUAGE_IT                  0x4d4
#define IDMS_LANGUAGE_NL                  0x4d5
#define IDMS_LANGUAGE_PL                  0x4d6
#define IDMS_LANGUAGE_SV                  0x4d7

#define IDMS_FULLSCREEN_STATUSBAR         0x59c
#define IDMS_FULLSCREEN_STATUSBAR_DE      0x59d
#define IDMS_FULLSCREEN_STATUSBAR_FR      0x59e
#define IDMS_FULLSCREEN_STATUSBAR_IT      0x59f
#define IDMS_FULLSCREEN_STATUSBAR_NL      0x5a0
#define IDMS_FULLSCREEN_STATUSBAR_PL      0x5a1
#define IDMS_FULLSCREEN_STATUSBAR_SV      0x5a2

#define IDMS_VIDEO_OVERLAY                0x5a3
#define IDMS_VIDEO_OVERLAY_DE             0x5a4
#define IDMS_VIDEO_OVERLAY_FR             0x5a5
#define IDMS_VIDEO_OVERLAY_IT             0x5a6
#define IDMS_VIDEO_OVERLAY_NL             0x5a7
#define IDMS_VIDEO_OVERLAY_PL             0x5a8
#define IDMS_VIDEO_OVERLAY_SV             0x5a9

/* ----------------------- AmigaOS Message/Error Strings ----------------------- */

#define IDMES_SETTINGS_SAVED_SUCCESS      0x4d8
#define IDMES_SETTINGS_SAVED_SUCCESS_DE   0x4d9
#define IDMES_SETTINGS_SAVED_SUCCESS_FR   0x4da
#define IDMES_SETTINGS_SAVED_SUCCESS_IT   0x4db
#define IDMES_SETTINGS_SAVED_SUCCESS_NL   0x4dc
#define IDMES_SETTINGS_SAVED_SUCCESS_PL   0x4dd
#define IDMES_SETTINGS_SAVED_SUCCESS_SV   0x4de

#define IDMES_SETTINGS_LOAD_SUCCESS       0x4df
#define IDMES_SETTINGS_LOAD_SUCCESS_DE    0x4e0
#define IDMES_SETTINGS_LOAD_SUCCESS_FR    0x4e1
#define IDMES_SETTINGS_LOAD_SUCCESS_IT    0x4e2
#define IDMES_SETTINGS_LOAD_SUCCESS_NL    0x4e3
#define IDMES_SETTINGS_LOAD_SUCCESS_PL    0x4e4
#define IDMES_SETTINGS_LOAD_SUCCESS_SV    0x4e5

#define IDMES_DFLT_SETTINGS_RESTORED      0x4e6
#define IDMES_DFLT_SETTINGS_RESTORED_DE   0x4e7
#define IDMES_DFLT_SETTINGS_RESTORED_FR   0x4e8
#define IDMES_DFLT_SETTINGS_RESTORED_IT   0x4e9
#define IDMES_DFLT_SETTINGS_RESTORED_NL   0x4ea
#define IDMES_DFLT_SETTINGS_RESTORED_PL   0x4eb
#define IDMES_DFLT_SETTINGS_RESTORED_SV   0x4ec

#define IDMES_VICE_MESSAGE                0x4ed
#define IDMES_VICE_MESSAGE_DE             0x4ee
#define IDMES_VICE_MESSAGE_FR             0x4ef
#define IDMES_VICE_MESSAGE_IT             0x4f0
#define IDMES_VICE_MESSAGE_NL             0x4f1
#define IDMES_VICE_MESSAGE_PL             0x4f2
#define IDMES_VICE_MESSAGE_SV             0x4f3

#define IDMES_OK                          0x4f4
#define IDMES_OK_DE                       0x4f5
#define IDMES_OK_FR                       0x4f6
#define IDMES_OK_IT                       0x4f7
#define IDMES_OK_NL                       0x4f8
#define IDMES_OK_PL                       0x4f9
#define IDMES_OK_SV                       0x4fa

#define IDMES_VICE_CONTRIBUTORS           0x4fb
#define IDMES_VICE_CONTRIBUTORS_DE        0x4fc
#define IDMES_VICE_CONTRIBUTORS_FR        0x4fd
#define IDMES_VICE_CONTRIBUTORS_IT        0x4fe
#define IDMES_VICE_CONTRIBUTORS_NL        0x4ff
#define IDMES_VICE_CONTRIBUTORS_PL        0x500
#define IDMES_VICE_CONTRIBUTORS_SV        0x501

#define IDMES_WHO_MADE_WHAT               0x502
#define IDMES_WHO_MADE_WHAT_DE            0x503
#define IDMES_WHO_MADE_WHAT_FR            0x504
#define IDMES_WHO_MADE_WHAT_IT            0x505
#define IDMES_WHO_MADE_WHAT_NL            0x506
#define IDMES_WHO_MADE_WHAT_PL            0x507
#define IDMES_WHO_MADE_WHAT_SV            0x508

#define IDMES_VICE_DIST_NO_WARRANTY       0x509
#define IDMES_VICE_DIST_NO_WARRANTY_DE    0x50a
#define IDMES_VICE_DIST_NO_WARRANTY_FR    0x50b
#define IDMES_VICE_DIST_NO_WARRANTY_IT    0x50c
#define IDMES_VICE_DIST_NO_WARRANTY_NL    0x50d
#define IDMES_VICE_DIST_NO_WARRANTY_PL    0x50e
#define IDMES_VICE_DIST_NO_WARRANTY_SV    0x50f

#define IDMES_WHICH_COMMANDS_AVAILABLE    0x510
#define IDMES_WHICH_COMMANDS_AVAILABLE_DE 0x511
#define IDMES_WHICH_COMMANDS_AVAILABLE_FR 0x512
#define IDMES_WHICH_COMMANDS_AVAILABLE_IT 0x513
#define IDMES_WHICH_COMMANDS_AVAILABLE_NL 0x514
#define IDMES_WHICH_COMMANDS_AVAILABLE_PL 0x515
#define IDMES_WHICH_COMMANDS_AVAILABLE_SV 0x516

#define IDMES_CANNOT_SAVE_SETTINGS        0x517
#define IDMES_CANNOT_SAVE_SETTINGS_DE     0x518
#define IDMES_CANNOT_SAVE_SETTINGS_FR     0x519
#define IDMES_CANNOT_SAVE_SETTINGS_IT     0x51a
#define IDMES_CANNOT_SAVE_SETTINGS_NL     0x51b
#define IDMES_CANNOT_SAVE_SETTINGS_PL     0x51c
#define IDMES_CANNOT_SAVE_SETTINGS_SV     0x51d

#define IDMES_CANNOT_LOAD_SETTINGS        0x51e
#define IDMES_CANNOT_LOAD_SETTINGS_DE     0x51f
#define IDMES_CANNOT_LOAD_SETTINGS_FR     0x520
#define IDMES_CANNOT_LOAD_SETTINGS_IT     0x521
#define IDMES_CANNOT_LOAD_SETTINGS_NL     0x522
#define IDMES_CANNOT_LOAD_SETTINGS_PL     0x523
#define IDMES_CANNOT_LOAD_SETTINGS_SV     0x524

#define IDMES_VICE_ERROR                  0x525
#define IDMES_VICE_ERROR_DE               0x526
#define IDMES_VICE_ERROR_FR               0x527
#define IDMES_VICE_ERROR_IT               0x528
#define IDMES_VICE_ERROR_NL               0x529
#define IDMES_VICE_ERROR_PL               0x52a
#define IDMES_VICE_ERROR_SV               0x52b

#define IDMES_NO_JOY_ON_PORT_D            0x52c
#define IDMES_NO_JOY_ON_PORT_D_DE         0x52d
#define IDMES_NO_JOY_ON_PORT_D_FR         0x52e
#define IDMES_NO_JOY_ON_PORT_D_IT         0x52f
#define IDMES_NO_JOY_ON_PORT_D_NL         0x530
#define IDMES_NO_JOY_ON_PORT_D_PL         0x531
#define IDMES_NO_JOY_ON_PORT_D_SV         0x532

#define IDMES_MOUSE_ON_PORT_D             0x533
#define IDMES_MOUSE_ON_PORT_D_DE          0x534
#define IDMES_MOUSE_ON_PORT_D_FR          0x535
#define IDMES_MOUSE_ON_PORT_D_IT          0x536
#define IDMES_MOUSE_ON_PORT_D_NL          0x537
#define IDMES_MOUSE_ON_PORT_D_PL          0x538
#define IDMES_MOUSE_ON_PORT_D_SV          0x539

#define IDMES_UNKNOWN_DEVICE_ON_PORT_D    0x53a
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_DE 0x53b
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_FR 0x53c
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_IT 0x53d
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_NL 0x53e
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_PL 0x53f
#define IDMES_UNKNOWN_DEVICE_ON_PORT_D_SV 0x540

#define IDMES_DEVICE_NOT_GAMEPAD          0x541
#define IDMES_DEVICE_NOT_GAMEPAD_DE       0x542
#define IDMES_DEVICE_NOT_GAMEPAD_FR       0x543
#define IDMES_DEVICE_NOT_GAMEPAD_IT       0x544
#define IDMES_DEVICE_NOT_GAMEPAD_NL       0x545
#define IDMES_DEVICE_NOT_GAMEPAD_PL       0x546
#define IDMES_DEVICE_NOT_GAMEPAD_SV       0x547

#define IDMES_NOT_MAPPED_TO_AMIGA_PORT    0x548
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_DE 0x549
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_FR 0x54a
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_IT 0x54b
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_NL 0x54c
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_PL 0x54d
#define IDMES_NOT_MAPPED_TO_AMIGA_PORT_SV 0x54e

#define IDMES_CANNOT_AUTOSTART_FILE       0x54f
#define IDMES_CANNOT_AUTOSTART_FILE_DE    0x550
#define IDMES_CANNOT_AUTOSTART_FILE_FR    0x551
#define IDMES_CANNOT_AUTOSTART_FILE_IT    0x552
#define IDMES_CANNOT_AUTOSTART_FILE_NL    0x553
#define IDMES_CANNOT_AUTOSTART_FILE_PL    0x554
#define IDMES_CANNOT_AUTOSTART_FILE_SV    0x555

#define IDMES_CANNOT_ATTACH_FILE          0x556
#define IDMES_CANNOT_ATTACH_FILE_DE       0x557
#define IDMES_CANNOT_ATTACH_FILE_FR       0x558
#define IDMES_CANNOT_ATTACH_FILE_IT       0x559
#define IDMES_CANNOT_ATTACH_FILE_NL       0x55a
#define IDMES_CANNOT_ATTACH_FILE_PL       0x55b
#define IDMES_CANNOT_ATTACH_FILE_SV       0x55c

#define IDMES_INVALID_CART                0x55d
#define IDMES_INVALID_CART_DE             0x55e
#define IDMES_INVALID_CART_FR             0x55f
#define IDMES_INVALID_CART_IT             0x560
#define IDMES_INVALID_CART_NL             0x561
#define IDMES_INVALID_CART_PL             0x562
#define IDMES_INVALID_CART_SV             0x563

#define IDMES_BAD_CART_CONFIG_IN_UI       0x564
#define IDMES_BAD_CART_CONFIG_IN_UI_DE    0x565
#define IDMES_BAD_CART_CONFIG_IN_UI_FR    0x566
#define IDMES_BAD_CART_CONFIG_IN_UI_IT    0x567
#define IDMES_BAD_CART_CONFIG_IN_UI_NL    0x568
#define IDMES_BAD_CART_CONFIG_IN_UI_PL    0x569
#define IDMES_BAD_CART_CONFIG_IN_UI_SV    0x56a

#define IDMES_INVALID_CART_IMAGE          0x56b
#define IDMES_INVALID_CART_IMAGE_DE       0x56c
#define IDMES_INVALID_CART_IMAGE_FR       0x56d
#define IDMES_INVALID_CART_IMAGE_IT       0x56e
#define IDMES_INVALID_CART_IMAGE_NL       0x56f
#define IDMES_INVALID_CART_IMAGE_PL       0x570
#define IDMES_INVALID_CART_IMAGE_SV       0x571

#define IDMES_CANNOT_CREATE_IMAGE         0x572
#define IDMES_CANNOT_CREATE_IMAGE_DE      0x573
#define IDMES_CANNOT_CREATE_IMAGE_FR      0x574
#define IDMES_CANNOT_CREATE_IMAGE_IT      0x575
#define IDMES_CANNOT_CREATE_IMAGE_NL      0x576
#define IDMES_CANNOT_CREATE_IMAGE_PL      0x577
#define IDMES_CANNOT_CREATE_IMAGE_SV      0x578

#define IDMES_ERROR_STARTING_SERVER       0x579
#define IDMES_ERROR_STARTING_SERVER_DE    0x57a
#define IDMES_ERROR_STARTING_SERVER_FR    0x57b
#define IDMES_ERROR_STARTING_SERVER_IT    0x57c
#define IDMES_ERROR_STARTING_SERVER_NL    0x57d
#define IDMES_ERROR_STARTING_SERVER_PL    0x57e
#define IDMES_ERROR_STARTING_SERVER_SV    0x57f

#define IDMES_ERROR_CONNECTING_CLIENT     0x580
#define IDMES_ERROR_CONNECTING_CLIENT_DE  0x581
#define IDMES_ERROR_CONNECTING_CLIENT_FR  0x582
#define IDMES_ERROR_CONNECTING_CLIENT_IT  0x583
#define IDMES_ERROR_CONNECTING_CLIENT_NL  0x584
#define IDMES_ERROR_CONNECTING_CLIENT_PL  0x585
#define IDMES_ERROR_CONNECTING_CLIENT_SV  0x586

#define IDMES_INVALID_PORT_NUMBER         0x587
#define IDMES_INVALID_PORT_NUMBER_DE      0x588
#define IDMES_INVALID_PORT_NUMBER_FR      0x589
#define IDMES_INVALID_PORT_NUMBER_IT      0x58a
#define IDMES_INVALID_PORT_NUMBER_NL      0x58b
#define IDMES_INVALID_PORT_NUMBER_PL      0x58c
#define IDMES_INVALID_PORT_NUMBER_SV      0x58d

#define IDMES_THIS_MACHINE_NO_SID         0x58e
#define IDMES_THIS_MACHINE_NO_SID_DE      0x58f
#define IDMES_THIS_MACHINE_NO_SID_FR      0x590
#define IDMES_THIS_MACHINE_NO_SID_IT      0x591
#define IDMES_THIS_MACHINE_NO_SID_NL      0x592
#define IDMES_THIS_MACHINE_NO_SID_PL      0x593
#define IDMES_THIS_MACHINE_NO_SID_SV      0x594

#define IDMES_SCREENSHOT_SAVE_S_FAILED    0x5b1
#define IDMES_SCREENSHOT_SAVE_S_FAILED_DE 0x5b2
#define IDMES_SCREENSHOT_SAVE_S_FAILED_FR 0x5b3
#define IDMES_SCREENSHOT_SAVE_S_FAILED_IT 0x5b4
#define IDMES_SCREENSHOT_SAVE_S_FAILED_NL 0x5b5
#define IDMES_SCREENSHOT_SAVE_S_FAILED_PL 0x5b6
#define IDMES_SCREENSHOT_SAVE_S_FAILED_SV 0x5b7

#define IDMES_SCREENSHOT_S_SAVED          0x5b8
#define IDMES_SCREENSHOT_S_SAVED_DE       0x5b9
#define IDMES_SCREENSHOT_S_SAVED_FR       0x5ba
#define IDMES_SCREENSHOT_S_SAVED_IT       0x5bb
#define IDMES_SCREENSHOT_S_SAVED_NL       0x5bc
#define IDMES_SCREENSHOT_S_SAVED_PL       0x5bd
#define IDMES_SCREENSHOT_S_SAVED_SV       0x5be

/* ----------------------- AmigaOS Strings ----------------------- */

#define IDS_PRESS_KEY_BUTTON              0x595
#define IDS_PRESS_KEY_BUTTON_DE           0x596
#define IDS_PRESS_KEY_BUTTON_FR           0x597
#define IDS_PRESS_KEY_BUTTON_IT           0x598
#define IDS_PRESS_KEY_BUTTON_NL           0x599
#define IDS_PRESS_KEY_BUTTON_PL           0x59a
#define IDS_PRESS_KEY_BUTTON_SV           0x59b

#define IDS_SAVE                          0x5aa
#define IDS_SAVE_DE                       0x5ab
#define IDS_SAVE_FR                       0x5ac
#define IDS_SAVE_IT                       0x5ad
#define IDS_SAVE_NL                       0x5ae
#define IDS_SAVE_PL                       0x5af
#define IDS_SAVE_SV                       0x5b0

#define IDS_SAVE_SCREENSHOT               0x5bf
#define IDS_SAVE_SCREENSHOT_DE            0x5c0
#define IDS_SAVE_SCREENSHOT_FR            0x5c1
#define IDS_SAVE_SCREENSHOT_IT            0x5c2
#define IDS_SAVE_SCREENSHOT_NL            0x5c3
#define IDS_SAVE_SCREENSHOT_PL            0x5c4
#define IDS_SAVE_SCREENSHOT_SV            0x5c5

#define IDS_CANCEL                        0x5c6
#define IDS_CANCEL_DE                     0x5c7
#define IDS_CANCEL_FR                     0x5c8
#define IDS_CANCEL_IT                     0x5c9
#define IDS_CANCEL_NL                     0x5ca
#define IDS_CANCEL_PL                     0x5cb
#define IDS_CANCEL_SV                     0x5cc

#define IDS_CHOOSE_SCREENSHOT_FORMAT      0x5cd
#define IDS_CHOOSE_SCREENSHOT_FORMAT_DE   0x5ce
#define IDS_CHOOSE_SCREENSHOT_FORMAT_FR   0x5cf
#define IDS_CHOOSE_SCREENSHOT_FORMAT_IT   0x5d0
#define IDS_CHOOSE_SCREENSHOT_FORMAT_NL   0x5d1
#define IDS_CHOOSE_SCREENSHOT_FORMAT_PL   0x5d2
#define IDS_CHOOSE_SCREENSHOT_FORMAT_SV   0x5d3

#define IDS_SAVE_S_SCREENSHOT             0x5d4
#define IDS_SAVE_S_SCREENSHOT_DE          0x5d5
#define IDS_SAVE_S_SCREENSHOT_FR          0x5d6
#define IDS_SAVE_S_SCREENSHOT_IT          0x5d7
#define IDS_SAVE_S_SCREENSHOT_NL          0x5d8
#define IDS_SAVE_S_SCREENSHOT_PL          0x5d9
#define IDS_SAVE_S_SCREENSHOT_SV          0x5da

#define IDS_DETACHED_DEVICE_D             0x5db
#define IDS_DETACHED_DEVICE_D_DE          0x5dc
#define IDS_DETACHED_DEVICE_D_FR          0x5dd
#define IDS_DETACHED_DEVICE_D_IT          0x5de
#define IDS_DETACHED_DEVICE_D_NL          0x5df
#define IDS_DETACHED_DEVICE_D_PL          0x5e0
#define IDS_DETACHED_DEVICE_D_SV          0x5e1

#define IDS_ATTACHED_S_TO_DEVICE_D        0x5e2
#define IDS_ATTACHED_S_TO_DEVICE_D_DE     0x5e3
#define IDS_ATTACHED_S_TO_DEVICE_D_FR     0x5e4
#define IDS_ATTACHED_S_TO_DEVICE_D_IT     0x5e5
#define IDS_ATTACHED_S_TO_DEVICE_D_NL     0x5e6
#define IDS_ATTACHED_S_TO_DEVICE_D_PL     0x5e7
#define IDS_ATTACHED_S_TO_DEVICE_D_SV     0x5e8

#define IDS_VICE_QUESTION                 0x5e9
#define IDS_VICE_QUESTION_DE              0x5ea
#define IDS_VICE_QUESTION_FR              0x5eb
#define IDS_VICE_QUESTION_IT              0x5ec
#define IDS_VICE_QUESTION_NL              0x5ed
#define IDS_VICE_QUESTION_PL              0x5ee
#define IDS_VICE_QUESTION_SV              0x5ef

#define IDS_EXTEND_TO_40_TRACK            0x5f0
#define IDS_EXTEND_TO_40_TRACK_DE         0x5f1
#define IDS_EXTEND_TO_40_TRACK_FR         0x5f2
#define IDS_EXTEND_TO_40_TRACK_IT         0x5f3
#define IDS_EXTEND_TO_40_TRACK_NL         0x5f4
#define IDS_EXTEND_TO_40_TRACK_PL         0x5f5
#define IDS_EXTEND_TO_40_TRACK_SV         0x5f6

#define IDS_YES_NO                        0x5f7
#define IDS_YES_NO_DE                     0x5f8
#define IDS_YES_NO_FR                     0x5f9
#define IDS_YES_NO_IT                     0x5fa
#define IDS_YES_NO_NL                     0x5fb
#define IDS_YES_NO_PL                     0x5fc
#define IDS_YES_NO_SV                     0x5fd

#define IDS_DETACHED_TAPE                 0x5fe
#define IDS_DETACHED_TAPE_DE              0x5ff
#define IDS_DETACHED_TAPE_FR              0x600
#define IDS_DETACHED_TAPE_IT              0x601
#define IDS_DETACHED_TAPE_NL              0x602
#define IDS_DETACHED_TAPE_PL              0x603
#define IDS_DETACHED_TAPE_SV              0x604

#define IDS_ATTACHED_TAPE_S               0x605
#define IDS_ATTACHED_TAPE_S_DE            0x606
#define IDS_ATTACHED_TAPE_S_FR            0x607
#define IDS_ATTACHED_TAPE_S_IT            0x608
#define IDS_ATTACHED_TAPE_S_NL            0x609
#define IDS_ATTACHED_TAPE_S_PL            0x60a
#define IDS_ATTACHED_TAPE_S_SV            0x60b

#define IDS_SELECT_START_SNAPSHOT         0x60c
#define IDS_SELECT_START_SNAPSHOT_DE      0x60d
#define IDS_SELECT_START_SNAPSHOT_FR      0x60e
#define IDS_SELECT_START_SNAPSHOT_IT      0x60f
#define IDS_SELECT_START_SNAPSHOT_NL      0x610
#define IDS_SELECT_START_SNAPSHOT_PL      0x611
#define IDS_SELECT_START_SNAPSHOT_SV      0x612

#define IDS_SELECT_END_SNAPSHOT           0x613
#define IDS_SELECT_END_SNAPSHOT_DE        0x614
#define IDS_SELECT_END_SNAPSHOT_FR        0x615
#define IDS_SELECT_END_SNAPSHOT_IT        0x616
#define IDS_SELECT_END_SNAPSHOT_NL        0x617
#define IDS_SELECT_END_SNAPSHOT_PL        0x618
#define IDS_SELECT_END_SNAPSHOT_SV        0x619

#define IDS_VICE_QUIT                     0x61a
#define IDS_VICE_QUIT_DE                  0x61b
#define IDS_VICE_QUIT_FR                  0x61c
#define IDS_VICE_QUIT_IT                  0x61d
#define IDS_VICE_QUIT_NL                  0x61e
#define IDS_VICE_QUIT_PL                  0x61f
#define IDS_VICE_QUIT_SV                  0x620

#define IDS_REALLY_EXIT                   0x621
#define IDS_REALLY_EXIT_DE                0x622
#define IDS_REALLY_EXIT_FR                0x623
#define IDS_REALLY_EXIT_IT                0x624
#define IDS_REALLY_EXIT_NL                0x625
#define IDS_REALLY_EXIT_PL                0x626
#define IDS_REALLY_EXIT_SV                0x627

#define IDS_ATTACH_CRT_IMAGE              0x628
#define IDS_ATTACH_CRT_IMAGE_DE           0x629
#define IDS_ATTACH_CRT_IMAGE_FR           0x62a
#define IDS_ATTACH_CRT_IMAGE_IT           0x62b
#define IDS_ATTACH_CRT_IMAGE_NL           0x62c
#define IDS_ATTACH_CRT_IMAGE_PL           0x62d
#define IDS_ATTACH_CRT_IMAGE_SV           0x62e

#define IDS_ATTACH_RAW_8KB_IMAGE          0x62f
#define IDS_ATTACH_RAW_8KB_IMAGE_DE       0x630
#define IDS_ATTACH_RAW_8KB_IMAGE_FR       0x631
#define IDS_ATTACH_RAW_8KB_IMAGE_IT       0x632
#define IDS_ATTACH_RAW_8KB_IMAGE_NL       0x633
#define IDS_ATTACH_RAW_8KB_IMAGE_PL       0x634
#define IDS_ATTACH_RAW_8KB_IMAGE_SV       0x635

#define IDS_ATTACH_RAW_16KB_IMAGE         0x636
#define IDS_ATTACH_RAW_16KB_IMAGE_DE      0x637
#define IDS_ATTACH_RAW_16KB_IMAGE_FR      0x638
#define IDS_ATTACH_RAW_16KB_IMAGE_IT      0x639
#define IDS_ATTACH_RAW_16KB_IMAGE_NL      0x63a
#define IDS_ATTACH_RAW_16KB_IMAGE_PL      0x63b
#define IDS_ATTACH_RAW_16KB_IMAGE_SV      0x63c

#define IDS_ATTACH_ACTION_REPLAY_IMAGE    0x63d
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_DE 0x63e
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_FR 0x63f
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_IT 0x640
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_NL 0x641
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_PL 0x642
#define IDS_ATTACH_ACTION_REPLAY_IMAGE_SV 0x643

#define IDS_ATTACH_ATOMIC_POWER_IMAGE     0x644
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_DE  0x645
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_FR  0x646
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_IT  0x647
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_NL  0x648
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_PL  0x649
#define IDS_ATTACH_ATOMIC_POWER_IMAGE_SV  0x64a

#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE    0x64b
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_DE 0x64c
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_FR 0x64d
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_IT 0x64e
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_NL 0x64f
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_PL 0x650
#define IDS_ATTACH_EPYX_FASTLOAD_IMAGE_SV 0x651

#define IDS_ATTACH_IEEE488_IMAGE          0x652
#define IDS_ATTACH_IEEE488_IMAGE_DE       0x653
#define IDS_ATTACH_IEEE488_IMAGE_FR       0x654
#define IDS_ATTACH_IEEE488_IMAGE_IT       0x655
#define IDS_ATTACH_IEEE488_IMAGE_NL       0x656
#define IDS_ATTACH_IEEE488_IMAGE_PL       0x657
#define IDS_ATTACH_IEEE488_IMAGE_SV       0x658

#define IDS_ATTACH_RETRO_REPLAY_IMAGE     0x659
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_DE  0x65a
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_FR  0x65b
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_IT  0x65c
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_NL  0x65d
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_PL  0x65e
#define IDS_ATTACH_RETRO_REPLAY_IMAGE_SV  0x65f

#define IDS_ATTACH_IDE64_IMAGE            0x660
#define IDS_ATTACH_IDE64_IMAGE_DE         0x661
#define IDS_ATTACH_IDE64_IMAGE_FR         0x662
#define IDS_ATTACH_IDE64_IMAGE_IT         0x663
#define IDS_ATTACH_IDE64_IMAGE_NL         0x664
#define IDS_ATTACH_IDE64_IMAGE_PL         0x665
#define IDS_ATTACH_IDE64_IMAGE_SV         0x666

#define IDS_ATTACH_SS4_IMAGE              0x667
#define IDS_ATTACH_SS4_IMAGE_DE           0x668
#define IDS_ATTACH_SS4_IMAGE_FR           0x669
#define IDS_ATTACH_SS4_IMAGE_IT           0x66a
#define IDS_ATTACH_SS4_IMAGE_NL           0x66b
#define IDS_ATTACH_SS4_IMAGE_PL           0x66c
#define IDS_ATTACH_SS4_IMAGE_SV           0x66d

#define IDS_ATTACH_SS5_IMAGE              0x66e
#define IDS_ATTACH_SS5_IMAGE_DE           0x66f
#define IDS_ATTACH_SS5_IMAGE_FR           0x670
#define IDS_ATTACH_SS5_IMAGE_IT           0x671
#define IDS_ATTACH_SS5_IMAGE_NL           0x672
#define IDS_ATTACH_SS5_IMAGE_PL           0x673
#define IDS_ATTACH_SS5_IMAGE_SV           0x674

#define IDS_ATTACH_SB_IMAGE               0x675
#define IDS_ATTACH_SB_IMAGE_DE            0x676
#define IDS_ATTACH_SB_IMAGE_FR            0x677
#define IDS_ATTACH_SB_IMAGE_IT            0x678
#define IDS_ATTACH_SB_IMAGE_NL            0x679
#define IDS_ATTACH_SB_IMAGE_PL            0x67a
#define IDS_ATTACH_SB_IMAGE_SV            0x67b

#define IDS_AVAILABLE_CMDLINE_OPTIONS     0x67c
#define IDS_AVAILABLE_CMDLINE_OPTIONS_DE  0x67d
#define IDS_AVAILABLE_CMDLINE_OPTIONS_FR  0x67e
#define IDS_AVAILABLE_CMDLINE_OPTIONS_IT  0x67f
#define IDS_AVAILABLE_CMDLINE_OPTIONS_NL  0x680
#define IDS_AVAILABLE_CMDLINE_OPTIONS_PL  0x681
#define IDS_AVAILABLE_CMDLINE_OPTIONS_SV  0x682

#define IDS_ATTACH_4_8_16KB_AT_2000       0x683
#define IDS_ATTACH_4_8_16KB_AT_2000_DE    0x684
#define IDS_ATTACH_4_8_16KB_AT_2000_FR    0x685
#define IDS_ATTACH_4_8_16KB_AT_2000_IT    0x686
#define IDS_ATTACH_4_8_16KB_AT_2000_NL    0x687
#define IDS_ATTACH_4_8_16KB_AT_2000_PL    0x688
#define IDS_ATTACH_4_8_16KB_AT_2000_SV    0x689

#define IDS_ATTACH_4_8_16KB_AT_4000       0x68a
#define IDS_ATTACH_4_8_16KB_AT_4000_DE    0x68b
#define IDS_ATTACH_4_8_16KB_AT_4000_FR    0x68c
#define IDS_ATTACH_4_8_16KB_AT_4000_IT    0x68d
#define IDS_ATTACH_4_8_16KB_AT_4000_NL    0x68e
#define IDS_ATTACH_4_8_16KB_AT_4000_PL    0x68f
#define IDS_ATTACH_4_8_16KB_AT_4000_SV    0x690

#define IDS_ATTACH_4_8_16KB_AT_6000       0x691
#define IDS_ATTACH_4_8_16KB_AT_6000_DE    0x692
#define IDS_ATTACH_4_8_16KB_AT_6000_FR    0x693
#define IDS_ATTACH_4_8_16KB_AT_6000_IT    0x694
#define IDS_ATTACH_4_8_16KB_AT_6000_NL    0x695
#define IDS_ATTACH_4_8_16KB_AT_6000_PL    0x696
#define IDS_ATTACH_4_8_16KB_AT_6000_SV    0x697

#define IDS_ATTACH_4_8KB_AT_A000          0x698
#define IDS_ATTACH_4_8KB_AT_A000_DE       0x699
#define IDS_ATTACH_4_8KB_AT_A000_FR       0x69a
#define IDS_ATTACH_4_8KB_AT_A000_IT       0x69b
#define IDS_ATTACH_4_8KB_AT_A000_NL       0x69c
#define IDS_ATTACH_4_8KB_AT_A000_PL       0x69d
#define IDS_ATTACH_4_8KB_AT_A000_SV       0x69e

#define IDS_ATTACH_4KB_AT_B000            0x69f
#define IDS_ATTACH_4KB_AT_B000_DE         0x6a0
#define IDS_ATTACH_4KB_AT_B000_FR         0x6a1
#define IDS_ATTACH_4KB_AT_B000_IT         0x6a2
#define IDS_ATTACH_4KB_AT_B000_NL         0x6a3
#define IDS_ATTACH_4KB_AT_B000_PL         0x6a4
#define IDS_ATTACH_4KB_AT_B000_SV         0x6a5

#define IDS_S_AT_D_SPEED                  0x6a6
#define IDS_S_AT_D_SPEED_DE               0x6a7
#define IDS_S_AT_D_SPEED_FR               0x6a8
#define IDS_S_AT_D_SPEED_IT               0x6a9
#define IDS_S_AT_D_SPEED_NL               0x6aa
#define IDS_S_AT_D_SPEED_PL               0x6ab
#define IDS_S_AT_D_SPEED_SV               0x6ac

#define IDS_ATTACH                        0x6ad
#define IDS_ATTACH_DE                     0x6ae
#define IDS_ATTACH_FR                     0x6af
#define IDS_ATTACH_IT                     0x6b0
#define IDS_ATTACH_NL                     0x6b1
#define IDS_ATTACH_PL                     0x6b2
#define IDS_ATTACH_SV                     0x6b3

#define IDS_PARENT                        0x6b4
#define IDS_PARENT_DE                     0x6b5
#define IDS_PARENT_FR                     0x6b6
#define IDS_PARENT_IT                     0x6b7
#define IDS_PARENT_NL                     0x6b8
#define IDS_PARENT_PL                     0x6b9
#define IDS_PARENT_SV                     0x6ba

#define IDS_ATTACH_READ_ONLY              0x6bb
#define IDS_ATTACH_READ_ONLY_DE           0x6bc
#define IDS_ATTACH_READ_ONLY_FR           0x6bd
#define IDS_ATTACH_READ_ONLY_IT           0x6be
#define IDS_ATTACH_READ_ONLY_NL           0x6bf
#define IDS_ATTACH_READ_ONLY_PL           0x6c0
#define IDS_ATTACH_READ_ONLY_SV           0x6c1

#define IDS_NAME                          0x6c2
#define IDS_NAME_DE                       0x6c3
#define IDS_NAME_FR                       0x6c4
#define IDS_NAME_IT                       0x6c5
#define IDS_NAME_NL                       0x6c6
#define IDS_NAME_PL                       0x6c7
#define IDS_NAME_SV                       0x6c8

#define IDS_CREATE_IMAGE                  0x6c9
#define IDS_CREATE_IMAGE_DE               0x6ca
#define IDS_CREATE_IMAGE_FR               0x6cb
#define IDS_CREATE_IMAGE_IT               0x6cc
#define IDS_CREATE_IMAGE_NL               0x6cd
#define IDS_CREATE_IMAGE_PL               0x6ce
#define IDS_CREATE_IMAGE_SV               0x6cf

#define IDS_NEW_IMAGE                     0x6d0
#define IDS_NEW_IMAGE_DE                  0x6d1
#define IDS_NEW_IMAGE_FR                  0x6d2
#define IDS_NEW_IMAGE_IT                  0x6d3
#define IDS_NEW_IMAGE_NL                  0x6d4
#define IDS_NEW_IMAGE_PL                  0x6d5
#define IDS_NEW_IMAGE_SV                  0x6d6

#define IDS_NEW_TAP_IMAGE                 0x6d7
#define IDS_NEW_TAP_IMAGE_DE              0x6d8
#define IDS_NEW_TAP_IMAGE_FR              0x6d9
#define IDS_NEW_TAP_IMAGE_IT              0x6da
#define IDS_NEW_TAP_IMAGE_NL              0x6db
#define IDS_NEW_TAP_IMAGE_PL              0x6dc
#define IDS_NEW_TAP_IMAGE_SV              0x6dd

#define IDS_OVERWRITE_EXISTING_IMAGE      0x6de
#define IDS_OVERWRITE_EXISTING_IMAGE_DE   0x6df
#define IDS_OVERWRITE_EXISTING_IMAGE_FR   0x6e0
#define IDS_OVERWRITE_EXISTING_IMAGE_IT   0x6e1
#define IDS_OVERWRITE_EXISTING_IMAGE_NL   0x6e2
#define IDS_OVERWRITE_EXISTING_IMAGE_PL   0x6e3
#define IDS_OVERWRITE_EXISTING_IMAGE_SV   0x6e4

#define IDS_IMAGE_CONTENTS                0x6e5
#define IDS_IMAGE_CONTENTS_DE             0x6e6
#define IDS_IMAGE_CONTENTS_FR             0x6e7
#define IDS_IMAGE_CONTENTS_IT             0x6e8
#define IDS_IMAGE_CONTENTS_NL             0x6e9
#define IDS_IMAGE_CONTENTS_PL             0x6ea
#define IDS_IMAGE_CONTENTS_SV             0x6eb

#define IDS_ABOUT                         0x6ec
#define IDS_ABOUT_DE                      0x6ed
#define IDS_ABOUT_FR                      0x6ee
#define IDS_ABOUT_IT                      0x6ef
#define IDS_ABOUT_NL                      0x6f0
#define IDS_ABOUT_PL                      0x6f1
#define IDS_ABOUT_SV                      0x6f2

#define IDMS_DISABLED                     0x6f3
#define IDMS_DISABLED_DE                  0x6f4
#define IDMS_DISABLED_FR                  0x6f5
#define IDMS_DISABLED_IT                  0x6f6
#define IDMS_DISABLED_NL                  0x6f7
#define IDMS_DISABLED_PL                  0x6f8
#define IDMS_DISABLED_SV                  0x6f9

#define IDS_ENABLED                       0x6fa
#define IDS_ENABLED_DE                    0x6fb
#define IDS_ENABLED_FR                    0x6fc
#define IDS_ENABLED_IT                    0x6fd
#define IDS_ENABLED_NL                    0x6fe
#define IDS_ENABLED_PL                    0x6ff
#define IDS_ENABLED_SV                    0x700

#define IDS_RS232_DEVICE_1                0x701
#define IDS_RS232_DEVICE_1_DE             0x702
#define IDS_RS232_DEVICE_1_FR             0x703
#define IDS_RS232_DEVICE_1_IT             0x704
#define IDS_RS232_DEVICE_1_NL             0x705
#define IDS_RS232_DEVICE_1_PL             0x706
#define IDS_RS232_DEVICE_1_SV             0x707

#define IDS_RS232_DEVICE_2                0x708
#define IDS_RS232_DEVICE_2_DE             0x709
#define IDS_RS232_DEVICE_2_FR             0x70a
#define IDS_RS232_DEVICE_2_IT             0x70b
#define IDS_RS232_DEVICE_2_NL             0x70c
#define IDS_RS232_DEVICE_2_PL             0x70d
#define IDS_RS232_DEVICE_2_SV             0x70e

#define IDS_RS232_DEVICE_3                0x70f
#define IDS_RS232_DEVICE_3_DE             0x710
#define IDS_RS232_DEVICE_3_FR             0x711
#define IDS_RS232_DEVICE_3_IT             0x712
#define IDS_RS232_DEVICE_3_NL             0x713
#define IDS_RS232_DEVICE_3_PL             0x714
#define IDS_RS232_DEVICE_3_SV             0x715

#define IDS_RS232_DEVICE_4                0x716
#define IDS_RS232_DEVICE_4_DE             0x717
#define IDS_RS232_DEVICE_4_FR             0x718
#define IDS_RS232_DEVICE_4_IT             0x719
#define IDS_RS232_DEVICE_4_NL             0x71a
#define IDS_RS232_DEVICE_4_PL             0x71b
#define IDS_RS232_DEVICE_4_SV             0x71c

#define IDS_NONE                          0x71d
#define IDS_NONE_DE                       0x71e
#define IDS_NONE_FR                       0x71f
#define IDS_NONE_IT                       0x720
#define IDS_NONE_NL                       0x721
#define IDS_NONE_PL                       0x722
#define IDS_NONE_SV                       0x723

#define IDS_IRQ                           0x724
#define IDS_IRQ_DE                        0x725
#define IDS_IRQ_FR                        0x726
#define IDS_IRQ_IT                        0x727
#define IDS_IRQ_NL                        0x728
#define IDS_IRQ_PL                        0x729
#define IDS_IRQ_SV                        0x72a

#define IDS_NMI                           0x72b
#define IDS_NMI_DE                        0x72c
#define IDS_NMI_FR                        0x72d
#define IDS_NMI_IT                        0x72e
#define IDS_NMI_NL                        0x72f
#define IDS_NMI_PL                        0x730
#define IDS_NMI_SV                        0x731

#define IDS_ACIA_DEVICE                   0x732
#define IDS_ACIA_DEVICE_DE                0x733
#define IDS_ACIA_DEVICE_FR                0x734
#define IDS_ACIA_DEVICE_IT                0x735
#define IDS_ACIA_DEVICE_NL                0x736
#define IDS_ACIA_DEVICE_PL                0x737
#define IDS_ACIA_DEVICE_SV                0x738

#define IDS_ACIA_INTERRUPT                0x739
#define IDS_ACIA_INTERRUPT_DE             0x73a
#define IDS_ACIA_INTERRUPT_FR             0x73b
#define IDS_ACIA_INTERRUPT_IT             0x73c
#define IDS_ACIA_INTERRUPT_NL             0x73d
#define IDS_ACIA_INTERRUPT_PL             0x73e
#define IDS_ACIA_INTERRUPT_SV             0x73f

#define IDS_ACIA_MODE                     0x740
#define IDS_ACIA_MODE_DE                  0x741
#define IDS_ACIA_MODE_FR                  0x742
#define IDS_ACIA_MODE_IT                  0x743
#define IDS_ACIA_MODE_NL                  0x744
#define IDS_ACIA_MODE_PL                  0x745
#define IDS_ACIA_MODE_SV                  0x746

#define IDS_ACIA_SETTINGS                 0x747
#define IDS_ACIA_SETTINGS_DE              0x748
#define IDS_ACIA_SETTINGS_FR              0x749
#define IDS_ACIA_SETTINGS_IT              0x74a
#define IDS_ACIA_SETTINGS_NL              0x74b
#define IDS_ACIA_SETTINGS_PL              0x74c
#define IDS_ACIA_SETTINGS_SV              0x74d

#define IDS_256K_ENABLED                  0x74e
#define IDS_256K_ENABLED_DE               0x74f
#define IDS_256K_ENABLED_FR               0x750
#define IDS_256K_ENABLED_IT               0x751
#define IDS_256K_ENABLED_NL               0x752
#define IDS_256K_ENABLED_PL               0x753
#define IDS_256K_ENABLED_SV               0x754

#define IDS_256K_BASE                     0x755
#define IDS_256K_BASE_DE                  0x756
#define IDS_256K_BASE_FR                  0x757
#define IDS_256K_BASE_IT                  0x758
#define IDS_256K_BASE_NL                  0x759
#define IDS_256K_BASE_PL                  0x75a
#define IDS_256K_BASE_SV                  0x75b

#define IDS_256K_SETTINGS                 0x75c
#define IDS_256K_SETTINGS_DE              0x75d
#define IDS_256K_SETTINGS_FR              0x75e
#define IDS_256K_SETTINGS_IT              0x75f
#define IDS_256K_SETTINGS_NL              0x760
#define IDS_256K_SETTINGS_PL              0x761
#define IDS_256K_SETTINGS_SV              0x762

#define IDS_MODEL_LINE                    0x763
#define IDS_MODEL_LINE_DE                 0x764
#define IDS_MODEL_LINE_FR                 0x765
#define IDS_MODEL_LINE_IT                 0x766
#define IDS_MODEL_LINE_NL                 0x767
#define IDS_MODEL_LINE_PL                 0x768
#define IDS_MODEL_LINE_SV                 0x769

#define IDS_RAM_SIZE                      0x76a
#define IDS_RAM_SIZE_DE                   0x76b
#define IDS_RAM_SIZE_FR                   0x76c
#define IDS_RAM_SIZE_IT                   0x76d
#define IDS_RAM_SIZE_NL                   0x76e
#define IDS_RAM_SIZE_PL                   0x76f
#define IDS_RAM_SIZE_SV                   0x770

#define IDS_RAM_BLOCK_0800_0FFF           0x771
#define IDS_RAM_BLOCK_0800_0FFF_DE        0x772
#define IDS_RAM_BLOCK_0800_0FFF_FR        0x773
#define IDS_RAM_BLOCK_0800_0FFF_IT        0x774
#define IDS_RAM_BLOCK_0800_0FFF_NL        0x775
#define IDS_RAM_BLOCK_0800_0FFF_PL        0x776
#define IDS_RAM_BLOCK_0800_0FFF_SV        0x777

#define IDS_RAM_BLOCK_1000_1FFF           0x778
#define IDS_RAM_BLOCK_1000_1FFF_DE        0x779
#define IDS_RAM_BLOCK_1000_1FFF_FR        0x77a
#define IDS_RAM_BLOCK_1000_1FFF_IT        0x77b
#define IDS_RAM_BLOCK_1000_1FFF_NL        0x77c
#define IDS_RAM_BLOCK_1000_1FFF_PL        0x77d
#define IDS_RAM_BLOCK_1000_1FFF_SV        0x77e

#define IDS_RAM_BLOCK_2000_3FFF           0x77f
#define IDS_RAM_BLOCK_2000_3FFF_DE        0x780
#define IDS_RAM_BLOCK_2000_3FFF_FR        0x781
#define IDS_RAM_BLOCK_2000_3FFF_IT        0x782
#define IDS_RAM_BLOCK_2000_3FFF_NL        0x783
#define IDS_RAM_BLOCK_2000_3FFF_PL        0x784
#define IDS_RAM_BLOCK_2000_3FFF_SV        0x785

#define IDS_RAM_BLOCK_4000_5FFF           0x786
#define IDS_RAM_BLOCK_4000_5FFF_DE        0x787
#define IDS_RAM_BLOCK_4000_5FFF_FR        0x788
#define IDS_RAM_BLOCK_4000_5FFF_IT        0x789
#define IDS_RAM_BLOCK_4000_5FFF_NL        0x78a
#define IDS_RAM_BLOCK_4000_5FFF_PL        0x78b
#define IDS_RAM_BLOCK_4000_5FFF_SV        0x78c

#define IDS_RAM_BLOCK_6000_7FFF           0x78d
#define IDS_RAM_BLOCK_6000_7FFF_DE        0x78e
#define IDS_RAM_BLOCK_6000_7FFF_FR        0x78f
#define IDS_RAM_BLOCK_6000_7FFF_IT        0x790
#define IDS_RAM_BLOCK_6000_7FFF_NL        0x791
#define IDS_RAM_BLOCK_6000_7FFF_PL        0x792
#define IDS_RAM_BLOCK_6000_7FFF_SV        0x793

#define IDS_RAM_BLOCK_C000_CFFF           0x794
#define IDS_RAM_BLOCK_C000_CFFF_DE        0x795
#define IDS_RAM_BLOCK_C000_CFFF_FR        0x796
#define IDS_RAM_BLOCK_C000_CFFF_IT        0x797
#define IDS_RAM_BLOCK_C000_CFFF_NL        0x798
#define IDS_RAM_BLOCK_C000_CFFF_PL        0x799
#define IDS_RAM_BLOCK_C000_CFFF_SV        0x79a

#define IDS_CBM2_SETTINGS                 0x79b
#define IDS_CBM2_SETTINGS_DE              0x79c
#define IDS_CBM2_SETTINGS_FR              0x79d
#define IDS_CBM2_SETTINGS_IT              0x79e
#define IDS_CBM2_SETTINGS_NL              0x79f
#define IDS_CBM2_SETTINGS_PL              0x7a0
#define IDS_CBM2_SETTINGS_SV              0x7a1

#define IDS_RESET_DATASETTE_WITH_CPU      0x7a2
#define IDS_RESET_DATASETTE_WITH_CPU_DE   0x7a3
#define IDS_RESET_DATASETTE_WITH_CPU_FR   0x7a4
#define IDS_RESET_DATASETTE_WITH_CPU_IT   0x7a5
#define IDS_RESET_DATASETTE_WITH_CPU_NL   0x7a6
#define IDS_RESET_DATASETTE_WITH_CPU_PL   0x7a7
#define IDS_RESET_DATASETTE_WITH_CPU_SV   0x7a8

#define IDS_ADDITIONAL_DELAY              0x7a9
#define IDS_ADDITIONAL_DELAY_DE           0x7aa
#define IDS_ADDITIONAL_DELAY_FR           0x7ab
#define IDS_ADDITIONAL_DELAY_IT           0x7ac
#define IDS_ADDITIONAL_DELAY_NL           0x7ad
#define IDS_ADDITIONAL_DELAY_PL           0x7ae
#define IDS_ADDITIONAL_DELAY_SV           0x7af

#define IDS_DELAY_AT_ZERO_VALUES          0x7b0
#define IDS_DELAY_AT_ZERO_VALUES_DE       0x7b1
#define IDS_DELAY_AT_ZERO_VALUES_FR       0x7b2
#define IDS_DELAY_AT_ZERO_VALUES_IT       0x7b3
#define IDS_DELAY_AT_ZERO_VALUES_NL       0x7b4
#define IDS_DELAY_AT_ZERO_VALUES_PL       0x7b5
#define IDS_DELAY_AT_ZERO_VALUES_SV       0x7b6

#define IDS_DATASETTE_SETTINGS            0x7b7
#define IDS_DATASETTE_SETTINGS_DE         0x7b8
#define IDS_DATASETTE_SETTINGS_FR         0x7b9
#define IDS_DATASETTE_SETTINGS_IT         0x7ba
#define IDS_DATASETTE_SETTINGS_NL         0x7bb
#define IDS_DATASETTE_SETTINGS_PL         0x7bc
#define IDS_DATASETTE_SETTINGS_SV         0x7bd

#define IDS_NEVER_EXTEND                  0x7be
#define IDS_NEVER_EXTEND_DE               0x7bf
#define IDS_NEVER_EXTEND_FR               0x7c0
#define IDS_NEVER_EXTEND_IT               0x7c1
#define IDS_NEVER_EXTEND_NL               0x7c2
#define IDS_NEVER_EXTEND_PL               0x7c3
#define IDS_NEVER_EXTEND_SV               0x7c4

#define IDS_ASK_ON_EXTEND                 0x7c5
#define IDS_ASK_ON_EXTEND_DE              0x7c6
#define IDS_ASK_ON_EXTEND_FR              0x7c7
#define IDS_ASK_ON_EXTEND_IT              0x7c8
#define IDS_ASK_ON_EXTEND_NL              0x7c9
#define IDS_ASK_ON_EXTEND_PL              0x7ca
#define IDS_ASK_ON_EXTEND_SV              0x7cb

#define IDS_EXTEND_ON_ACCESS              0x7cc
#define IDS_EXTEND_ON_ACCESS_DE           0x7cd
#define IDS_EXTEND_ON_ACCESS_FR           0x7ce
#define IDS_EXTEND_ON_ACCESS_IT           0x7cf
#define IDS_EXTEND_ON_ACCESS_NL           0x7d0
#define IDS_EXTEND_ON_ACCESS_PL           0x7d1
#define IDS_EXTEND_ON_ACCESS_SV           0x7d2

#define IDS_NO_TRAPS                      0x7d3
#define IDS_NO_TRAPS_DE                   0x7d4
#define IDS_NO_TRAPS_FR                   0x7d5
#define IDS_NO_TRAPS_IT                   0x7d6
#define IDS_NO_TRAPS_NL                   0x7d7
#define IDS_NO_TRAPS_PL                   0x7d8
#define IDS_NO_TRAPS_SV                   0x7d9

#define IDS_SKIP_CYCLES                   0x7da
#define IDS_SKIP_CYCLES_DE                0x7db
#define IDS_SKIP_CYCLES_FR                0x7dc
#define IDS_SKIP_CYCLES_IT                0x7dd
#define IDS_SKIP_CYCLES_NL                0x7de
#define IDS_SKIP_CYCLES_PL                0x7df
#define IDS_SKIP_CYCLES_SV                0x7e0

#define IDS_TRAP_IDLE                     0x7e1
#define IDS_TRAP_IDLE_DE                  0x7e2
#define IDS_TRAP_IDLE_FR                  0x7e3
#define IDS_TRAP_IDLE_IT                  0x7e4
#define IDS_TRAP_IDLE_NL                  0x7e5
#define IDS_TRAP_IDLE_PL                  0x7e6
#define IDS_TRAP_IDLE_SV                  0x7e7

#define IDS_DRIVE_TYPE                    0x7e8
#define IDS_DRIVE_TYPE_DE                 0x7e9
#define IDS_DRIVE_TYPE_FR                 0x7ea
#define IDS_DRIVE_TYPE_IT                 0x7eb
#define IDS_DRIVE_TYPE_NL                 0x7ec
#define IDS_DRIVE_TYPE_PL                 0x7ed
#define IDS_DRIVE_TYPE_SV                 0x7ee

#define IDS_40_TRACK_HANDLING             0x7ef
#define IDS_40_TRACK_HANDLING_DE          0x7f0
#define IDS_40_TRACK_HANDLING_FR          0x7f1
#define IDS_40_TRACK_HANDLING_IT          0x7f2
#define IDS_40_TRACK_HANDLING_NL          0x7f3
#define IDS_40_TRACK_HANDLING_PL          0x7f4
#define IDS_40_TRACK_HANDLING_SV          0x7f5

#define IDS_DRIVE_EXPANSION               0x7f6
#define IDS_DRIVE_EXPANSION_DE            0x7f7
#define IDS_DRIVE_EXPANSION_FR            0x7f8
#define IDS_DRIVE_EXPANSION_IT            0x7f9
#define IDS_DRIVE_EXPANSION_NL            0x7fa
#define IDS_DRIVE_EXPANSION_PL            0x7fb
#define IDS_DRIVE_EXPANSION_SV            0x7fc

#define IDS_IDLE_METHOD                   0x7fd
#define IDS_IDLE_METHOD_DE                0x7fe
#define IDS_IDLE_METHOD_FR                0x7ff
#define IDS_IDLE_METHOD_IT                0x800
#define IDS_IDLE_METHOD_NL                0x801
#define IDS_IDLE_METHOD_PL                0x802
#define IDS_IDLE_METHOD_SV                0x803

#define IDS_PARALLEL_CABLE                0x804
#define IDS_PARALLEL_CABLE_DE             0x805
#define IDS_PARALLEL_CABLE_FR             0x806
#define IDS_PARALLEL_CABLE_IT             0x807
#define IDS_PARALLEL_CABLE_NL             0x808
#define IDS_PARALLEL_CABLE_PL             0x809
#define IDS_PARALLEL_CABLE_SV             0x80a

#define IDS_DRIVE_SETTINGS                0x80b
#define IDS_DRIVE_SETTINGS_DE             0x80c
#define IDS_DRIVE_SETTINGS_FR             0x80d
#define IDS_DRIVE_SETTINGS_IT             0x80e
#define IDS_DRIVE_SETTINGS_NL             0x80f
#define IDS_DRIVE_SETTINGS_PL             0x810
#define IDS_DRIVE_SETTINGS_SV             0x811

#define IDS_GEORAM_SIZE                   0x812
#define IDS_GEORAM_SIZE_DE                0x813
#define IDS_GEORAM_SIZE_FR                0x814
#define IDS_GEORAM_SIZE_IT                0x815
#define IDS_GEORAM_SIZE_NL                0x816
#define IDS_GEORAM_SIZE_PL                0x817
#define IDS_GEORAM_SIZE_SV                0x818

#define IDS_GEORAM_SETTINGS               0x819
#define IDS_GEORAM_SETTINGS_DE            0x81a
#define IDS_GEORAM_SETTINGS_FR            0x81b
#define IDS_GEORAM_SETTINGS_IT            0x81c
#define IDS_GEORAM_SETTINGS_NL            0x81d
#define IDS_GEORAM_SETTINGS_PL            0x81e
#define IDS_GEORAM_SETTINGS_SV            0x81f

#define IDS_JOYSTICK_IN_PORT_1            0x820
#define IDS_JOYSTICK_IN_PORT_1_DE         0x821
#define IDS_JOYSTICK_IN_PORT_1_FR         0x822
#define IDS_JOYSTICK_IN_PORT_1_IT         0x823
#define IDS_JOYSTICK_IN_PORT_1_NL         0x824
#define IDS_JOYSTICK_IN_PORT_1_PL         0x825
#define IDS_JOYSTICK_IN_PORT_1_SV         0x826

#define IDS_JOYSTICK_IN_PORT_2            0x827
#define IDS_JOYSTICK_IN_PORT_2_DE         0x828
#define IDS_JOYSTICK_IN_PORT_2_FR         0x829
#define IDS_JOYSTICK_IN_PORT_2_IT         0x82a
#define IDS_JOYSTICK_IN_PORT_2_NL         0x82b
#define IDS_JOYSTICK_IN_PORT_2_PL         0x82c
#define IDS_JOYSTICK_IN_PORT_2_SV         0x82d

#define IDS_JOYSTICK_SETTINGS             0x82e
#define IDS_JOYSTICK_SETTINGS_DE          0x82f
#define IDS_JOYSTICK_SETTINGS_FR          0x830
#define IDS_JOYSTICK_SETTINGS_IT          0x831
#define IDS_JOYSTICK_SETTINGS_NL          0x832
#define IDS_JOYSTICK_SETTINGS_PL          0x833
#define IDS_JOYSTICK_SETTINGS_SV          0x834

#define IDS_KEYPAD                        0x835
#define IDS_KEYPAD_DE                     0x836
#define IDS_KEYPAD_FR                     0x837
#define IDS_KEYPAD_IT                     0x838
#define IDS_KEYPAD_NL                     0x839
#define IDS_KEYPAD_PL                     0x83a
#define IDS_KEYPAD_SV                     0x83b

#define IDS_JOY_PORT_0                    0x83c
#define IDS_JOY_PORT_0_DE                 0x83d
#define IDS_JOY_PORT_0_FR                 0x83e
#define IDS_JOY_PORT_0_IT                 0x83f
#define IDS_JOY_PORT_0_NL                 0x840
#define IDS_JOY_PORT_0_PL                 0x841
#define IDS_JOY_PORT_0_SV                 0x842

#define IDS_JOY_PORT_1                    0x843
#define IDS_JOY_PORT_1_DE                 0x844
#define IDS_JOY_PORT_1_FR                 0x845
#define IDS_JOY_PORT_1_IT                 0x846
#define IDS_JOY_PORT_1_NL                 0x847
#define IDS_JOY_PORT_1_PL                 0x848
#define IDS_JOY_PORT_1_SV                 0x849

#define IDS_JOY_PORT_2                    0x84a
#define IDS_JOY_PORT_2_DE                 0x84b
#define IDS_JOY_PORT_2_FR                 0x84c
#define IDS_JOY_PORT_2_IT                 0x84d
#define IDS_JOY_PORT_2_NL                 0x84e
#define IDS_JOY_PORT_2_PL                 0x84f
#define IDS_JOY_PORT_2_SV                 0x850

#define IDS_JOY_PORT_3                    0x851
#define IDS_JOY_PORT_3_DE                 0x852
#define IDS_JOY_PORT_3_FR                 0x853
#define IDS_JOY_PORT_3_IT                 0x854
#define IDS_JOY_PORT_3_NL                 0x855
#define IDS_JOY_PORT_3_PL                 0x856
#define IDS_JOY_PORT_3_SV                 0x857

#define IDS_STOP_BLUE                     0x858
#define IDS_STOP_BLUE_DE                  0x859
#define IDS_STOP_BLUE_FR                  0x85a
#define IDS_STOP_BLUE_IT                  0x85b
#define IDS_STOP_BLUE_NL                  0x85c
#define IDS_STOP_BLUE_PL                  0x85d
#define IDS_STOP_BLUE_SV                  0x85e

#define IDS_SELECT_RED                    0x85f
#define IDS_SELECT_RED_DE                 0x860
#define IDS_SELECT_RED_FR                 0x861
#define IDS_SELECT_RED_IT                 0x862
#define IDS_SELECT_RED_NL                 0x863
#define IDS_SELECT_RED_PL                 0x864
#define IDS_SELECT_RED_SV                 0x865

#define IDS_REPEAT_YELLOW                 0x866
#define IDS_REPEAT_YELLOW_DE              0x867
#define IDS_REPEAT_YELLOW_FR              0x868
#define IDS_REPEAT_YELLOW_IT              0x869
#define IDS_REPEAT_YELLOW_NL              0x86a
#define IDS_REPEAT_YELLOW_PL              0x86b
#define IDS_REPEAT_YELLOW_SV              0x86c

#define IDS_SHUFFLE_GREEN                 0x86d
#define IDS_SHUFFLE_GREEN_DE              0x86e
#define IDS_SHUFFLE_GREEN_FR              0x86f
#define IDS_SHUFFLE_GREEN_IT              0x870
#define IDS_SHUFFLE_GREEN_NL              0x871
#define IDS_SHUFFLE_GREEN_PL              0x872
#define IDS_SHUFFLE_GREEN_SV              0x873

#define IDS_FORWARD_CHARCOAL              0x874
#define IDS_FORWARD_CHARCOAL_DE           0x875
#define IDS_FORWARD_CHARCOAL_FR           0x876
#define IDS_FORWARD_CHARCOAL_IT           0x877
#define IDS_FORWARD_CHARCOAL_NL           0x878
#define IDS_FORWARD_CHARCOAL_PL           0x879
#define IDS_FORWARD_CHARCOAL_SV           0x87a

#define IDS_REVERSE_CHARCOAL              0x87b
#define IDS_REVERSE_CHARCOAL_DE           0x87c
#define IDS_REVERSE_CHARCOAL_FR           0x87d
#define IDS_REVERSE_CHARCOAL_IT           0x87e
#define IDS_REVERSE_CHARCOAL_NL           0x87f
#define IDS_REVERSE_CHARCOAL_PL           0x880
#define IDS_REVERSE_CHARCOAL_SV           0x881

#define IDS_PLAY_PAUSE_GREY               0x882
#define IDS_PLAY_PAUSE_GREY_DE            0x883
#define IDS_PLAY_PAUSE_GREY_FR            0x884
#define IDS_PLAY_PAUSE_GREY_IT            0x885
#define IDS_PLAY_PAUSE_GREY_NL            0x886
#define IDS_PLAY_PAUSE_GREY_PL            0x887
#define IDS_PLAY_PAUSE_GREY_SV            0x888

#define IDS_JOY_1_DEVICE                  0x889
#define IDS_JOY_1_DEVICE_DE               0x88a
#define IDS_JOY_1_DEVICE_FR               0x88b
#define IDS_JOY_1_DEVICE_IT               0x88c
#define IDS_JOY_1_DEVICE_NL               0x88d
#define IDS_JOY_1_DEVICE_PL               0x88e
#define IDS_JOY_1_DEVICE_SV               0x88f

#define IDS_JOY_2_DEVICE                  0x890
#define IDS_JOY_2_DEVICE_DE               0x891
#define IDS_JOY_2_DEVICE_FR               0x892
#define IDS_JOY_2_DEVICE_IT               0x893
#define IDS_JOY_2_DEVICE_NL               0x894
#define IDS_JOY_2_DEVICE_PL               0x895
#define IDS_JOY_2_DEVICE_SV               0x896

#define IDS_JOY_1_FIRE                    0x897
#define IDS_JOY_1_FIRE_DE                 0x898
#define IDS_JOY_1_FIRE_FR                 0x899
#define IDS_JOY_1_FIRE_IT                 0x89a
#define IDS_JOY_1_FIRE_NL                 0x89b
#define IDS_JOY_1_FIRE_PL                 0x89c
#define IDS_JOY_1_FIRE_SV                 0x89d

#define IDS_JOY_2_FIRE                    0x89e
#define IDS_JOY_2_FIRE_DE                 0x89f
#define IDS_JOY_2_FIRE_FR                 0x8a0
#define IDS_JOY_2_FIRE_IT                 0x8a1
#define IDS_JOY_2_FIRE_NL                 0x8a2
#define IDS_JOY_2_FIRE_PL                 0x8a3
#define IDS_JOY_2_FIRE_SV                 0x8a4

#define IDS_SET_INPUT_JOYSTICK_1          0x8b3
#define IDS_SET_INPUT_JOYSTICK_1_DE       0x8b4
#define IDS_SET_INPUT_JOYSTICK_1_FR       0x8b5
#define IDS_SET_INPUT_JOYSTICK_1_IT       0x8b6
#define IDS_SET_INPUT_JOYSTICK_1_NL       0x8b7
#define IDS_SET_INPUT_JOYSTICK_1_PL       0x8b8
#define IDS_SET_INPUT_JOYSTICK_1_SV       0x8b9

#define IDS_SET_INPUT_JOYSTICK_2          0x8ba
#define IDS_SET_INPUT_JOYSTICK_2_DE       0x8bb
#define IDS_SET_INPUT_JOYSTICK_2_FR       0x8bc
#define IDS_SET_INPUT_JOYSTICK_2_IT       0x8bd
#define IDS_SET_INPUT_JOYSTICK_2_NL       0x8be
#define IDS_SET_INPUT_JOYSTICK_2_PL       0x8bf
#define IDS_SET_INPUT_JOYSTICK_2_SV       0x8c0

#define IDS_SET_INPUT_JOYLL_1             0x8c1
#define IDS_SET_INPUT_JOYLL_1_DE          0x8c2
#define IDS_SET_INPUT_JOYLL_1_FR          0x8c3
#define IDS_SET_INPUT_JOYLL_1_IT          0x8c4
#define IDS_SET_INPUT_JOYLL_1_NL          0x8c5
#define IDS_SET_INPUT_JOYLL_1_PL          0x8c6
#define IDS_SET_INPUT_JOYLL_1_SV          0x8c7

#define IDS_SET_INPUT_JOYLL_2             0x8c8
#define IDS_SET_INPUT_JOYLL_2_DE          0x8c9
#define IDS_SET_INPUT_JOYLL_2_FR          0x8ca
#define IDS_SET_INPUT_JOYLL_2_IT          0x8cb
#define IDS_SET_INPUT_JOYLL_2_NL          0x8cc
#define IDS_SET_INPUT_JOYLL_2_PL          0x8cd
#define IDS_SET_INPUT_JOYLL_2_SV          0x8ce

#define IDS_SAVE_SETTINGS_ON_EXIT         0x8cf
#define IDS_SAVE_SETTINGS_ON_EXIT_DE      0x8d0
#define IDS_SAVE_SETTINGS_ON_EXIT_FR      0x8d1
#define IDS_SAVE_SETTINGS_ON_EXIT_IT      0x8d2
#define IDS_SAVE_SETTINGS_ON_EXIT_NL      0x8d3
#define IDS_SAVE_SETTINGS_ON_EXIT_PL      0x8d4
#define IDS_SAVE_SETTINGS_ON_EXIT_SV      0x8d5

#define IDS_NEVER_SAVE_SETTINGS_EXIT      0x8d6
#define IDS_NEVER_SAVE_SETTINGS_EXIT_DE   0x8d7
#define IDS_NEVER_SAVE_SETTINGS_EXIT_FR   0x8d8
#define IDS_NEVER_SAVE_SETTINGS_EXIT_IT   0x8d9
#define IDS_NEVER_SAVE_SETTINGS_EXIT_NL   0x8da
#define IDS_NEVER_SAVE_SETTINGS_EXIT_PL   0x8db
#define IDS_NEVER_SAVE_SETTINGS_EXIT_SV   0x8dc

#define IDS_CONFIRM_QUITING_VICE          0x8dd
#define IDS_CONFIRM_QUITING_VICE_DE       0x8de
#define IDS_CONFIRM_QUITING_VICE_FR       0x8df
#define IDS_CONFIRM_QUITING_VICE_IT       0x8e0
#define IDS_CONFIRM_QUITING_VICE_NL       0x8e1
#define IDS_CONFIRM_QUITING_VICE_PL       0x8e2
#define IDS_CONFIRM_QUITING_VICE_SV       0x8e3

#define IDS_NEVER_CONFIRM_QUITING_VICE    0x8e4
#define IDS_NEVER_CONFIRM_QUITING_VICE_DE 0x8e5
#define IDS_NEVER_CONFIRM_QUITING_VICE_FR 0x8e6
#define IDS_NEVER_CONFIRM_QUITING_VICE_IT 0x8e7
#define IDS_NEVER_CONFIRM_QUITING_VICE_NL 0x8e8
#define IDS_NEVER_CONFIRM_QUITING_VICE_PL 0x8e9
#define IDS_NEVER_CONFIRM_QUITING_VICE_SV 0x8ea

#define IDS_PET_REU_SIZE                  0x8eb
#define IDS_PET_REU_SIZE_DE               0x8ec
#define IDS_PET_REU_SIZE_FR               0x8ed
#define IDS_PET_REU_SIZE_IT               0x8ee
#define IDS_PET_REU_SIZE_NL               0x8ef
#define IDS_PET_REU_SIZE_PL               0x8f0
#define IDS_PET_REU_SIZE_SV               0x8f1

#define IDS_PET_REU_SETTINGS              0x8f2
#define IDS_PET_REU_SETTINGS_DE           0x8f3
#define IDS_PET_REU_SETTINGS_FR           0x8f4
#define IDS_PET_REU_SETTINGS_IT           0x8f5
#define IDS_PET_REU_SETTINGS_NL           0x8f6
#define IDS_PET_REU_SETTINGS_PL           0x8f7
#define IDS_PET_REU_SETTINGS_SV           0x8f8

#define IDS_PLUS256K_SETTINGS             0x8f9
#define IDS_PLUS256K_SETTINGS_DE          0x8fa
#define IDS_PLUS256K_SETTINGS_FR          0x8fb
#define IDS_PLUS256K_SETTINGS_IT          0x8fc
#define IDS_PLUS256K_SETTINGS_NL          0x8fd
#define IDS_PLUS256K_SETTINGS_PL          0x8fe
#define IDS_PLUS256K_SETTINGS_SV          0x8ff

#define IDS_PLUS60K_BASE                  0x900
#define IDS_PLUS60K_BASE_DE               0x901
#define IDS_PLUS60K_BASE_FR               0x902
#define IDS_PLUS60K_BASE_IT               0x903
#define IDS_PLUS60K_BASE_NL               0x904
#define IDS_PLUS60K_BASE_PL               0x905
#define IDS_PLUS60K_BASE_SV               0x906

#define IDS_PLUS60K_SETTINGS              0x907
#define IDS_PLUS60K_SETTINGS_DE           0x908
#define IDS_PLUS60K_SETTINGS_FR           0x909
#define IDS_PLUS60K_SETTINGS_IT           0x90a
#define IDS_PLUS60K_SETTINGS_NL           0x90b
#define IDS_PLUS60K_SETTINGS_PL           0x90c
#define IDS_PLUS60K_SETTINGS_SV           0x90d

#define IDS_VALUE_FIRST_BYTE              0x90e
#define IDS_VALUE_FIRST_BYTE_DE           0x90f
#define IDS_VALUE_FIRST_BYTE_FR           0x910
#define IDS_VALUE_FIRST_BYTE_IT           0x911
#define IDS_VALUE_FIRST_BYTE_NL           0x912
#define IDS_VALUE_FIRST_BYTE_PL           0x913
#define IDS_VALUE_FIRST_BYTE_SV           0x914

#define IDS_LENGTH_CONSTANT_VALUES        0x915
#define IDS_LENGTH_CONSTANT_VALUES_DE     0x916
#define IDS_LENGTH_CONSTANT_VALUES_FR     0x917
#define IDS_LENGTH_CONSTANT_VALUES_IT     0x918
#define IDS_LENGTH_CONSTANT_VALUES_NL     0x919
#define IDS_LENGTH_CONSTANT_VALUES_PL     0x91a
#define IDS_LENGTH_CONSTANT_VALUES_SV     0x91b

#define IDS_LENGTH_CONSTANT_PATTERN       0x91c
#define IDS_LENGTH_CONSTANT_PATTERN_DE    0x91d
#define IDS_LENGTH_CONSTANT_PATTERN_FR    0x91e
#define IDS_LENGTH_CONSTANT_PATTERN_IT    0x91f
#define IDS_LENGTH_CONSTANT_PATTERN_NL    0x920
#define IDS_LENGTH_CONSTANT_PATTERN_PL    0x921
#define IDS_LENGTH_CONSTANT_PATTERN_SV    0x922

#define IDS_RAM_SETTINGS                  0x923
#define IDS_RAM_SETTINGS_DE               0x924
#define IDS_RAM_SETTINGS_FR               0x925
#define IDS_RAM_SETTINGS_IT               0x926
#define IDS_RAM_SETTINGS_NL               0x927
#define IDS_RAM_SETTINGS_PL               0x928
#define IDS_RAM_SETTINGS_SV               0x929

#define IDS_READ_WRITE                    0x92a
#define IDS_READ_WRITE_DE                 0x92b
#define IDS_READ_WRITE_FR                 0x92c
#define IDS_READ_WRITE_IT                 0x92d
#define IDS_READ_WRITE_NL                 0x92e
#define IDS_READ_WRITE_PL                 0x92f
#define IDS_READ_WRITE_SV                 0x930

#define IDS_READ_ONLY                     0x931
#define IDS_READ_ONLY_DE                  0x932
#define IDS_READ_ONLY_FR                  0x933
#define IDS_READ_ONLY_IT                  0x934
#define IDS_READ_ONLY_NL                  0x935
#define IDS_READ_ONLY_PL                  0x936
#define IDS_READ_ONLY_SV                  0x937

#define IDS_RAMCART_READ_WRITE            0x938
#define IDS_RAMCART_READ_WRITE_DE         0x939
#define IDS_RAMCART_READ_WRITE_FR         0x93a
#define IDS_RAMCART_READ_WRITE_IT         0x93b
#define IDS_RAMCART_READ_WRITE_NL         0x93c
#define IDS_RAMCART_READ_WRITE_PL         0x93d
#define IDS_RAMCART_READ_WRITE_SV         0x93e

#define IDS_RAMCART_SIZE                  0x93f
#define IDS_RAMCART_SIZE_DE               0x940
#define IDS_RAMCART_SIZE_FR               0x941
#define IDS_RAMCART_SIZE_IT               0x942
#define IDS_RAMCART_SIZE_NL               0x943
#define IDS_RAMCART_SIZE_PL               0x944
#define IDS_RAMCART_SIZE_SV               0x945

#define IDS_RAMCART_SETTINGS              0x946
#define IDS_RAMCART_SETTINGS_DE           0x947
#define IDS_RAMCART_SETTINGS_FR           0x948
#define IDS_RAMCART_SETTINGS_IT           0x949
#define IDS_RAMCART_SETTINGS_NL           0x94a
#define IDS_RAMCART_SETTINGS_PL           0x94b
#define IDS_RAMCART_SETTINGS_SV           0x94c

#define IDS_REU_SIZE                      0x94d
#define IDS_REU_SIZE_DE                   0x94e
#define IDS_REU_SIZE_FR                   0x94f
#define IDS_REU_SIZE_IT                   0x950
#define IDS_REU_SIZE_NL                   0x951
#define IDS_REU_SIZE_PL                   0x952
#define IDS_REU_SIZE_SV                   0x953

#define IDS_REU_SETTINGS                  0x954
#define IDS_REU_SETTINGS_DE               0x955
#define IDS_REU_SETTINGS_FR               0x956
#define IDS_REU_SETTINGS_IT               0x957
#define IDS_REU_SETTINGS_NL               0x958
#define IDS_REU_SETTINGS_PL               0x959
#define IDS_REU_SETTINGS_SV               0x95a

#define IDS_USERPORT_RS232                0x95b
#define IDS_USERPORT_RS232_DE             0x95c
#define IDS_USERPORT_RS232_FR             0x95d
#define IDS_USERPORT_RS232_IT             0x95e
#define IDS_USERPORT_RS232_NL             0x95f
#define IDS_USERPORT_RS232_PL             0x960
#define IDS_USERPORT_RS232_SV             0x961

#define IDS_USERPORT_DEVICE               0x962
#define IDS_USERPORT_DEVICE_DE            0x963
#define IDS_USERPORT_DEVICE_FR            0x964
#define IDS_USERPORT_DEVICE_IT            0x965
#define IDS_USERPORT_DEVICE_NL            0x966
#define IDS_USERPORT_DEVICE_PL            0x967
#define IDS_USERPORT_DEVICE_SV            0x968

#define IDS_USERPORT_BAUD_RATE            0x969
#define IDS_USERPORT_BAUD_RATE_DE         0x96a
#define IDS_USERPORT_BAUD_RATE_FR         0x96b
#define IDS_USERPORT_BAUD_RATE_IT         0x96c
#define IDS_USERPORT_BAUD_RATE_NL         0x96d
#define IDS_USERPORT_BAUD_RATE_PL         0x96e
#define IDS_USERPORT_BAUD_RATE_SV         0x96f

#define IDS_RS232_USERPORT_SETTINGS       0x970
#define IDS_RS232_USERPORT_SETTINGS_DE    0x971
#define IDS_RS232_USERPORT_SETTINGS_FR    0x972
#define IDS_RS232_USERPORT_SETTINGS_IT    0x973
#define IDS_RS232_USERPORT_SETTINGS_NL    0x974
#define IDS_RS232_USERPORT_SETTINGS_PL    0x975
#define IDS_RS232_USERPORT_SETTINGS_SV    0x976

#define IDS_FAST                          0x977
#define IDS_FAST_DE                       0x978
#define IDS_FAST_FR                       0x979
#define IDS_FAST_IT                       0x97a
#define IDS_FAST_NL                       0x97b
#define IDS_FAST_PL                       0x97c
#define IDS_FAST_SV                       0x97d

#define IDS_INTERPOLATING                 0x97e
#define IDS_INTERPOLATING_DE              0x97f
#define IDS_INTERPOLATING_FR              0x980
#define IDS_INTERPOLATING_IT              0x981
#define IDS_INTERPOLATING_NL              0x982
#define IDS_INTERPOLATING_PL              0x983
#define IDS_INTERPOLATING_SV              0x984

#define IDS_RESAMPLING                    0x985
#define IDS_RESAMPLING_DE                 0x986
#define IDS_RESAMPLING_FR                 0x987
#define IDS_RESAMPLING_IT                 0x988
#define IDS_RESAMPLING_NL                 0x989
#define IDS_RESAMPLING_PL                 0x98a
#define IDS_RESAMPLING_SV                 0x98b

#define IDS_FAST_RESAMPLING               0x98c
#define IDS_FAST_RESAMPLING_DE            0x98d
#define IDS_FAST_RESAMPLING_FR            0x98e
#define IDS_FAST_RESAMPLING_IT            0x98f
#define IDS_FAST_RESAMPLING_NL            0x990
#define IDS_FAST_RESAMPLING_PL            0x991
#define IDS_FAST_RESAMPLING_SV            0x992

#define IDS_6581_OLD                      0x993
#define IDS_6581_OLD_DE                   0x994
#define IDS_6581_OLD_FR                   0x995
#define IDS_6581_OLD_IT                   0x996
#define IDS_6581_OLD_NL                   0x997
#define IDS_6581_OLD_PL                   0x998
#define IDS_6581_OLD_SV                   0x999

#define IDS_8580_NEW                      0x99a
#define IDS_8580_NEW_DE                   0x99b
#define IDS_8580_NEW_FR                   0x99c
#define IDS_8580_NEW_IT                   0x99d
#define IDS_8580_NEW_NL                   0x99e
#define IDS_8580_NEW_PL                   0x99f
#define IDS_8580_NEW_SV                   0x9a0

#define IDS_SID_MODEL                     0x9a1
#define IDS_SID_MODEL_DE                  0x9a2
#define IDS_SID_MODEL_FR                  0x9a3
#define IDS_SID_MODEL_IT                  0x9a4
#define IDS_SID_MODEL_NL                  0x9a5
#define IDS_SID_MODEL_PL                  0x9a6
#define IDS_SID_MODEL_SV                  0x9a7

#define IDS_SID_STEREO                    0x9a8
#define IDS_SID_STEREO_DE                 0x9a9
#define IDS_SID_STEREO_FR                 0x9aa
#define IDS_SID_STEREO_IT                 0x9ab
#define IDS_SID_STEREO_NL                 0x9ac
#define IDS_SID_STEREO_PL                 0x9ad
#define IDS_SID_STEREO_SV                 0x9ae

#define IDS_STEREO_SID_AT                 0x9af
#define IDS_STEREO_SID_AT_DE              0x9b0
#define IDS_STEREO_SID_AT_FR              0x9b1
#define IDS_STEREO_SID_AT_IT              0x9b2
#define IDS_STEREO_SID_AT_NL              0x9b3
#define IDS_STEREO_SID_AT_PL              0x9b4
#define IDS_STEREO_SID_AT_SV              0x9b5

#define IDS_SID_FILTERS                   0x9b6
#define IDS_SID_FILTERS_DE                0x9b7
#define IDS_SID_FILTERS_FR                0x9b8
#define IDS_SID_FILTERS_IT                0x9b9
#define IDS_SID_FILTERS_NL                0x9ba
#define IDS_SID_FILTERS_PL                0x9bb
#define IDS_SID_FILTERS_SV                0x9bc

#define IDS_SAMPLE_METHOD                 0x9bd
#define IDS_SAMPLE_METHOD_DE              0x9be
#define IDS_SAMPLE_METHOD_FR              0x9bf
#define IDS_SAMPLE_METHOD_IT              0x9c0
#define IDS_SAMPLE_METHOD_NL              0x9c1
#define IDS_SAMPLE_METHOD_PL              0x9c2
#define IDS_SAMPLE_METHOD_SV              0x9c3

#define IDS_PASSBAND_0_90                 0x9c4
#define IDS_PASSBAND_0_90_DE              0x9c5
#define IDS_PASSBAND_0_90_FR              0x9c6
#define IDS_PASSBAND_0_90_IT              0x9c7
#define IDS_PASSBAND_0_90_NL              0x9c8
#define IDS_PASSBAND_0_90_PL              0x9c9
#define IDS_PASSBAND_0_90_SV              0x9ca

#define IDS_NOT_IMPLEMENTED_YET           0x9cb
#define IDS_NOT_IMPLEMENTED_YET_DE        0x9cc
#define IDS_NOT_IMPLEMENTED_YET_FR        0x9cd
#define IDS_NOT_IMPLEMENTED_YET_IT        0x9ce
#define IDS_NOT_IMPLEMENTED_YET_NL        0x9cf
#define IDS_NOT_IMPLEMENTED_YET_PL        0x9d0
#define IDS_NOT_IMPLEMENTED_YET_SV        0x9d1

#define IDS_SID_SETTINGS                  0x9d2
#define IDS_SID_SETTINGS_DE               0x9d3
#define IDS_SID_SETTINGS_FR               0x9d4
#define IDS_SID_SETTINGS_IT               0x9d5
#define IDS_SID_SETTINGS_NL               0x9d6
#define IDS_SID_SETTINGS_PL               0x9d7
#define IDS_SID_SETTINGS_SV               0x9d8

#define IDS_FLEXIBLE                      0x9d9
#define IDS_FLEXIBLE_DE                   0x9da
#define IDS_FLEXIBLE_FR                   0x9db
#define IDS_FLEXIBLE_IT                   0x9dc
#define IDS_FLEXIBLE_NL                   0x9dd
#define IDS_FLEXIBLE_PL                   0x9de
#define IDS_FLEXIBLE_SV                   0x9df

#define IDS_ADJUSTING                     0x9e0
#define IDS_ADJUSTING_DE                  0x9e1
#define IDS_ADJUSTING_FR                  0x9e2
#define IDS_ADJUSTING_IT                  0x9e3
#define IDS_ADJUSTING_NL                  0x9e4
#define IDS_ADJUSTING_PL                  0x9e5
#define IDS_ADJUSTING_SV                  0x9e6

#define IDS_EXACT                         0x9e7
#define IDS_EXACT_DE                      0x9e8
#define IDS_EXACT_FR                      0x9e9
#define IDS_EXACT_IT                      0x9ea
#define IDS_EXACT_NL                      0x9eb
#define IDS_EXACT_PL                      0x9ec
#define IDS_EXACT_SV                      0x9ed

#define IDS_SAMPLE_RATE                   0x9ee
#define IDS_SAMPLE_RATE_DE                0x9ef
#define IDS_SAMPLE_RATE_FR                0x9f0
#define IDS_SAMPLE_RATE_IT                0x9f1
#define IDS_SAMPLE_RATE_NL                0x9f2
#define IDS_SAMPLE_RATE_PL                0x9f3
#define IDS_SAMPLE_RATE_SV                0x9f4

#define IDS_BUFFER_SIZE                   0x9f5
#define IDS_BUFFER_SIZE_DE                0x9f6
#define IDS_BUFFER_SIZE_FR                0x9f7
#define IDS_BUFFER_SIZE_IT                0x9f8
#define IDS_BUFFER_SIZE_NL                0x9f9
#define IDS_BUFFER_SIZE_PL                0x9fa
#define IDS_BUFFER_SIZE_SV                0x9fb

#define IDS_OVERSAMPLE                    0x9fc
#define IDS_OVERSAMPLE_DE                 0x9fd
#define IDS_OVERSAMPLE_FR                 0x9fe
#define IDS_OVERSAMPLE_IT                 0x9ff
#define IDS_OVERSAMPLE_NL                 0xa00
#define IDS_OVERSAMPLE_PL                 0xa01
#define IDS_OVERSAMPLE_SV                 0xa02

#define IDS_SPEED_ADJUSTMENT              0xa03
#define IDS_SPEED_ADJUSTMENT_DE           0xa04
#define IDS_SPEED_ADJUSTMENT_FR           0xa05
#define IDS_SPEED_ADJUSTMENT_IT           0xa06
#define IDS_SPEED_ADJUSTMENT_NL           0xa07
#define IDS_SPEED_ADJUSTMENT_PL           0xa08
#define IDS_SPEED_ADJUSTMENT_SV           0xa09

#define IDS_SOUND_SETTINGS                0xa0a
#define IDS_SOUND_SETTINGS_DE             0xa0b
#define IDS_SOUND_SETTINGS_FR             0xa0c
#define IDS_SOUND_SETTINGS_IT             0xa0d
#define IDS_SOUND_SETTINGS_NL             0xa0e
#define IDS_SOUND_SETTINGS_PL             0xa0f
#define IDS_SOUND_SETTINGS_SV             0xa10

#define IDS_RAM_BLOCK_0400_0FFF           0xa11
#define IDS_RAM_BLOCK_0400_0FFF_DE        0xa12
#define IDS_RAM_BLOCK_0400_0FFF_FR        0xa13
#define IDS_RAM_BLOCK_0400_0FFF_IT        0xa14
#define IDS_RAM_BLOCK_0400_0FFF_NL        0xa15
#define IDS_RAM_BLOCK_0400_0FFF_PL        0xa16
#define IDS_RAM_BLOCK_0400_0FFF_SV        0xa17

#define IDS_RAM_BLOCK_A000_BFFF           0xa18
#define IDS_RAM_BLOCK_A000_BFFF_DE        0xa19
#define IDS_RAM_BLOCK_A000_BFFF_FR        0xa1a
#define IDS_RAM_BLOCK_A000_BFFF_IT        0xa1b
#define IDS_RAM_BLOCK_A000_BFFF_NL        0xa1c
#define IDS_RAM_BLOCK_A000_BFFF_PL        0xa1d
#define IDS_RAM_BLOCK_A000_BFFF_SV        0xa1e

#define IDS_VIC_SETTINGS                  0xa1f
#define IDS_VIC_SETTINGS_DE               0xa20
#define IDS_VIC_SETTINGS_FR               0xa21
#define IDS_VIC_SETTINGS_IT               0xa22
#define IDS_VIC_SETTINGS_NL               0xa23
#define IDS_VIC_SETTINGS_PL               0xa24
#define IDS_VIC_SETTINGS_SV               0xa25

#define IDS_SPRITE_SPRITE_COL             0xa26
#define IDS_SPRITE_SPRITE_COL_DE          0xa27
#define IDS_SPRITE_SPRITE_COL_FR          0xa28
#define IDS_SPRITE_SPRITE_COL_IT          0xa29
#define IDS_SPRITE_SPRITE_COL_NL          0xa2a
#define IDS_SPRITE_SPRITE_COL_PL          0xa2b
#define IDS_SPRITE_SPRITE_COL_SV          0xa2c

#define IDS_SPRITE_BACKGROUND_COL         0xa2d
#define IDS_SPRITE_BACKGROUND_COL_DE      0xa2e
#define IDS_SPRITE_BACKGROUND_COL_FR      0xa2f
#define IDS_SPRITE_BACKGROUND_COL_IT      0xa30
#define IDS_SPRITE_BACKGROUND_COL_NL      0xa31
#define IDS_SPRITE_BACKGROUND_COL_PL      0xa32
#define IDS_SPRITE_BACKGROUND_COL_SV      0xa33

#define IDS_NEW_LUMINANCES                0xa34
#define IDS_NEW_LUMINANCES_DE             0xa35
#define IDS_NEW_LUMINANCES_FR             0xa36
#define IDS_NEW_LUMINANCES_IT             0xa37
#define IDS_NEW_LUMINANCES_NL             0xa38
#define IDS_NEW_LUMINANCES_PL             0xa39
#define IDS_NEW_LUMINANCES_SV             0xa3a

#define IDS_VICII_SETTINGS                0xa3b
#define IDS_VICII_SETTINGS_DE             0xa3c
#define IDS_VICII_SETTINGS_FR             0xa3d
#define IDS_VICII_SETTINGS_IT             0xa3e
#define IDS_VICII_SETTINGS_NL             0xa3f
#define IDS_VICII_SETTINGS_PL             0xa40
#define IDS_VICII_SETTINGS_SV             0xa41

#define IDS_ATTACH_TAPE_IMAGE             0xa42
#define IDS_ATTACH_TAPE_IMAGE_DE          0xa43
#define IDS_ATTACH_TAPE_IMAGE_FR          0xa44
#define IDS_ATTACH_TAPE_IMAGE_IT          0xa45
#define IDS_ATTACH_TAPE_IMAGE_NL          0xa46
#define IDS_ATTACH_TAPE_IMAGE_PL          0xa47
#define IDS_ATTACH_TAPE_IMAGE_SV          0xa48

#define IDS_AUTOSTART_IMAGE               0xa49
#define IDS_AUTOSTART_IMAGE_DE            0xa4a
#define IDS_AUTOSTART_IMAGE_FR            0xa4b
#define IDS_AUTOSTART_IMAGE_IT            0xa4c
#define IDS_AUTOSTART_IMAGE_NL            0xa4d
#define IDS_AUTOSTART_IMAGE_PL            0xa4e
#define IDS_AUTOSTART_IMAGE_SV            0xa4f

#endif
