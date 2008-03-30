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
extern char *intl_translate_text_new(int en_resource);

extern void intl_init(void);
extern void intl_shutdown(void);
extern char *intl_arch_language_init(void);
extern void intl_update_ui(void);
extern char *intl_convert_cp(char *text, int cp);


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

/* ----------------------- AmigaOS Strings ----------------------- */

#define IDS_PRESS_KEY_BUTTON              0x595
#define IDS_PRESS_KEY_BUTTON_DE           0x596
#define IDS_PRESS_KEY_BUTTON_FR           0x597
#define IDS_PRESS_KEY_BUTTON_IT           0x598
#define IDS_PRESS_KEY_BUTTON_NL           0x599
#define IDS_PRESS_KEY_BUTTON_PL           0x59a
#define IDS_PRESS_KEY_BUTTON_SV           0x59b



#endif
