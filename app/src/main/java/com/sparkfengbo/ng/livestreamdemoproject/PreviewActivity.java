package com.sparkfengbo.ng.livestreamdemoproject;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

public class PreviewActivity extends Activity {
    /**
     * 翻转摄像头
     */
    private Button mBtnTurnAround;

    /**
     * 打开/关闭闪光灯
     */
    private Button mBtnLignts;

    /**
     * 开始/结束推流
     */
    private Button mBtnPushLive;

    /**
     * 推流或展示其他信息
     */
    private TextView mPushInfoTextView;

    /**
     * 摄像头预览和采集
     */
    private CameraRecorder mCameraRecorder;

    private RecorderManager mRecorderManager;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_preview);

        mBtnTurnAround = (Button) findViewById(R.id.btn_turn_around);
        mBtnLignts = (Button) findViewById(R.id.btn_lignts);
        mBtnPushLive = (Button) findViewById(R.id.btn_push_live);
        mCameraRecorder = (CameraRecorder) findViewById(R.id.camera_preview);
        mPushInfoTextView = (TextView) findViewById(R.id.push_info);

        mBtnTurnAround.setOnClickListener(mOnClickListener);
        mBtnLignts.setOnClickListener(mOnClickListener);
        mBtnPushLive.setOnClickListener(mOnClickListener);

        mRecorderManager = new RecorderManager(mCameraRecorder);
    }

    private View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (view == mBtnTurnAround) {
                //TODO 摄像头翻转
                if (mCameraRecorder != null) {
                    mCameraRecorder.switchCamera();
                }
            } else if (view == mBtnLignts) {
                //TODO 闪光灯
            } else if  (view == mBtnPushLive) {
                //TODO 开始推流
                //TODO 摄像头翻转后的推流
                mRecorderManager.startLivePush();
            }
        }
    };
}
