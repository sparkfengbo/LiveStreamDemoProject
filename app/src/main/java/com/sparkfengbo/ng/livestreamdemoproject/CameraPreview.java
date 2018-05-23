package com.sparkfengbo.ng.livestreamdemoproject;

import com.sparkfengbo.ng.livestreamdemoproject.recorder.RecordConfig;
import com.sparkfengbo.ng.livestreamdemoproject.util.Mog;

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;
import java.util.List;

/**
 * Created by fengbo on 2018/5/2.
 *
 * 摄像头预览画面
 */

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback{

    private Context mContext;
    private Camera mCamera;
    private SurfaceHolder mSurfaceHolder;
    private boolean mFrontCamera;
    private int mRotate;
    private OnFrameDataCallback mOnFrameDataCallback;

    public CameraPreview(Context context) {
        super(context);
        init(context);
    }

    public CameraPreview(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public CameraPreview(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {
        mContext = context;
        mCamera = getCamera();
        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);
    }

    public void switchCamera() {
        try {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;

            mCamera = getCamera(!mFrontCamera);
            mCamera.setPreviewDisplay(mSurfaceHolder);
            mCamera.setPreviewCallback(mPreviewCallback);
            mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private Camera getCamera(){
        return getCamera(false);
    }

    private Camera getCamera(boolean isBack) {
        Camera camera = null;
        int cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        if(isBack) {
            cameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

        mFrontCamera = cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT;

        Camera.CameraInfo info = new Camera.CameraInfo();
        for(int i = 0; i < Camera.getNumberOfCameras(); i++) {
            Camera.getCameraInfo(i, info);
            if(info.facing == cameraId) {
                try {
                    camera = Camera.open(cameraId);

                } catch (RuntimeException ex) {

                }
                break;
            }
        }

        if(cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            camera.setDisplayOrientation(90);
            mRotate = 90;
        } else if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            camera.setDisplayOrientation(270);
            mRotate = 270;
        }

        Camera.Parameters parameters = camera.getParameters();

        //default is NV21
        parameters.setPreviewFormat(ImageFormat.YV12);

        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        Camera.Size optimalSize = getOptimalPreviewSize(sizes, getResources().getDisplayMetrics().widthPixels, getResources().getDisplayMetrics().heightPixels);

        parameters.setPreviewSize(optimalSize.width, optimalSize.height);


        parameters.setPreviewFrameRate(RecordConfig.VIDEO_FRAME_RATE);


        Mog.i("width : " + optimalSize.width + "   height : "  +optimalSize.height);
        camera.setParameters(parameters);

        camera.setPreviewCallback(mPreviewCallback);
        return camera;
    }

    private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, int w, int h) {
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) h / w;

        if (sizes == null) {
            return null;
        }

        Camera.Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        int targetHeight = h;

        for (Camera.Size size : sizes) {
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE) {
                continue;
            }
            if (Math.abs(size.height - targetHeight) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height - targetHeight);
            }
        }

        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Camera.Size size : sizes) {
                if (Math.abs(size.height - targetHeight) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - targetHeight);
                }
            }
        }
        return optimalSize;
    }

    private Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            Mog.d("" + data.length);
            if(mOnFrameDataCallback != null) {
                mOnFrameDataCallback.onFrameData(data, data.length, mRotate);
            }
        }
    };

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        Mog.e("surfaceCreated");
        if(mCamera != null) {
            Mog.e("mCamera not null");
            try {
                mCamera.setPreviewDisplay(mSurfaceHolder);
                mCamera.setPreviewCallback(mPreviewCallback);
                mCamera.startPreview();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        Mog.e("surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        Mog.e("surfaceDestroyed");
        if (mCamera != null) {
            mCamera.stopPreview();
        }
    }

    public void setCallback(OnFrameDataCallback callback) {
        mOnFrameDataCallback = callback;
    }

    interface OnFrameDataCallback {
        void onFrameData(byte[] data, int length, int rotate);
    }

    public void release () {
        if(mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

}
