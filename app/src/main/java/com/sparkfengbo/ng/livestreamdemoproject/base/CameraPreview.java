package com.sparkfengbo.ng.livestreamdemoproject.base;

import com.sparkfengbo.ng.livestreamdemoproject.recorder.RtmpConfig;
import com.sparkfengbo.ng.livestreamdemoproject.util.FLog;

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
 * 使用android.hardware.Camera 进行摄像头预览画面
 */
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback{

    private Context mContext;
    private Camera mCamera;
    private SurfaceHolder mSurfaceHolder;
    //是否是前置摄像头
    private boolean mFrontCamera;
    //旋转角度
    private int mRotate;
    private OnFrameDataCallback mOnFrameDataCallback;
    //相机最佳Width
    private int mOptimalSizeWidth;
    //相机最佳Height
    private int mOptimalSizeHeight;

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

    private Camera getCamera(){
        return getCamera(false);
    }

    private Camera getCamera(boolean isBack) {
        Camera camera = null;
        int cameraId;
        if(isBack) {
            cameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        } else {
            cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        }

        mFrontCamera = cameraId == Camera.CameraInfo.CAMERA_FACING_FRONT;

        Camera.CameraInfo info = new Camera.CameraInfo();
        for(int i = 0; i < Camera.getNumberOfCameras(); i++) {
            Camera.getCameraInfo(i, info);
            if(info.facing == cameraId) {
                try {
                    camera = Camera.open(cameraId);
                } catch (RuntimeException ex) {
                    ex.printStackTrace();
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

        //default is NV21,使用NV21格式，底层C代码使用libyuv进行格式转换比较方便
        parameters.setPreviewFormat(ImageFormat.NV21);

        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        Camera.Size optimalSize = getOptimalPreviewSize(sizes, getResources().getDisplayMetrics().widthPixels, getResources().getDisplayMetrics().heightPixels);

        mOptimalSizeWidth = optimalSize.width;
        mOptimalSizeHeight = optimalSize.height;

        parameters.setPreviewSize(optimalSize.width, optimalSize.height);
        parameters.setPreviewFrameRate(RtmpConfig.VIDEO_FRAME_RATE);

        FLog.i("OptimalSize Width : " + optimalSize.width + " OptimalSize Height : "  +optimalSize.height);
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
            FLog.v("onPreviewFrame : " + data.length);
            if(mOnFrameDataCallback != null) {
                mOnFrameDataCallback.onFrameData(data, data.length, mRotate);
            }
        }
    };

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        FLog.i("surfaceCreated");
        if(mCamera != null) {
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
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        FLog.i("surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        FLog.i("surfaceDestroyed");
        if (mCamera != null) {
            mCamera.stopPreview();
        }
    }

    /**
     * 设置获取YUV数据回调
     * @param callback
     */
    public void setCallback(OnFrameDataCallback callback) {
        mOnFrameDataCallback = callback;
    }

    /**
     * 切换摄像头
     */
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

    /**
     * 释放资源
     */
    public void release () {
        if(mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    /**
     * Activity等外部获取YUV数据回调接口
     */
    public interface OnFrameDataCallback {
        /**
         * @param data      YUV数据
         * @param length    数据长度
         * @param rotate    旋转角度
         */
        void onFrameData(byte[] data, int length, int rotate);
    }
}
