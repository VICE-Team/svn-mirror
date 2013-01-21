/*
 *  Copyright (C) 2011 Locnet  (android.locnet@gmail.com)
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

#include <time.h>
#include <malloc.h>
#include <sys/select.h>
#include <android/log.h>
#include "loader.h"

//extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...) {return 0;}

/*void MSCDEX_SetCDInterface(int intNr, int forceCD) {}
int  MSCDEX_AddDrive(char driveLetter, const char* physicalPath, Bit8u& subUnit){return 0;}
bool MSCDEX_HasMediaChanged(Bit8u subUnit){return false;}
bool MSCDEX_GetVolumeName(Bit8u subUnit, char* name){return false;}
int  MSCDEX_RemoveDrive(char driveLetter){return 1;}

isoDrive::isoDrive(char driveLetter, const char *fileName, Bit8u mediaid, int &error) {
	nextFreeDirIterator = 0;
	memset(dirIterators, 0, sizeof(dirIterators));
	memset(sectorHashEntries, 0, sizeof(sectorHashEntries));
	memset(&rootEntry, 0, sizeof(isoDirEntry));
	
	safe_strncpy(this->fileName, fileName, CROSS_LEN);

	strcpy(info, "isoDrive ");
	strcat(info, fileName);
	this->driveLetter = driveLetter;
	this->mediaid = mediaid;
	char buffer[32] = { 0 };
	strcpy(buffer, "Audio_CD");
	Set_Label(buffer,discLabel,true);
	
	error = 2;
}
isoDrive::~isoDrive() {}
 bool isoDrive::FileOpen(DOS_File **file, char *name, Bit32u flags){return false;}
 bool isoDrive::FileCreate(DOS_File **file, char *name, Bit16u attributes){return false;}
 bool isoDrive::FileUnlink(char *name){return false;}
 bool isoDrive::RemoveDir(char *dir){return false;}
 bool isoDrive::MakeDir(char *dir){return false;}
 bool isoDrive::TestDir(char *dir){return false;}
 bool isoDrive::FindFirst(char *_dir, DOS_DTA &dta, bool fcb_findfirst){return false;}
 bool isoDrive::FindNext(DOS_DTA &dta){return false;}
 bool isoDrive::GetFileAttr(char *name, Bit16u *attr){return false;}
 bool isoDrive::Rename(char * oldname,char * newname){return false;}
 bool isoDrive::AllocationInfo(Bit16u *bytes_sector, Bit8u *sectors_cluster, Bit16u *total_clusters, Bit16u *free_clusters){return false;}
 bool isoDrive::FileExists(const char *name){return false;}
 bool isoDrive::FileStat(const char *name, FileStat_Block *const stat_block){return false;}
 Bit8u isoDrive::GetMediaByte(void){return mediaid;}
 bool isoDrive::isRemote(void){return true;}
 bool isoDrive::isRemovable(void){return true;}
Bits isoDrive::UnMount(void) {return 0;}
void isoDrive::Activate(void) {}*/

#include "SDL.h"
#include "SDL_thread.h"

//SDL_AudioCallback mixerCallBack = NULL;
void (SDLCALL *mixerCallBack)(void *userdata, Uint8 *stream, int len) = NULL;

#if 1

struct SDL_mutex {
	int recursive;
	Uint32 owner;
	SDL_sem *sem;
};

SDL_Surface	sdl_surface;
SDL_PixelFormat sdl_format;

//C64
extern DECLSPEC Uint32 SDLCALL SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b){
	//__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_MapRGB: %d %d %d", r, g, b);
	return ((r<<8)&0xF800)|((g<<3)&0x07E0)|((b>>3)&0x1F);
	/*if (format->palette != NULL)
		__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_MapRGB");
	return (r >> format->Rloss) << format->Rshift
			       | (g >> format->Gloss) << format->Gshift
			       | (b >> format->Bloss) << format->Bshift
			       | format->Amask;*/
}
extern DECLSPEC SDL_Surface* SDLCALL SDL_CreateRGBSurface(Uint32 flags, int width, int height, int bitsPerPixel, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){
__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_CreateRGBSurface: %d %d %d %d", width, height, bitsPerPixel, flags);
	return SDL_SetVideoMode(width, height, bitsPerPixel, flags);
}
extern DECLSPEC int SDLCALL SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors){
	__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_SetColors");
	return 0;
}
extern DECLSPEC Uint8 SDLCALL SDL_GetMouseState(int *x, int *y){return 0;}
extern DECLSPEC Uint8 SDLCALL SDL_GetAppState(void){return SDL_APPACTIVE|SDL_APPINPUTFOCUS|SDL_APPMOUSEFOCUS;}
//extern DECLSPEC int SDLCALL SDL_PollEvent(SDL_Event *event) {return 0;}
//extern DECLSPEC int SDLCALL SDL_WaitEvent(SDL_Event *event) {return 0;}
extern DECLSPEC int SDLCALL SDL_EnableKeyRepeat(int delay, int interval){return 0;}
extern DECLSPEC int SDLCALL SDL_EnableUNICODE(int enable){return 0;}
extern DECLSPEC void SDLCALL SDL_CloseAudio(void){}


extern DECLSPEC void SDLCALL SDL_FreeSurface(SDL_Surface* surface){}
//extern DECLSPEC void SDLCALL SDL_VideoQuit(){}
//extern DECLSPEC int SDLCALL SDL_WM_ToggleFullScreen(SDL_Surface* surface){return 1;}
//extern DECLSPEC Uint8 * SDLCALL SDL_GetKeyState(int *numkeys){return 0;}

extern struct loader_config *loadf;

extern DECLSPEC void SDLCALL SDL_UnlockSurface(SDL_Surface* surface)
{
	//__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_UnlockSurface");
}

extern DECLSPEC int SDLCALL SDL_LockSurface(SDL_Surface* surface) {
	//__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_LockSurface");
	Android_LockSurface();
	surface->pixels = loadf->videoBuffer;
	return 0;
}

extern DECLSPEC SDL_Surface* SDLCALL SDL_SetVideoMode(int width, int height, int bitsperpixel, Uint32 flags) {
	//__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_SetVideoMode: %d %d %d %d", width, height, bitsperpixel, flags);
	//Android_SetVideoMode(width, height, bitsperpixel);
	Android_SetVideoMode(width, height, 16);
	sdl_surface.flags = (flags & SDL_FULLSCREEN);
	sdl_surface.w = width;
	sdl_surface.h = height;
	sdl_surface.pitch = width * 2;
	sdl_surface.format = &sdl_format;
	sdl_surface.pixels = loadf->videoBuffer;
	sdl_surface.format->BytesPerPixel = 2;
	sdl_surface.format->BitsPerPixel = 16;
	sdl_surface.format->palette = 0;
	sdl_surface.format->Rmask = 0x00f800;
	sdl_surface.format->Gmask = 0x0007e0;
	sdl_surface.format->Bmask = 0x00001f;
	sdl_surface.format->Amask = 0;
	sdl_surface.format->Rloss = 3;
	sdl_surface.format->Gloss = 2;
	sdl_surface.format->Bloss = 3;
	sdl_surface.format->Rshift = 11;
	sdl_surface.format->Gshift = 5;
	sdl_surface.format->Bshift = 0;

	return &sdl_surface;
}

extern DECLSPEC int SDLCALL SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
	__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_FillRect");
	return 0;
}

extern DECLSPEC void SDLCALL SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	if ((x == 0) && (y == 0) && (w == 0) && (h == 0))
		__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_UpdateRect: %d %d %d %d", x, y, w, h);
	Android_UnlockSurface(y, y+h);
}


#define SDL_malloc	malloc
#define SDL_free	free


static int SDLCALL stdio_seek(SDL_RWops *context, int offset, int whence)
{
	if ( fseek(context->hidden.stdio.fp, offset, whence) == 0 ) {
		return(ftell(context->hidden.stdio.fp));
	} else {
		//SDL_Error(SDL_EFSEEK);
		return(-1);
	}
}
static int SDLCALL stdio_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	size_t nread;

	nread = fread(ptr, size, maxnum, context->hidden.stdio.fp);
	if ( nread == 0 && ferror(context->hidden.stdio.fp) ) {
		//SDL_Error(SDL_EFREAD);
	}
	return(nread);
}
static int SDLCALL stdio_write(SDL_RWops *context, const void *ptr, int size, int num)
{
	size_t nwrote;

	nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
	if ( nwrote == 0 && ferror(context->hidden.stdio.fp) ) {
		//SDL_Error(SDL_EFWRITE);
	}
	return(nwrote);
}
static int SDLCALL stdio_close(SDL_RWops *context)
{
	if ( context ) {
		if ( context->hidden.stdio.autoclose ) {
			/* WARNING:  Check the return value here! */
			fclose(context->hidden.stdio.fp);
		}
		SDL_FreeRW(context);
	}
	return(0);
}

SDL_RWops *SDL_AllocRW(void)
{
	SDL_RWops *area;

	area = (SDL_RWops *)SDL_malloc(sizeof *area);
	if ( area == NULL ) {
		//SDL_OutOfMemory();
	}
	return(area);
}

void SDL_FreeRW(SDL_RWops *area)
{
	SDL_free(area);
}

SDL_RWops *SDL_RWFromFP(FILE *fp, int autoclose)
{
	SDL_RWops *rwops = NULL;

	rwops = SDL_AllocRW();
	if ( rwops != NULL ) {
		rwops->seek = stdio_seek;
		rwops->read = stdio_read;
		rwops->write = stdio_write;
		rwops->close = stdio_close;
		rwops->hidden.stdio.fp = fp;
		rwops->hidden.stdio.autoclose = autoclose;
	}
	return(rwops);
}

extern DECLSPEC SDL_RWops * SDLCALL  SDL_RWFromFile(const char *file, const char *mode){
	SDL_RWops *rwops = NULL;
	FILE *fp = NULL;
	if ( !file || !*file || !mode || !*mode ) {
		//SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
		return NULL;
	}

	fp = fopen(file, mode);
	if ( fp == NULL ) {
		//SDL_SetError("Couldn't open %s", file);
	} else {
		rwops = SDL_RWFromFP(fp, 1);
	}

	return(rwops);
}




extern DECLSPEC SDL_mutex * SDLCALL SDL_CreateMutex(void) {
	SDL_mutex *mutex;

	/* Allocate mutex memory */
	mutex = (SDL_mutex *)SDL_malloc(sizeof(*mutex));
	if ( mutex ) {
		/* Create the mutex semaphore, with initial value 1 */
		mutex->sem = 0;
		mutex->recursive = 0;
		mutex->owner = 0;
		if ( ! mutex->sem ) {
			SDL_free(mutex);
			mutex = NULL;
		}
	} else {
		//SDL_OutOfMemory();
	}
	return mutex;
}

extern DECLSPEC void SDLCALL SDL_DestroyMutex(SDL_mutex *mutex) {
	if ( mutex ) {
		if ( mutex->sem ) {
		}
		SDL_free(mutex);
	}
}

extern DECLSPEC int SDLCALL SDL_mutexP(SDL_mutex *mutex) {return 0;}
extern DECLSPEC int SDLCALL SDL_mutexV(SDL_mutex *mutex) {return 0;}

//extern DECLSPEC SDL_Overlay * SDLCALL SDL_CreateYUVOverlay(int width, int height, Uint32 format, SDL_Surface *display){return NULL;}
//extern DECLSPEC void SDLCALL SDL_FreeYUVOverlay(SDL_Overlay* surface){}
//extern DECLSPEC int SDLCALL SDL_LockYUVOverlay(SDL_Overlay *overlay) {return 0;}
//extern DECLSPEC void SDLCALL SDL_UnlockYUVOverlay(SDL_Overlay* surface) {}
//extern DECLSPEC int SDLCALL SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect){}

//extern DECLSPEC int SDLCALL SDL_LockSurface(SDL_Surface* surface) {return 0;}
//extern DECLSPEC void SDLCALL SDL_UnlockSurface(SDL_Surface* surface) {}
//extern DECLSPEC SDL_Surface* SDLCALL SDL_CreateRGBSurface(Uint32 flags, int width, int height, int bitsPerPixel, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){return NULL;}
//extern DECLSPEC void SDLCALL SDL_FreeSurface(SDL_Surface* surface){}

//extern DECLSPEC int SDLCALL SDL_PollEvent(SDL_Event *event) {return 0;}
//extern DECLSPEC int SDLCALL SDL_WaitEvent(SDL_Event *event) {return 0;}

//extern DECLSPEC void SDLCALL SDL_UpdateRects(SDL_Surface* screen, int numrects, SDL_Rect* rects){}
//extern DECLSPEC void SDLCALL SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h){}
//extern DECLSPEC int SDLCALL SDL_UpperBlit(SDL_Surface* src, const SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect) {}

//extern DECLSPEC SDL_Surface* SDLCALL SDL_SetVideoMode(int width, int height, int bitsperpixel, Uint32 flags) {return NULL;}
//extern DECLSPEC int SDLCALL SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags){return 16;}



extern DECLSPEC int	SDLCALL SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained){
	//__android_log_print(ANDROID_LOG_INFO,LOCNET_LOG_TAG, "SDL_OpenAudio");
	*obtained = *desired;
	mixerCallBack = desired->callback;
	int bufSize = Android_OpenAudio(desired->freq, desired->channels, 1, desired->samples << 2);

	if (bufSize == 0)
		return -1;

	obtained->samples = bufSize >> 2;
	Android_AudioGetBuffer();

	return 0;
}

extern DECLSPEC void SDLCALL SDL_Delay(Uint32 ms)
{
	int was_error;

	#if HAVE_NANOSLEEP
	struct timespec elapsed, tv;
	#else
	struct timeval tv;
	Uint32 then, now, elapsed;
	#endif

	/* Set the timeout interval */
	#if HAVE_NANOSLEEP
	elapsed.tv_sec = ms / 1000;
	elapsed.tv_nsec = (ms % 1000) * 1000000;
	#else
	then = SDL_GetTicks();
	#endif
	do {
	    //errno = 0;

	#if HAVE_NANOSLEEP
	    tv.tv_sec = elapsed.tv_sec;
	    tv.tv_nsec = elapsed.tv_nsec;
	    was_error = nanosleep(&tv, &elapsed);
	#else
	    /* Calculate the time interval left (in case of interrupt) */
	    now = SDL_GetTicks();
	    elapsed = (now - then);
	    then = now;
	    if (elapsed >= ms) {
	        break;
	    }
	    ms -= elapsed;
	    tv.tv_sec = ms / 1000;
	    tv.tv_usec = (ms % 1000) * 1000;

	    was_error = select(0, NULL, NULL, NULL, &tv);
	#endif /* HAVE_NANOSLEEP */
	//} while (was_error && (errno == EINTR));
	} while (was_error);
}

//extern DECLSPEC Uint32 SDLCALL SDL_GetTicks(void) {return TimGetTicks()*10;}
extern DECLSPEC Uint32 SDLCALL SDL_GetTicks(void)
{
#if HAVE_CLOCK_GETTIME
    Uint32 ticks;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    //ticks = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000;
    ticks = (now.tv_sec) * 1000 + (now.tv_nsec) / 1000000;
    return (ticks);
#else
    Uint32 ticks;
    struct timeval now;
    gettimeofday(&now, NULL);
    //ticks = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;
    ticks = (now.tv_sec) * 1000 + (now.tv_usec) / 1000;
    return (ticks);
#endif
}


//dummy functions

int sdl_joystick;

extern DECLSPEC SDL_Joystick *SDLCALL SDL_JoystickOpen(int index){return (SDL_Joystick *)&sdl_joystick;}
extern DECLSPEC void SDLCALL SDL_JoystickClose(SDL_Joystick *joystick){}

extern DECLSPEC int SDLCALL SDL_NumJoysticks(void) {return 1;}
extern DECLSPEC int SDLCALL SDL_JoystickNumButtons(SDL_Joystick *joystick){return 2;}
extern DECLSPEC Sint16 SDLCALL SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis){return 0;}
extern DECLSPEC int SDLCALL SDL_JoystickNumAxes(SDL_Joystick *joystick){return 2;}
extern DECLSPEC Uint8 SDLCALL SDL_JoystickGetButton(SDL_Joystick *joystick, int button){return 0;}
extern DECLSPEC int SDLCALL SDL_JoystickNumHats(SDL_Joystick *joystick){return 0;}
extern DECLSPEC int SDLCALL SDL_JoystickNumBalls(SDL_Joystick *joystick){return 0;}
//extern DECLSPEC Uint8 SDLCALL SDL_JoystickGetHat(SDL_Joystick *joystick, int hat){return 0;}
extern DECLSPEC const char * SDLCALL SDL_JoystickName(int index){return "Joystick " + index;}
extern DECLSPEC void SDLCALL SDL_JoystickUpdate(void) {}
extern DECLSPEC int SDLCALL SDL_JoystickEventState(int state){return 0;}

extern DECLSPEC char * SDLCALL SDL_GetKeyName(SDLKey key){return (char *)("Key " + key);}


extern DECLSPEC int SDLCALL SDL_Flip(SDL_Surface* screen) {return 0;}

extern DECLSPEC SDL_GrabMode SDLCALL SDL_WM_GrabInput(SDL_GrabMode mode) {return SDL_GRAB_ON;}
extern DECLSPEC int SDLCALL SDL_ShowCursor(int toggle){return SDL_ENABLE;}
extern DECLSPEC void SDLCALL SDL_WM_SetCaption(const char *title, const char *icon){}

extern DECLSPEC int SDLCALL SDL_CDNumDrives(){return 0;}
extern DECLSPEC const char * SDLCALL SDL_CDName(int drive){return "CDROM " + drive;}

extern DECLSPEC char * SDLCALL SDL_GetError(void){return (char *)"";}

extern DECLSPEC void SDLCALL SDL_PauseAudio(int pause_on){}
extern DECLSPEC void SDLCALL SDL_LockAudio(void){}
extern DECLSPEC void SDLCALL SDL_UnlockAudio(void){}

//extern DECLSPEC Uint32 SDLCALL SDL_MapRGB(SDL_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b){return 0;}
//extern DECLSPEC int SDLCALL SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors) {return 0;}

extern DECLSPEC int SDLCALL SDL_Init(Uint32 flags) {return 0;}
extern DECLSPEC int SDLCALL SDL_InitSubSystem(Uint32 flags) {return 0;}
extern DECLSPEC void SDLCALL SDL_Quit (void){}
#endif
