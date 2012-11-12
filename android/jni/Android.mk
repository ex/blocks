LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := helloworld_shared

LOCAL_MODULE_FILENAME := libhelloworld

LOCAL_CFLAGS    := -DSTC_SHOW_GHOST_PIECE -DSTC_WALL_KICK_ENABLED -DSTC_AUTO_ROTATION

LOCAL_SRC_FILES := stc/main.cpp \
                   ../../AppDelegate.cpp \
                   ../../PlatformCocos2DX.cpp \
                   ../../../../trunk/src/game.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../..

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,CocosDenshion/android)
