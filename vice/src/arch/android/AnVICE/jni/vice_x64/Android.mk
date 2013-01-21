LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vice_x64

MY_PATH2 := ../../../../../..

CG_SUBDIRS := \
$(MY_PATH2)/src/c64 \

# Add more subdirs here, like src/subdir1 src/subdir2

MY_PATH := $(LOCAL_PATH)

CG_SRCDIR := $(LOCAL_PATH)

LOCAL_CFLAGS := -DANDROID_COMPILE

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/../sdl/include \
				$(LOCAL_PATH)/../locnet/include \
				$(LOCAL_PATH) \
				$(LOCAL_PATH)/../../../../../../src/drive \
				$(LOCAL_PATH)/../../../../../../src/drive/iec/c64exp \
				$(LOCAL_PATH)/../../../../../../src/tape \
				$(LOCAL_PATH)/../../../../../../src/vicii \
				$(LOCAL_PATH)/../../../../../../src/sid \
				$(LOCAL_PATH)/../../../../../../src/c64/cart \
				$(LOCAL_PATH)/../../../../../../src/lib/p64 \
				$(LOCAL_PATH)/../../../../../../src/userport \
				$(LOCAL_PATH)/../../../../../../src/video \
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
	$(MY_PATH2)/src/c64/c64-cmdline-options.c \
	$(MY_PATH2)/src/c64/c64-resources.c \
	$(MY_PATH2)/src/c64/c64-snapshot.c \
	$(MY_PATH2)/src/c64/c64.c \
	$(MY_PATH2)/src/c64/c64_256k.c \
	$(MY_PATH2)/src/c64/c64bus.c \
	$(MY_PATH2)/src/c64/c64cia1.c \
	$(MY_PATH2)/src/c64/c64cia2.c \
	$(MY_PATH2)/src/c64/c64datasette.c \
	$(MY_PATH2)/src/c64/c64drive.c \
	$(MY_PATH2)/src/c64/c64embedded.c \
	$(MY_PATH2)/src/c64/c64export.c \
	$(MY_PATH2)/src/c64/c64fastiec.c \
	$(MY_PATH2)/src/c64/c64gluelogic.c \
	$(MY_PATH2)/src/c64/c64iec.c \
	$(MY_PATH2)/src/c64/c64io.c \
	$(MY_PATH2)/src/c64/c64keyboard.c \
	$(MY_PATH2)/src/c64/c64mem.c \
	$(MY_PATH2)/src/c64/c64meminit.c \
	$(MY_PATH2)/src/c64/c64memlimit.c \
	$(MY_PATH2)/src/c64/c64memrom.c \
	$(MY_PATH2)/src/c64/c64memsnapshot.c \
	$(MY_PATH2)/src/c64/c64model.c \
	$(MY_PATH2)/src/c64/c64parallel.c \
	$(MY_PATH2)/src/c64/c64pla.c \
	$(MY_PATH2)/src/c64/c64printer.c \
	$(MY_PATH2)/src/c64/c64rom.c \
	$(MY_PATH2)/src/c64/c64romset.c \
	$(MY_PATH2)/src/c64/c64rsuser.c \
	$(MY_PATH2)/src/c64/c64sound.c \
	$(MY_PATH2)/src/c64/c64video.c \
	$(MY_PATH2)/src/c64/patchrom.c \
	$(MY_PATH2)/src/c64/plus256k.c \
	$(MY_PATH2)/src/c64/plus60k.c \
	$(MY_PATH2)/src/c64/psid.c \
	$(MY_PATH2)/src/c64/reloc65.c \
	
# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
#LOCAL_STATIC_LIBRARIES := locnet_al

#LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lsupc++ -ljnigraphics

#LOCAL_LDLIBS := -ldl -lsupc++ -ljnigraphics

LOCAL_ARM_MODE := arm

include $(BUILD_STATIC_LIBRARY)

