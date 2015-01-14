LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := badb-pan
LOCAL_SRC_FILES := bluetooth_shell_proc.c
LOCAL_CFLAGS += -I./jni/include
LOCAL_LDLIBS += -L./jni/libfrompanda -lbluetooth
include $(BUILD_EXECUTABLE)
#include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)
