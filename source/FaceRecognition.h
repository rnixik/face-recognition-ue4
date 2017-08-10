#ifdef FACERECOGNITION_EXPORTS
#define FACERECOGNITION_API __declspec(dllexport) 
#else
#ifndef __ANDROID__
#define FACERECOGNITION_API __declspec(dllimport)
#else
#define FACERECOGNITION_API 
#endif
#endif

#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <vector>

#ifdef __ANDROID__
#include <jni.h>
#include <errno.h>
#include <android/log.h>

#define ALOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FaceRecognition", __VA_ARGS__))
#define ALOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FaceRecognition", __VA_ARGS__))
#endif

using namespace cv;
using namespace std;


class FACERECOGNITION_API FaceRecognition {

public:
	FaceRecognition();

	bool captureCamera(int cameraId, int width, int height);
	bool getActualVideoSize(int& width, int& height);
	void releaseCamera();
	bool loadClassifierFile(const char *classifierFile);
	
	void update(std::vector<uchar>& buffer);

	static cv::Mat androidFrame;

	void getFrame(Mat& frame);

private:

	VideoCapture* stream;
	bool m_isOpen;

	cv::CascadeClassifier faceCascade;
};


#ifdef __ANDROID__

extern "C" {
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved);
};

extern "C"
jboolean
Java_org_getid_facerecognition_AndroidCamera_FrameProcessing(
	JNIEnv* env, jobject thiz,
	jint width, jint height,
	jbyteArray NV21FrameData);

extern "C"
void
Java_org_getid_facerecognition_AndroidCamera_InitJNI(JNIEnv* env, jobject thiz);

#endif        
