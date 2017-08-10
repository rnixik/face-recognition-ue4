// FaceRecognition.cpp : Defines the exported functions for the DLL application.
//

#include "FaceRecognition.h"
#include <array>
#include <iostream>
#include <mutex>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

std::mutex frame_mutex;
cv::Mat FaceRecognition::androidFrame;


#ifdef __ANDROID__

JavaVM *gJavaVm = NULL;

jclass AndroidCameraClass;
jmethodID midSetPreviewSize;
jobject AndroidCameraObject;


JNIEnv* getEnv() {
	JNIEnv *env;

	int status = gJavaVm->GetEnv((void **)&env, JNI_VERSION_1_6);
	if (status == JNI_EDETACHED)
	{
		status = gJavaVm->AttachCurrentThread(&env, NULL);
		if (status != JNI_OK) {
			ALOGW("AttachCurrentThread failed %d", status);
			return nullptr;
		}
	}
	return env;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVm, void *reserved) {
	gJavaVm = javaVm;

	JNIEnv *env = getEnv();
	AndroidCameraClass = env->FindClass("org/getid/facerecognition/AndroidCamera");
	midSetPreviewSize = env->GetMethodID(AndroidCameraClass, "SetPreviewSize", "(II)V");

	return JNI_VERSION_1_6;
}



bool setPreviewSize(int width, int height) {
	JNIEnv *env = getEnv();

	if (nullptr == AndroidCameraObject) {
		ALOGW("AndroidCameraObject is null. Check InitJNI call");
		return false;
	}

	if (nullptr == midSetPreviewSize) {
		ALOGW("MethodId for SetPreviewSize is null");
		return false;
	}

	if (nullptr == env) {
		ALOGW("Env is null");
		return false;
	}

	try {
		//env->CallVoidMethod(AndroidCameraObject, midSetPreviewSize, width, height);
	}
	catch (std::exception) {
		ALOGW("Exception in calling method SetPreviewSize from JNI");
		return false;
	}

	return true;
}
#endif



FaceRecognition::FaceRecognition()
{
	m_isOpen = false;
}

bool FaceRecognition::captureCamera(int cameraId, int width, int height)
{
#ifdef __ANDROID__
	m_isOpen = setPreviewSize(width, height);
	return m_isOpen;
#endif

	stream = new cv::VideoCapture();
	stream->open(cameraId);
	stream->set(CV_CAP_PROP_FRAME_WIDTH, width);
	stream->set(CV_CAP_PROP_FRAME_HEIGHT, height);

	m_isOpen = stream->isOpened();
	return m_isOpen;
}

bool FaceRecognition::getActualVideoSize(int& width, int& height)
{
	if (m_isOpen && stream != nullptr) {
		Mat frame;
		getFrame(frame);

		if (!frame.data) {
			return false;
		}

		width = frame.size().width;
		height = frame.size().height;
		return true;
	}
	return false;
}

void FaceRecognition::releaseCamera()
{
	if (m_isOpen && stream != nullptr && stream->isOpened()) {
		stream->release();
		delete stream;
	}

	m_isOpen = false;
}

bool FaceRecognition::loadClassifierFile(const char *classifierFile)
{
	return faceCascade.load(classifierFile);
}

void FaceRecognition::getFrame(Mat& frame)
{
	if (!m_isOpen) {
		return;
	}

#ifndef __ANDROID__
	stream->read(frame);
#else
	frame_mutex.lock();
	frame = FaceRecognition::androidFrame.clone();
	frame_mutex.unlock();
#endif

}

void FaceRecognition::update(std::vector<uchar>& buffer)
{
	Mat frame;
	getFrame(frame);

	if (!frame.data) {
		return;
	}

	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	faceCascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(60, 60));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
	}

	(buffer).assign(frame.datastart, frame.dataend);
}


#ifdef __ANDROID__

extern "C"
void
Java_org_getid_facerecognition_AndroidCamera_InitJNI(JNIEnv* env, jobject thiz)
{
	AndroidCameraObject = thiz;
}

extern "C"
jboolean
Java_org_getid_facerecognition_AndroidCamera_FrameProcessing(
	JNIEnv* env, jobject thiz,
	jint width, jint height,
	jbyteArray NV21FrameData)
{

	jbyte* pNV21FrameData = env->GetByteArrayElements(NV21FrameData, NULL);


	cv::Mat yuv(height + height / 2, width, CV_8UC1, (uchar*)pNV21FrameData);
	cv::Mat bgr(height, width, CV_8UC4);
	cv::cvtColor(yuv, bgr, CV_YUV2BGR_NV21);

	frame_mutex.lock();
	FaceRecognition::androidFrame = bgr;
	frame_mutex.unlock();

	env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, JNI_ABORT);

	return true;
}


#endif
