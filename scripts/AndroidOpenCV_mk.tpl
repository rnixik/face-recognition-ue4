LOCAL_PATH := $(call my-dir)

OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC
include _OPENCV_MK_PATH_

LOCAL_MODULE    := facerecognition
LOCAL_CFLAGS    := -std=gnu++11
LOCAL_SRC_FILES := ../source/FaceRecognition.cpp 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../source
LOCAL_LDLIBS    += -llog -ldl -lGLESv2
include $(BUILD_SHARED_LIBRARY)
