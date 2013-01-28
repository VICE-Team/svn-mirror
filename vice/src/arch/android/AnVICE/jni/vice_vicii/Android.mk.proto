LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vice_vicii

MY_PATH2 := ../../../../../..

CG_SUBDIRS := \
$(MY_PATH2)/src/vicii \

# Add more subdirs here, like src/subdir1 src/subdir2

MY_PATH := $(LOCAL_PATH)

CG_SRCDIR := $(LOCAL_PATH)

LOCAL_CFLAGS := -DANDROID_COMPILE

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/../sdl/include \
				$(LOCAL_PATH)/../locnet/include \
				$(LOCAL_PATH) \
				$(LOCAL_PATH)/../../../../../../src/raster \
				$(LOCAL_PATH)/../../../../../../src/c64/cart \
				$(LOCAL_PATH)/../../../../../../src/c64 \
				$(LOCAL_PATH)/../../../../../../src/c64dtv \
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
	$(MY_PATH2)/src/vicii/vicii-badline.c \
	$(MY_PATH2)/src/vicii/vicii-clock-stretch.c \
	$(MY_PATH2)/src/vicii/vicii-cmdline-options.c \
	$(MY_PATH2)/src/vicii/vicii-color.c \
	$(MY_PATH2)/src/vicii/vicii-draw.c \
	$(MY_PATH2)/src/vicii/vicii-fetch.c \
	$(MY_PATH2)/src/vicii/vicii-irq.c \
	$(MY_PATH2)/src/vicii/vicii-mem.c \
	$(MY_PATH2)/src/vicii/vicii-phi1.c \
	$(MY_PATH2)/src/vicii/vicii-resources.c \
	$(MY_PATH2)/src/vicii/vicii-snapshot.c \
	$(MY_PATH2)/src/vicii/vicii-sprites.c \
	$(MY_PATH2)/src/vicii/vicii-stubs.c \
	$(MY_PATH2)/src/vicii/vicii-timing.c \
	$(MY_PATH2)/src/vicii/vicii.c \

# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
#LOCAL_STATIC_LIBRARIES := locnet_al

#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lsupc++ -ljnigraphics

#LOCAL_LDLIBS := -ldl -lsupc++ -ljnigraphics

LOCAL_ARM_MODE := arm

include $(BUILD_STATIC_LIBRARY)

