package org.getid.facerecognition;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;


import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

public class AndroidCamera implements Camera.PreviewCallback{
    private static final String TAG = "AndroidCamera";

    private Camera mCamera;
    private int imageFormat;

    private byte[] FrameData = null;
    private int PreviewSizeWidth = 640;
    private int PreviewSizeHeight = 480;
    private Camera.Size actualSize;
    private boolean bProcessing = false;
    
    private int mTextureID;
    private SurfaceTexture surfaceTexture;


    Handler mHandler = new Handler(Looper.getMainLooper());

    public static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open(0); // attempt to get a Camera instance
        }
        catch (Exception e){
            // Camera is not available (in use or does not exist)
        }
        return c; // returns null if camera is unavailable

    }


    public AndroidCamera(Context context) {
        //super(context);
        InitJNI();
    }

    public void startPreview() {
        Log.d(TAG, "Try to start preview");

        if (mCamera != null) {
            Log.w(TAG, "Already connected to camera! Skipping..");
            return;
        }

        mCamera = getCameraInstance();
        if (mCamera == null) {
            Log.e(TAG, "Cannot connect to camera");
            return;
        }

        Camera.Parameters parameters;
        parameters = mCamera.getParameters();
        parameters.setPreviewSize(PreviewSizeWidth, PreviewSizeHeight);
        if (parameters.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        } else if (parameters.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }

        imageFormat = parameters.getPreviewFormat();
        try {
            mCamera.setParameters(parameters);
        } catch (Exception e) {
            Log.e(TAG, "Can't set parameters");
            e.printStackTrace();
        }

        actualSize = parameters.getPreviewSize();
        Log.i(TAG, "Actual preview size: " + actualSize.width + "x" + actualSize.height);

		try {
		    int[] textures = new int[1];  
            GLES20.glGenTextures(1, textures, 0);  
            mTextureID = textures[0];  
            GLES20.glBindTexture(android.opengl.GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mTextureID); 

		    surfaceTexture = new SurfaceTexture(mTextureID);
			mCamera.setPreviewTexture(surfaceTexture);
		} catch (IOException e) {
		    Log.e(TAG, "Can't create or set SurfaceTexture");
			e.printStackTrace();
			return;
		}

        mCamera.setPreviewCallback(this);
        mCamera.startPreview();
    }

    public void stopPreview() {
        Log.d(TAG, "Try to stop preview");
        if (mCamera == null) {
            Log.w(TAG, "Already disconnected");
            return;
        }
        mCamera.setPreviewCallback(null);
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }


        @Override
        public void onPreviewFrame(byte[] arg0, Camera arg1)
        {
            //Log.d(TAG, "Got frame");

            // At preview mode, the frame data will push to here.
            if (imageFormat == ImageFormat.NV21)
            {
                //We only accept the NV21(YUV420) format.
                if ( !bProcessing )
                {
                    FrameData = arg0;
                    mHandler.post(DoImageProcessing);
                }
            } else {
                Log.e(TAG, "Not my format of frame");
            }
        }


        private Runnable DoImageProcessing = new Runnable()
        {
            public void run()
            {
                bProcessing = true;
                FrameProcessing(actualSize.width, actualSize.height, FrameData);
                bProcessing = false;
            }
        };

    public void SetPreviewSize(int width, int height) {
        Log.i(TAG, "SET PREVIEW SIZE: " + width + " x " + height);
        PreviewSizeWidth = width;
        PreviewSizeHeight = height;
        stopPreview();
        startPreview();
    }

    public native boolean InitJNI();
    public native boolean FrameProcessing(int width, int height, byte[] NV21FrameData);

}
