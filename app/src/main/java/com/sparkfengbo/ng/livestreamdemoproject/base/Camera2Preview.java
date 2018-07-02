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
 * TODO 使用Camera2 进行摄像头预览画面
 * TODO 直接使用420P进行采样，则不需要C代码进行格式转换（NV21->I420P）
 */
public class Camera2Preview extends SurfaceView implements SurfaceHolder.Callback{

    public Camera2Preview(Context context) {
        super(context);
        init(context);
    }

    public Camera2Preview(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public Camera2Preview(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
