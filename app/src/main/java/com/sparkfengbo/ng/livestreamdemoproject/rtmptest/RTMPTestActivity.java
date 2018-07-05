package com.sparkfengbo.ng.livestreamdemoproject.rtmptest;

import com.sparkfengbo.ng.livestreamdemoproject.base.CameraPreview;
import com.sparkfengbo.ng.livestreamdemoproject.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

/**
 * 使用ffmepg进行rtmp推流
 *
 * 主要使用RTMPTestActivity进行测试
 *
 * 功能包括：
 *
 * 1.使用FFMEPG进行推流
 * 2....
 */
public class RTMPTestActivity extends Activity {
    private CameraPreview mCameraRecorder;

    private RTMPController mRtmpController;

    private EditText mEditText;
    private Switch mSwitchPush;
    private Switch mSwitchLight;
    private Switch mSwitchCamera;
    private Switch mSwitchFaceDetect;
    private Switch mSwitchFilter;

    private boolean mIsRTMPPushing;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_rtmp_test);

        mSwitchLight = (Switch) findViewById(R.id.swFlash);
        mSwitchPush = (Switch) findViewById(R.id.sw_push_rtmp);
        mSwitchCamera = (Switch) findViewById(R.id.swCamera);
        mSwitchFaceDetect = (Switch) findViewById(R.id.swFaceDetect);
        mSwitchFilter = (Switch) findViewById(R.id.swFilter);
        mCameraRecorder = (CameraPreview) findViewById(R.id.camera_preview);
        mEditText = (EditText) findViewById(R.id.et_rtmp_url);
        mRtmpController = new RTMPController(mCameraRecorder);

        mSwitchPush.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    if (mIsRTMPPushing) {
                        Toast.makeText(getApplicationContext(), "正在推流！", Toast.LENGTH_SHORT).show();
                        mSwitchPush.setChecked(false);
                    } else {
                        if (!mEditText.getText().equals("")) {
                            boolean isSuccess = mRtmpController.startPush(mEditText.getText().toString());
                            if (isSuccess) {
                                mIsRTMPPushing = true;
                            } else {
                                mIsRTMPPushing = false;
                                mSwitchPush.setChecked(false);
                                Toast.makeText(getApplicationContext(), "推流失败！", Toast.LENGTH_SHORT).show();
                            }
                        }
                    }
                } else {
                    mRtmpController.stopPush();
                }
            }
        });

        //TODO 更改rtmp地址（确保测试手机和PC上的nginx服务器在同一网络,否则RTMP不通并且会crash）
        mEditText.setText("rtmp://172.22.127.122:1935/test/live");
    }

    @Override
    protected void onPause() {
        super.onPause();

        //TODO rtmp 推流暂停
    }

    @Override
    protected void onResume() {
        super.onResume();

        //TODO rtmp 继续
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        //TODO rtmp 释放
    }
}
