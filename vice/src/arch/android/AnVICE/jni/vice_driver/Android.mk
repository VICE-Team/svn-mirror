LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vice_driver

MY_PATH2 := ../../../../../..

CG_SUBDIRS := \
$(MY_PATH2)/src/fsdevice \
$(MY_PATH2)/src/tape \
$(MY_PATH2)/src/imagecontents \
$(MY_PATH2)/src/fileio \
$(MY_PATH2)/src/printerdrv \
$(MY_PATH2)/src/rs232drv \
$(MY_PATH2)/src/diskimage

# Add more subdirs here, like src/subdir1 src/subdir2

MY_PATH := $(LOCAL_PATH)

CG_SRCDIR := $(LOCAL_PATH)

LOCAL_CFLAGS := -DANDROID_COMPILE

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/../sdl/include \
				$(LOCAL_PATH)/../locnet/include \
				$(LOCAL_PATH) \
				$(LOCAL_PATH)/../../../../../../src \
				$(LOCAL_PATH)/../../../../../../src/vdrive \
				$(LOCAL_PATH)/../../../../../../src/lib/p64 \
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
LOCAL_SRC_FILES += $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))
LOCAL_SRC_FILES += \
	$(MY_PATH2)/src/gfxoutputdrv/bmpdrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/doodledrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/gfxoutput.c \
	$(MY_PATH2)/src/gfxoutputdrv/iffdrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/koaladrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/nativedrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/pcxdrv.c \
	$(MY_PATH2)/src/gfxoutputdrv/ppmdrv.c
	
# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
#LOCAL_STATIC_LIBRARIES := locnet_al

#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lsupc++ -ljnigraphics

#LOCAL_LDLIBS := -ldl -lsupc++ -ljnigraphics

LOCAL_ARM_MODE := arm

include $(BUILD_STATIC_LIBRARY)

