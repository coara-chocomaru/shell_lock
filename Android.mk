LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := sh
LOCAL_SRC_FILES := sh.cpp
LOCAL_CFLAGS := -Wall -Wextra -O2
include $(BUILD_EXECUTABLE)
