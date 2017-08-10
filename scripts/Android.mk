LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := UE4
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libUE4.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := facerecognition
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libfacerecognition.so
include $(PREBUILT_SHARED_LIBRARY)
