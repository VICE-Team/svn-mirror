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

#ifndef ANDROID_OS_FUNC
#define ANDROID_OS_FUNC

#include <jni.h>
#include <android/log.h>

#define LOCNET_LOG_TAG	"c64"

#define DEAD_ZONE	0.3

typedef struct locnet_al_event {
	int eventType;
	int keycode;
	int unicode;
	int modifier;
	float x;
	float y;
	float down_x;
	float down_y;
} locnet_al_event_struct;

void Android_Init(JNIEnv * env, jobject obj, jobject bitmap, jint width, jint height);
void Android_ShutDown();
void Android_SetVideoMode(int width, int height, int depth);
void Android_LockSurface();
void Android_UnlockSurface(int startLine, int endLine);
void Android_ResetScreen();
void Android_AudioGetBuffer();
int Android_OpenAudio(int rate, int channels, int encoding, int bufSize);

#ifdef __cplusplus
extern "C" {
#endif
void Android_AudioWriteBuffer();
int Android_PollEvent(locnet_al_event_struct *event);
int Android_HasRepeatEvent(int eventType, int value);
#ifdef __cplusplus
}
#endif


#endif
