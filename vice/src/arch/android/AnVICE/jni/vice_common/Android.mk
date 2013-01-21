LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vice_common

MY_PATH2 := ../../../../../..

CG_SUBDIRS := \

# Add more subdirs here, like src/subdir1 src/subdir2

MY_PATH := $(LOCAL_PATH)

CG_SRCDIR := $(LOCAL_PATH)

LOCAL_CFLAGS := -DANDROID_COMPILE

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/../sdl/include \
				$(LOCAL_PATH)/../locnet/include \
				$(LOCAL_PATH) \
				$(LOCAL_PATH)/../../../../../../src/vdrive \
				$(LOCAL_PATH)/../../../../../../src/drive \
				$(LOCAL_PATH)/../../../../../../src/raster \
				$(LOCAL_PATH)/../../../../../../src/rs232drv \
				$(LOCAL_PATH)/../../../../../../src/c64 \
				$(LOCAL_PATH)/../../../../../../src/sid \
				$(LOCAL_PATH)/../../../../../../src/platform \
				$(LOCAL_PATH)/../../../../../../src/drive \
				$(LOCAL_PATH)/../../../../../../src/lib/p64 \
				$(LOCAL_PATH)/../../../../../../src/userport \
				$(LOCAL_PATH)/../../../../../../src/plus4 \
				$(LOCAL_PATH)/../../../../../../src/vic20 \
				$(LOCAL_PATH)/../../../../../../src/arch/sdl \
				$(LOCAL_PATH)/../../../../../../src/arch \
				$(LOCAL_PATH)/../../../../../../src \
				$(LOCAL_PATH)/../.. \
				$(LOCAL_PATH)

LOCAL_PATH := $(MY_PATH)

LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)
LOCAL_CXXFLAGS := $(LOCAL_CFLAGS)

#Change C++ file extension as appropriate
LOCAL_CPP_EXTENSION := .cpp

#LOCAL_SRC_FILES := $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.cpp))))
# Uncomment to also add C sources
#LOCAL_SRC_FILES += $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))
LOCAL_SRC_FILES += \
	$(MY_PATH2)/src/arch/sdl/archdep.c \
	$(MY_PATH2)/src/arch/sdl/blockdev.c \
	$(MY_PATH2)/src/arch/sdl/catweaselmkiii.c \
	$(MY_PATH2)/src/arch/sdl/console.c \
	$(MY_PATH2)/src/arch/sdl/coproc.c \
	$(MY_PATH2)/src/arch/sdl/dynlib.c \
	$(MY_PATH2)/src/arch/sdl/fullscreen.c \
	$(MY_PATH2)/src/arch/sdl/hardsid.c \
	$(MY_PATH2)/src/arch/sdl/joy.c \
	$(MY_PATH2)/src/arch/sdl/kbd.c \
	$(MY_PATH2)/src/arch/sdl/lightpendrv.c \
	$(MY_PATH2)/src/arch/sdl/menu_c64cart.c \
	$(MY_PATH2)/src/arch/sdl/menu_c64_common_expansions.c \
	$(MY_PATH2)/src/arch/sdl/menu_c64_expansions.c \
	$(MY_PATH2)/src/arch/sdl/menu_c64hw.c \
	$(MY_PATH2)/src/arch/sdl/menu_c64model.c \
	$(MY_PATH2)/src/arch/sdl/menu_common.c \
	$(MY_PATH2)/src/arch/sdl/menu_debug.c \
	$(MY_PATH2)/src/arch/sdl/menu_drive.c \
	$(MY_PATH2)/src/arch/sdl/menu_drive_rom.c \
	$(MY_PATH2)/src/arch/sdl/menu_ffmpeg.c \
	$(MY_PATH2)/src/arch/sdl/menu_help.c \
	$(MY_PATH2)/src/arch/sdl/menu_joystick.c \
	$(MY_PATH2)/src/arch/sdl/menu_lightpen.c \
	$(MY_PATH2)/src/arch/sdl/menu_midi.c \
	$(MY_PATH2)/src/arch/sdl/menu_mouse.c \
	$(MY_PATH2)/src/arch/sdl/menu_network.c \
	$(MY_PATH2)/src/arch/sdl/menu_printer.c \
	$(MY_PATH2)/src/arch/sdl/menu_ram.c \
	$(MY_PATH2)/src/arch/sdl/menu_reset.c \
	$(MY_PATH2)/src/arch/sdl/menu_rom.c \
	$(MY_PATH2)/src/arch/sdl/menu_rs232.c \
	$(MY_PATH2)/src/arch/sdl/menu_screenshot.c \
	$(MY_PATH2)/src/arch/sdl/menu_settings.c \
	$(MY_PATH2)/src/arch/sdl/menu_sid.c \
	$(MY_PATH2)/src/arch/sdl/menu_snapshot.c \
	$(MY_PATH2)/src/arch/sdl/menu_sound.c \
	$(MY_PATH2)/src/arch/sdl/menu_speed.c \
	$(MY_PATH2)/src/arch/sdl/menu_tape.c \
	$(MY_PATH2)/src/arch/sdl/menu_tfe.c \
	$(MY_PATH2)/src/arch/sdl/menu_video.c \
	$(MY_PATH2)/src/arch/sdl/mididrv.c \
	$(MY_PATH2)/src/arch/sdl/mousedrv.c \
	$(MY_PATH2)/src/arch/sdl/parsid.c \
	$(MY_PATH2)/src/arch/sdl/rs232.c \
	$(MY_PATH2)/src/arch/sdl/rs232dev.c \
	$(MY_PATH2)/src/arch/sdl/rs232net.c \
	$(MY_PATH2)/src/arch/sdl/sdlmain.c \
	$(MY_PATH2)/src/arch/sdl/signals.c \
	$(MY_PATH2)/src/arch/sdl/rawnetarch.c \
	$(MY_PATH2)/src/arch/sdl/ui.c \
	$(MY_PATH2)/src/arch/sdl/uicmdline.c \
	$(MY_PATH2)/src/arch/sdl/uifilereq.c \
	$(MY_PATH2)/src/arch/sdl/uihotkey.c \
	$(MY_PATH2)/src/arch/sdl/uimenu.c \
	$(MY_PATH2)/src/arch/sdl/uimon.c \
	$(MY_PATH2)/src/arch/sdl/uimsgbox.c \
	$(MY_PATH2)/src/arch/sdl/uipause.c \
	$(MY_PATH2)/src/arch/sdl/uipoll.c \
	$(MY_PATH2)/src/arch/sdl/uistatusbar.c \
	$(MY_PATH2)/src/arch/sdl/video.c \
	$(MY_PATH2)/src/arch/sdl/vkbd.c \
	$(MY_PATH2)/src/arch/sdl/vsidui.c \
	$(MY_PATH2)/src/arch/sdl/vsyncarch.c \
	$(MY_PATH2)/src/arch/sdl/x64_ui.c \
	$(MY_PATH2)/src/arch/sdl/x64sc_ui.c
	
# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
#LOCAL_STATIC_LIBRARIES := locnet_al

#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lsupc++ -ljnigraphics

#LOCAL_LDLIBS := -ldl -lsupc++ -ljnigraphics

LOCAL_ARM_MODE := arm

include $(BUILD_STATIC_LIBRARY)

