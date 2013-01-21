/*
 *  Copyright (C) 2011 Locnet (android.locnet@gmail.com)
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include "loader.h"
#if 0
#include "sysconfig.h"
#include "sysdeps.h"
#include "uae.h"
#include "options.h"
#include "savestate.h"

int pause_program = 0;
int mainMenu_showStatus;
int mainMenu_ntsc = 0;
int mainMenu_height = 240;
//int mainMenu_mouseMoveHack = 0;

extern int mainMenu_drives;
void notice_screen_contents_lost (void);
#endif

extern "C" {
#include "machine.h"
#include "ui.h"
#include "autostart.h"
#include "attach.h"
#include "mouse.h"
#include "resources.h"
#include "joy.h"
#include "datasette.h"

int loader_get_drive_true_emulation();

}

char autostart_filename[256]="";
char execute_file[256]="/sdcard/x64";
char execute_path[256]="/sdcard/";
char savestate_filename[256];
int loader_loadstate = 0;
int loader_savestate = 0;
int loader_showinfo = 0;
int loader_turbo = 0;
int loader_true_drive = 0;
int loader_ntsc = 0;
int loader_border = 0;
//int loader_joyport = 1;

#define main SDL_main
//extern "C" int main(int argc, const char* argv[]);

extern struct loader_config myLoader;

//#define LOADER_PACKAGE_NAME   com_locnet_uae
//#define CLASS_NAME    DosBoxLauncher
//#define JAVA_EXPORT_NAME(package_name, class_name, method_name)       Java_##package_name##_##class_name##_##method_name

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeStart(JNIEnv * env, jobject obj, jobject bitmap, jint width, jint height)
{
        Android_Init(env, obj, bitmap, width, height);
        int i = 0;
        const char * argv[11];

        argv[i++] = execute_file;
        argv[i++] = "-chdir";
        argv[i++] = execute_path;
        argv[i++] = "-config";
        argv[i++] = "/sdcard/sdl-vicerc";
        if (loader_true_drive) {
            if (loader_true_drive == -1) {
                argv[i++] = "+truedrive";
            } else {
                argv[i++] = "-truedrive";
            }
        }
        if (loader_ntsc) {
            argv[i++] = "-ntsc";
        }
        if (loader_border == 1) {
            argv[i++] = "-sdllimitmode";
            argv[i++] = "0";
        }
        argv[i++] = "-autostart";
        argv[i++] = autostart_filename;

        loader_true_drive = 0;

        main(i, (char **)argv);
        
        __android_log_print(ANDROID_LOG_INFO, LOCNET_LOG_TAG, "before Android_ShutDown()");
        Android_ShutDown();
}

void setDisk(JNIEnv * env, jint id, jstring filename, jint change_disk)
{
        if (filename && ((env)->GetStringLength(filename) > 0))
        {
                const char *sfloppy = (env)->GetStringUTFChars(filename, 0);

                if (change_disk) {
                        if (file_system_attach_disk(id+8, sfloppy) < 0) {
                //ui_error("could not start auto-image");
            }
                        //__android_log_print(ANDROID_LOG_INFO, "UAE", "change floppy1: %s", changed_df[0]);
                } else {
            //if (autostart_autodetect(sfloppy, NULL, 0, AUTOSTART_MODE_RUN) < 0) {
                //ui_error("Cannot attach disk image.");
            //}
                        if (id == 0)
                                strcpy(autostart_filename, sfloppy);
                }
                (env)->ReleaseStringUTFChars(filename, sfloppy);

        } else {
                if (change_disk) {
                        file_system_detach_disk(id+8);
                }
                else {
                        file_system_detach_disk(id+8);
                }
        }
        //__android_log_print(ANDROID_LOG_INFO, "UAE", "floppy%d: %s", id, prefs_df[id]);
}

extern "C" jint Java_com_locnet_vice_DosBoxLauncher_nativeSetOption(JNIEnv * env, jobject obj, jint option, jint value, jobject value2)
{
        jint    result = 0;

        switch (option) {
#if 0
                case 1:
                        myLoader.soundEnable = value;
                        enableSound = (value != 0);
                        break;
                case 2:
                        myLoader.memsize = value;
                        break;
                case 10:
                        //myLoader.cycles = value;
                        //CPU_CycleMax = value;
                        //CPU_SkipCycleAutoAdjust = false;
                        //CPU_CycleAutoAdjust = false;
                        m68k_speed = value;
                        check_prefs_changed_cpu();
                        check_prefs_changed_audio();
                        break;
#endif
                case 11:
                        //render.frameskip.max = value;
                        myLoader.frameskip = (value > 0)?value:-1;
                        break;
                /*case 12:
                        myLoader.refreshHack = value;
                        enableRefreshHack = (value != 0);
                        break;*/
                /*case 13:
                        myLoader.cycleHack = value;
                        enableCycleHack = (value != 0);
                        break;*/
                case 14:
                        loader_turbo = (value)?1:-1;
                        break;
#if 0
                case 15:
                        //JOYSTICK_Enable(0, (value != 0));
                        break;
                case 16:
                        changed_produce_sound = (value != 0)?2:0;
                        check_prefs_changed_audio();
                        break;
#endif
                case 17:
                        loader_showinfo = (value)?1:-1;
                        break;
                case 51:
                        if (value2) {
                                const char *srom = (env)->GetStringUTFChars((jstring)value2, 0);
                                strcpy(execute_path, srom);
                                strcpy(execute_file, srom);
                                strcat(execute_file, "x64");
                                (env)->ReleaseStringUTFChars((jstring)value2, srom);
                        }
                        break;
                case 52:
                        setDisk(env, abs(value)-1, (jstring)value2, (value>0)?1:0);
                        break;
                case 53:
                        loader_ntsc = value;
                        break;
#if 0
                case 54:
                        mainMenu_drives = value;
                        break;
                case 55:
                        mainMenu_height = value;
                        break;
#endif
                case 56:
                        if (value)
                                loader_true_drive = value;
                        else
                                result = loader_get_drive_true_emulation();
                        break;
                case 57:
                        sdljoy_swap_ports();
                        break;
                case 58:
                        loader_border = value;
                        break;
                case 59:
                        //loader_datasette_control = value;
                        datasette_control(value);
                        break;
        }

        return result;
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeInit(JNIEnv * env, jobject obj)
{
        loadf = 0;
        myLoader.memsize = 2;
        myLoader.bmph = 0;
        myLoader.videoBuffer = 0;

        myLoader.abort = 0;
        myLoader.pause = 0;

        myLoader.frameskip = 0;
        myLoader.cycles = 1500;
        myLoader.soundEnable = 1;
        myLoader.cycleHack = 1;
        myLoader.refreshHack = 1;
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativePause(JNIEnv * env, jobject obj, jint state)
{
        if ((state == 0) || (state == 1))
                myLoader.pause = state;
        else
                myLoader.pause = (myLoader.pause)?0:1;
        ui_pause_emulation(myLoader.pause);
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeStop(JNIEnv * env, jobject obj)
{
        myLoader.abort = 1;
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeReset(JNIEnv * env, jobject obj)
{
        //uae_reset();
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeShutDown(JNIEnv * env, jobject obj)
{
        myLoader.bmph = 0;
        myLoader.videoBuffer = 0;
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeSaveState(JNIEnv * env, jobject obj, jstring filename, jint num)
{
        const char *srom = (env)->GetStringUTFChars(filename, 0);
        strcpy(savestate_filename, srom);

        switch(num)
        {
                case 1:
                        strcat(savestate_filename,"-1.vsf");
                        break;
                case 2:
                        strcat(savestate_filename,"-2.vsf");
                        break;
                case 3:
                        strcat(savestate_filename,"-3.vsf");
                        break;
                default:
                        strcat(savestate_filename,".vsf");
                        break;
        }

        (env)->ReleaseStringUTFChars(filename, srom);

        ui_pause_emulation(1);
        loader_savestate = 1;
        //__android_log_print(ANDROID_LOG_INFO, LOCNET_LOG_TAG, "Saved %s", savestate_filename);
}

extern "C" void Java_com_locnet_vice_DosBoxLauncher_nativeLoadState(JNIEnv * env, jobject obj, jstring filename, jint num)
{
        const char *srom = (env)->GetStringUTFChars(filename, 0);
        strcpy(savestate_filename, srom);

        switch(num)
        {
                case 1:
                        strcat(savestate_filename,"-1.vsf");
                        break;
                case 2:
                        strcat(savestate_filename,"-2.vsf");
                        break;
                case 3:
                        strcat(savestate_filename,"-3.vsf");
                        break;
                default:
                        strcat(savestate_filename,".vsf");
                        break;
        }

        (env)->ReleaseStringUTFChars(filename, srom);

        ui_pause_emulation(1);
        loader_loadstate = 1;
        //__android_log_print(ANDROID_LOG_INFO, LOCNET_LOG_TAG, "Loaded %s", savestate_filename);
}
