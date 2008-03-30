/*
 * res.h
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _RES_H
#define _RES_H

#define IDR_MENUC64                     101
#define IDR_MENUC128                    102
#define IDR_MENUVIC                     103
#define IDR_MENUPET                     104
#define IDR_MENUPLUS4                   105
#define IDR_MENUCBM2                    106
#define IDR_MENUMONITOR                 107
#define IDR_MONTOOLBAR                  108
#define IDI_ICON1                       109

#define IDD_ABOUT                       110
#define IDC_ABOUT_VERSION               1002
#define IDC_BROWSEDISK                  1006
#define IDC_SELECTDISK                  1007
#define IDC_SELECTNONE                  1008
#define IDC_SELECTDIR                   1009
#define IDC_BROWSEDIR                   1010
#define IDD_DISKDEVICE_DIALOG           1010
#define IDC_AUTOSTART                   1011
#define IDC_DISKIMAGE                   1012
#define IDC_DIR                         1013
#define IDC_TOGGLE_READP00              1014
#define IDC_TOGGLE_WRITEP00             1015
#define IDC_TOGGLE_HIDENONP00           1016
#define IDC_TOGGLE_ATTACH_READONLY      1017
#define IDD_DRIVE_SETTINGS_DIALOG       1020
#define IDC_SELECT_DRIVE_TYPE_1541      1021
#define IDC_SELECT_DRIVE_TYPE_1541II    1022
#define IDC_SELECT_DRIVE_TYPE_1571      1023
#define IDC_SELECT_DRIVE_TYPE_1581      1024
#define IDC_SELECT_DRIVE_TYPE_2031      1025
#define IDC_SELECT_DRIVE_TYPE_1001      1026
#define IDC_SELECT_DRIVE_TYPE_8050      1027
#define IDC_SELECT_DRIVE_TYPE_8250      1028
#define IDC_SELECT_DRIVE_TYPE_NONE      1029
#define IDD_DRIVE_EXTEND_DIALOG         1030
#define IDC_SELECT_DRIVE_EXTEND_NEVER   1031
#define IDC_SELECT_DRIVE_EXTEND_ASK     1032
#define IDC_SELECT_DRIVE_EXTEND_ACCESS  1033
#define IDD_DRIVE_IDLE_METHOD           1040
#define IDC_SELECT_DRIVE_IDLE_NO_IDLE   1041
#define IDC_SELECT_DRIVE_IDLE_TRAP_IDLE 1042
#define IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES 1043
#define IDD_DRIVE_SYNC_FACTOR           1050
#define IDC_SELECT_DRIVE_SYNC_PAL       1051
#define IDC_SELECT_DRIVE_SYNC_NTSC      1052
#define IDC_SELECT_DRIVE_SYNC_NTSCOLD   1053
#define IDC_TOGGLE_DRIVE_PARALLEL_CABLE 1060
#define IDD_SNAPSHOT_SAVE_DIALOG        1070
#define IDC_SNAPSHOT_SAVE_IMAGE         1071
#define IDC_TOGGLE_SNAPSHOT_SAVE_DISKS  1072
#define IDC_TOGGLE_SNAPSHOT_SAVE_ROMS   1073
#define IDD_SCREENSHOT_SAVE_DIALOG      1074
#define IDC_SCREENSHOT_DRIVER           1075
#define IDD_VICII_DIALOG                1080
#define IDC_TOGGLE_VICII_SSC            1081
#define IDC_TOGGLE_VICII_SBC            1082
#define IDC_TOGGLE_VICII_NEWLUM         1083
#define IDD_PET_SETTINGS_MODEL_DIALOG   1095
#define IDD_PET_SETTINGS_IO_DIALOG      1096
#define IDD_PET_SETTINGS_SUPER_DIALOG   1097
#define IDD_PET_SETTINGS_8296_DIALOG    1098
#define IDC_SELECT_PET_2001_8N          1100
#define IDC_SELECT_PET_3008             1101
#define IDC_SELECT_PET_3016             1102
#define IDC_SELECT_PET_3032             1103
#define IDC_SELECT_PET_3032B            1104
#define IDC_SELECT_PET_4016             1105
#define IDC_SELECT_PET_4032             1106
#define IDC_SELECT_PET_4032B            1107
#define IDC_SELECT_PET_8032             1108
#define IDC_SELECT_PET_8096             1109
#define IDC_SELECT_PET_8296             1110
#define IDC_SELECT_PET_SUPER            1111
#define IDC_SELECT_PET_MEM4K            1120
#define IDC_SELECT_PET_MEM8K            1121
#define IDC_SELECT_PET_MEM16K           1122
#define IDC_SELECT_PET_MEM32K           1123
#define IDC_SELECT_PET_MEM96K           1124
#define IDC_SELECT_PET_MEM128K          1125
#define IDC_SELECT_PET_IO2K             1128
#define IDC_SELECT_PET_IO256            1129
#define IDC_SELECT_PET_VIDEO_AUTO       1130
#define IDC_SELECT_PET_VIDEO_40         1131
#define IDC_SELECT_PET_VIDEO_80         1132
#define IDC_SELECT_PET_KEYB_GRAPHICS    1135
#define IDC_SELECT_PET_KEYB_BUSINESS    1136
#define IDC_TOGGLE_PET_CRTC             1140
#define IDC_TOGGLE_PET_SUPER_IO_ENABLE  1141
#define IDC_TOGGLE_PET_8296_RAM9        1142
#define IDC_TOGGLE_PET_8296_RAMA        1143
#define IDD_CBMII_SETTINGS_MODEL_DIALOG 1148
#define IDD_CBMII_SETTINGS_IO_DIALOG    1149
#define IDC_SELECT_CBMII_610            1150
#define IDC_SELECT_CBMII_620            1151
#define IDC_SELECT_CBMII_620P           1152
#define IDC_SELECT_CBMII_710            1153
#define IDC_SELECT_CBMII_720            1154
#define IDC_SELECT_CBMII_720P           1155
#define IDC_SELECT_CBMII_MEM_128        1160
#define IDC_SELECT_CBMII_MEM_256        1161
#define IDC_SELECT_CBMII_MEM_512        1162
#define IDC_SELECT_CBMII_MEM_1024       1163
#define IDC_SELECT_CBMII_HW0            1165
#define IDC_SELECT_CBMII_HW1            1166
#define IDC_SELECT_CBMII_HW2            1167
#define IDC_TOGGLE_CBMII_RAM08          1170
#define IDC_TOGGLE_CBMII_RAM1           1171
#define IDC_TOGGLE_CBMII_RAM2           1172
#define IDC_TOGGLE_CBMII_RAM4           1173
#define IDC_TOGGLE_CBMII_RAM6           1174
#define IDC_TOGGLE_CBMII_RAMC           1175
#define IDC_SELECT_CBMII_KEYB_GRAPHICS  1180
#define IDC_SELECT_CBMII_KEYB_BUSINESS  1181
#define IDC_TOGGLE_DRIVE_EXPANSION_2000 1190
#define IDC_TOGGLE_DRIVE_EXPANSION_4000 1191
#define IDC_TOGGLE_DRIVE_EXPANSION_6000 1192
#define IDC_TOGGLE_DRIVE_EXPANSION_8000 1193
#define IDC_TOGGLE_DRIVE_EXPANSION_A000 1194

#define IDD_JOY_SETTINGS_DIALOG         1200
#define IDC_JOY_DEV1                    1201
#define IDC_JOY_DEV2                    1202
#define IDC_JOY_CALIBRATE               1203
#define IDC_JOY_CONFIG_A                1204
#define IDC_JOY_CONFIG_B                1205
#define IDC_JOY_FIRE1_SPEED             1206
#define IDC_JOY_FIRE2_SPEED             1207
#define IDC_JOY_FIRE1_AXIS              1208
#define IDC_JOY_FIRE2_AXIS              1209

#define IDD_CONFIG_KEYSET_DIALOG        1210
#define IDC_KEYSET_SW                   1211
#define IDC_KEYSET_S                    1212
#define IDC_KEYSET_SE                   1213
#define IDC_KEYSET_W                    1214
#define IDC_KEYSET_E                    1215
#define IDC_KEYSET_NW                   1216
#define IDC_KEYSET_N                    1217
#define IDC_KEYSET_NE                   1218
#define IDC_KEYSET_FIRE                 1219
#define IDC_KEY_SW                      1220
#define IDC_KEY_S                       1221
#define IDC_KEY_SE                      1222
#define IDC_KEY_W                       1223
#define IDC_KEY_E                       1224
#define IDC_KEY_NW                      1225
#define IDC_KEY_N                       1226
#define IDC_KEY_NE                      1227
#define IDC_KEY_FIRE                    1228

#define IDD_CONFIG_KEY_DIALOG           1229

#define IDD_SOUND_SETTINGS_DIALOG       1240
#define IDC_SOUND_FREQ                  1241
#define IDC_SOUND_BUFFER                1242
#define IDC_SOUND_OVERSAMPLE            1243
#define IDC_SOUND_SYNCH                 1244
#define IDC_SOUND_DIRECTX               1245
#define IDC_SOUND_WMM                   1246
#define IDC_SOUND_SIDSTEREO             1247

#define IDD_OPEN_TEMPLATE               1250
#define IDD_OPENTAPE_TEMPLATE           1251
#define IDC_PREVIEW                     1252
#define IDC_BLANK_IMAGE                 1253
#define IDC_BLANK_IMAGE_TYPE            1254
#define IDC_BLANK_IMAGE_NAME            1255
#define IDC_BLANK_IMAGE_ID              1256

#define IDD_DATASETTE_SETTINGS_DIALOG   1260
#define IDC_DATASETTE_RESET_WITH_CPU    1261
#define IDC_DATASETTE_SPEED_TUNING      1262
#define IDC_DATASETTE_ZERO_GAP_DELAY    1263

#define IDD_VIC_SETTINGS_DIALOG         1270
#define IDC_VIC_NOEXPANSION             1271
#define IDC_VIC_3KEXPANSION             1272
#define IDC_VIC_8KEXPANSION             1273
#define IDC_VIC_16KEXPANSION            1274
#define IDC_VIC_24KEXPANSION            1275
#define IDC_VIC_FULLEXPANSION           1276
#define IDC_VIC_CUSTOMEXPANSION         1277
#define IDC_VIC_MEMORY_BLOCK0           1278
#define IDC_VIC_MEMORY_BLOCK1           1279
#define IDC_VIC_MEMORY_BLOCK2           1280
#define IDC_VIC_MEMORY_BLOCK3           1281
#define IDC_VIC_MEMORY_BLOCK5           1282

#define IDD_PRINTDEVICE_DIALOG          1290
#define IDC_TOGGLE_PRINTER              1291

#define IDD_DIALOG1                     1301
#define IDD_TEXTDLG                     1301
#define IDC_TEXT                        1302
#define IDC_HEADER                      1303

#define IDD_CONSOLE_SAVE_DIALOG         1310
#define IDC_TOGGLE_CONSOLE_APPEND       1311

#define IDD_FULLSCREEN_SETTINGS_DIALOG  1320
#define IDC_FULLSCREEN_DEVICE           1321
#define IDC_FULLSCREEN_RESOLUTION       1322
#define IDC_FULLSCREEN_BITDEPTH         1323
#define IDC_FULLSCREEN_REFRESHRATE      1324

#define IDD_REU_SETTINGS_DIALOG         1330
#define IDC_REU_ENABLE                  1331
#define IDC_REU_SIZE                    1332
#define IDC_REU_FILE                    1333
#define IDC_REU_BROWSE                  1334

#define IDD_SID_SETTINGS_DIALOG         1340
#define IDC_SID_FILTERS                 1341
#define IDC_SID_6581                    1342
#define IDC_SID_8580                    1343
#define IDC_SID_STEREO                  1344
#define IDC_SID_RESID                   1345
#define IDC_SID_RESID_SAMPLING          1346
#define IDC_SID_RESID_PASSBAND          1347
#define IDC_SID_STEREOADDRESS           1348

#define IDD_VIDEO_COLORS_DIALOG         1400
#define IDC_VIDEO_COLORS_SAT            1401
#define IDC_VIDEO_COLORS_CON            1402
#define IDC_VIDEO_COLORS_BRI            1403
#define IDD_VIDEO_FULLSCREEN_DIALOG     1404
#define IDC_VIDEO_FULLSCREEN_DEVICE     1405
#define IDC_VIDEO_FULLSCREEN_BITDEPTH   1406
#define IDC_VIDEO_FULLSCREEN_RESOLUTION 1407
#define IDC_VIDEO_FULLSCREEN_REFRESHRATE 1408
#define IDD_VIDEO_ADVANCED_DIALOG       1409
#define IDC_VIDEO_COLORS_GAM            1410
#define IDC_VIDEO_COLORS_PHA            1411

#define IDC_TOGGLE_VIDEO_EXTPALETTE     1412
#define IDC_VIDEO_CUSTOM_NAME           1413
#define IDC_VIDEO_CUSTOM_BROWSE         1414
#define IDC_SELECT_VIDEO_CUSTOM         1415
#define IDC_VIDEO_ADVANCED_SHADE		1416
#define IDC_VIDEO_ADVANCED_MODE			1417

#define IDD_PLUS4_MEMORY_DIALOG		1420
#define IDC_SELECT_PLUS4_MEM_16		1421
#define IDC_SELECT_PLUS4_MEM_32		1422
#define IDC_SELECT_PLUS4_MEM_64		1423

#define IDM_FILE_EXIT                   40001
#define IDM_EXIT                        40001
#define IDM_ABOUT                       40002
#define IDM_PAUSE                       40003
#define IDM_AUTOSTART                   40004
#define IDM_HARD_RESET                  40005
#define IDM_SOFT_RESET                  40006
#define IDM_ATTACH_8                    40007
#define IDM_ATTACH_9                    40008
#define IDM_ATTACH_10                   40009
#define IDM_ATTACH_11                   40010
#define IDM_ATTACH_TAPE                 40011
#define IDM_DETACH_TAPE                 40012
#define IDM_DATASETTE_CONTROL           40013
#define IDM_DETACH_8                    40014
#define IDM_DETACH_9                    40015
#define IDM_DETACH_10                   40016
#define IDM_DETACH_11                   40017
#define IDM_DETACH_ALL                  40018
#define IDM_TOGGLE_SOUND                40019
#define IDM_TOGGLE_DOUBLESIZE           40020
#define IDM_TOGGLE_DOUBLESCAN           40021
#define IDM_TOGGLE_DRIVE_TRUE_EMULATION 40022
#define IDM_TOGGLE_VIDEOCACHE           40023
#define IDM_DRIVE_SETTINGS              40030
#define IDM_CART_ATTACH_CRT             40040
#define IDM_CART_ATTACH_8KB             40041
#define IDM_CART_ATTACH_16KB            40042
#define IDM_CART_ATTACH_AR              40043
#define IDM_CART_ATTACH_AT              40044
#define IDM_CART_ATTACH_EPYX            40045
#define IDM_CART_ATTACH_IEEE488         40046
#define IDM_CART_ATTACH_RR              40047
#define IDM_CART_ATTACH_SS4             40048
#define IDM_CART_ATTACH_SS5             40049
#define IDM_CART_ATTACH_IDE64           40050
#define IDM_CART_SET_DEFAULT            40058
#define IDM_CART_DETACH                 40059
#define IDM_CART_VIC20_8KB_2000         40060
#define IDM_CART_VIC20_8KB_6000         40061
#define IDM_CART_VIC20_8KB_A000         40062
#define IDM_CART_VIC20_4KB_B000         40063
#define IDM_CART_VIC20_16KB             40064
#define IDM_CART_VIC20_16KB_4000        40065
#define IDM_SNAPSHOT_LOAD               40080
#define IDM_SNAPSHOT_SAVE               40081
#define IDM_SCREENSHOT                  40082
#define IDM_SOUNDSHOT                   40083
#define IDM_SETTINGS_SAVE               40090
#define IDM_SETTINGS_LOAD               40091
#define IDM_SETTINGS_DEFAULT            40092
#define IDM_DEVICEMANAGER               40093
#define IDM_REFRESH_RATE_AUTO           40100
#define IDM_REFRESH_RATE_1              40101
#define IDM_REFRESH_RATE_2              40102
#define IDM_REFRESH_RATE_3              40103
#define IDM_REFRESH_RATE_4              40104
#define IDM_REFRESH_RATE_5              40105
#define IDM_REFRESH_RATE_6              40106
#define IDM_REFRESH_RATE_7              40107
#define IDM_REFRESH_RATE_8              40108
#define IDM_REFRESH_RATE_9              40109
#define IDM_REFRESH_RATE_10             40110
#define IDM_REFRESH_RATE_CUSTOM         40111
#define IDM_MAXIMUM_SPEED_200           40120
#define IDM_MAXIMUM_SPEED_100           40121
#define IDM_MAXIMUM_SPEED_50            40122
#define IDM_MAXIMUM_SPEED_20            40123
#define IDM_MAXIMUM_SPEED_10            40124
#define IDM_MAXIMUM_SPEED_NO_LIMIT      40125
#define IDM_MAXIMUM_SPEED_CUSTOM        40126
#define IDM_TOGGLE_WARP_MODE            40129
#define IDM_VICII_SETTINGS              40130
#define IDM_PET_SETTINGS                40131
#define IDM_CBM2_SETTINGS               40132
#define IDM_SYNC_FACTOR_PAL             40140
#define IDM_SYNC_FACTOR_NTSC            40141
#define IDM_SYNC_FACTOR_NTSCOLD         40142
#define IDM_JOY_SETTINGS                40143
#define IDM_SOUND_SETTINGS              40144
#define IDM_STATUS_WINDOW               40145
#define IDM_MONITOR                     40146
#define IDM_CART_FREEZE                 40149
#define IDM_SAVEQUICK                   40150
#define IDM_LOADQUICK                   40151
#define IDM_HELP                        40152
#define IDM_DATASETTE_CONTROL_START     40160
#define IDM_DATASETTE_CONTROL_STOP      40161
#define IDM_DATASETTE_CONTROL_FORWARD   40162
#define IDM_DATASETTE_CONTROL_REWIND    40163
#define IDM_DATASETTE_CONTROL_RECORD    40164
#define IDM_DATASETTE_CONTROL_RESET     40165
#define IDM_DATASETTE_RESET_COUNTER     40166
#define IDM_FLIP_ADD                    40170
#define IDM_FLIP_REMOVE                 40171
#define IDM_FLIP_NEXT                   40172
#define IDM_FLIP_PREVIOUS               40173
#define IDM_REU_SETTINGS                40180
#define IDM_TOGGLE_CRTCDOUBLESIZE       40181
#define IDM_TOGGLE_CRTCDOUBLESCAN       40182
#define IDM_TOGGLE_CRTCVIDEOCACHE       40183
#define IDM_SWAP_JOYSTICK               40184
#define IDM_TOGGLE_EMUID                40185
#define IDM_IEEE488                     40186
#define IDM_MOUSE                       40187
#define IDM_DATASETTE_SETTINGS          40188
#define IDM_TOGGLE_VDC_64KB             40189
#define IDM_TOGGLE_VDC_DOUBLESIZE       40190
#define IDM_TOGGLE_VDC_DOUBLESCAN       40191
#define IDM_VIC_SETTINGS                40192
#define IDM_TOGGLE_VIRTUAL_DEVICES      40193
#define IDM_CONTRIBUTORS                40194
#define IDM_LICENSE                     40195
#define IDM_WARRANTY                    40196
#define IDM_CMDLINE                     40197
#define IDM_FULLSCREEN_SETTINGS         40198
#define IDM_TOGGLE_SAVE_SETTINGS_ON_EXIT 40199
#define IDM_TOGGLE_FULLSCREEN           40200
#define IDM_TOGGLE_CONFIRM_ON_EXIT      40201
#define IDM_SID_SETTINGS                40202
#define IDM_TOGGLE_IFUNCTIONROM         40205
#define IDM_IFUNCTIONROM_NAME           40206
#define IDM_TOGGLE_EFUNCTIONROM         40207
#define IDM_EFUNCTIONROM_NAME           40208
#define IDM_TOGGLE_FASTPAL              40209
#define IDM_VIDEO_SETTINGS              40210
#define IDM_PLUS4_SETTINGS		40211

#define IDM_MON_OPEN                    42000
#define IDM_MON_SAVE                    42001
#define IDM_MON_PRINT                   42002
#define IDM_MON_STOP_DEBUG              42003
#define IDM_MON_STOP_EXEC               42004
#define IDM_MON_CURRENT                 42005
#define IDM_MON_STEP_INTO               42006
#define IDM_MON_STEP_OVER               42007
#define IDM_MON_SKIP_RETURN             42008
#define IDM_MON_GOTO_CURSOR             42009
#define IDM_MON_EVAL                    42010
#define IDM_MON_WND_EVAL                42011
#define IDM_MON_WND_REG                 42012
#define IDM_MON_WND_MEM                 42013
#define IDM_MON_WND_DIS                 42014
#define IDM_MON_WND_CONSOLE             42015
#define IDM_MON_COMPUTER                42016
#define IDM_MON_DRIVE8                  42017
#define IDM_MON_DRIVE9                  42018
#define IDM_MON_HELP                    42019
#define IDM_MON_CASCADE                 42020
#define IDM_MON_TILE_HORIZ              42021
#define IDM_MON_TILE_VERT               42022
#define IDM_MON_ARRANGE_ICONS           42023
#define IDM_MON_GOTO_PC                 42024
#define IDM_MON_GOTO_ADDRESS            42025
#define IDM_MON_SET_BP                  42026
#define IDM_MON_UNSET_BP                42027
#define IDM_MON_ENABLE_BP               42028
#define IDM_MON_DISABLE_BP              42029
#define IDM_WNDCHILD                    42100

#define WM_CONSOLE_INSERTLINE           (WM_USER)
#define WM_CONSOLE_CLOSED               (WM_USER+1)
#define WM_CONSOLE_RESIZED              (WM_USER+2)
#define WM_CONSOLE_ACTIVATED            (WM_USER+3)

#endif

