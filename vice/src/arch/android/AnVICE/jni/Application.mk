APP_PROJECT_PATH := $(call my-dir)/..

# Available libraries: mad sdl_mixer sdl_image sdl_ttf sdl_net sdl_blitpool sdl_gfx intl xml2 lua
# Available static libraries (specified in AppLdflags as "-ljpeg"): jpeg png tremor freetype xerces
# sdl_mixer depends on tremor and optionally mad
# sdl_image depends on png and jpeg
# sdl_ttf depends on freetype

#APP_MODULES := application sdl-1.2

#APP_ABI := armeabi armeabi-v7a
APP_ABI := armeabi
APP_STL := stlport_static
APP_CPPFLAGS += -O2
APP_PLATFORM := android-8
APP_OPTIM := release