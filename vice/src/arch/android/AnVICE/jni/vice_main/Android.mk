LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vice_main

MY_PATH2 := ../../../../../..

CG_SUBDIRS := \
$(MY_PATH2)/src/rtc \
$(MY_PATH2)/src/c64/cart \
$(MY_PATH2)/src/drive/iec \
$(MY_PATH2)/src/drive/iecieee \
$(MY_PATH2)/src/drive/iec/c64exp \
$(MY_PATH2)/src/drive/ieee \
$(MY_PATH2)/src/drive \
$(MY_PATH2)/src/iecbus \
$(MY_PATH2)/src/lib/p64 \
$(MY_PATH2)/src/parallel \
$(MY_PATH2)/src/vdrive \
$(MY_PATH2)/src/userport

# Add more subdirs here, like src/subdir1 src/subdir2

MY_PATH := $(LOCAL_PATH)

CG_SRCDIR := $(LOCAL_PATH)

LOCAL_CFLAGS := -DANDROID_COMPILE

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/../sdl/include \
				$(LOCAL_PATH)/../locnet/include \
				$(LOCAL_PATH) \
				$(LOCAL_PATH)/../../../../../../src/vicii \
				$(LOCAL_PATH)/../../../../../../src/c64 \
				$(LOCAL_PATH)/../../../../../../src/c64/cart \
				$(LOCAL_PATH)/../../../../../../src/c64dtv \
				$(LOCAL_PATH)/../../../../../../src/imagecontents \
				$(LOCAL_PATH)/../../../../../../src/core \
				$(LOCAL_PATH)/../../../../../../src/tape \
				$(LOCAL_PATH)/../../../../../../src/rtc \
				$(LOCAL_PATH)/../../../../../../src/sid \
				$(LOCAL_PATH)/../../../../../../src/drive \
				$(LOCAL_PATH)/../../../../../../src/vdrive \
				$(LOCAL_PATH)/../../../../../../src/monitor \
				$(LOCAL_PATH)/../../../../../../src/lib/p64 \
				$(LOCAL_PATH)/../../../../../../src/userport \
				$(LOCAL_PATH)/../../../../../../src/platform \
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

LOCAL_PATH := $(MY_PATH)
	
# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
#LOCAL_STATIC_LIBRARIES := locnet_al

#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lsupc++ -ljnigraphics

#LOCAL_LDLIBS := -ldl -lsupc++ -ljnigraphics

LOCAL_ARM_MODE := arm

include $(BUILD_STATIC_LIBRARY)
