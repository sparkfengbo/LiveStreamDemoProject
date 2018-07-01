package com.sparkfengbo.ng.livestreamdemoproject.rtmptest;

import com.sparkfengbo.ng.livestreamdemoproject.CameraPreview;
import com.sparkfengbo.ng.livestreamdemoproject.R;
import com.sparkfengbo.ng.livestreamdemoproject.RecorderManager;
import com.sparkfengbo.ng.livestreamdemoproject.RtmpController;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

public class RtmpTestActivity extends Activity {
    /**
     * 摄像头预览和采集
     */
    private CameraPreview mCameraRecorder;

    private RtmpController mRtmpController;

    private EditText mEditText;
    private Switch mSwitchPush;
    private Switch mSwitchLight;
    private Switch mSwitchCamera;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_rtmp_test);

        mSwitchLight = (Switch) findViewById(R.id.swFlash);
        mSwitchPush = (Switch) findViewById(R.id.sw_push_rtmp);
        mSwitchCamera = (Switch) findViewById(R.id.swCamera);
        mCameraRecorder = (CameraPreview) findViewById(R.id.camera_preview);
        mEditText = (EditText) findViewById(R.id.et_rtmp_url);
        mRtmpController = new RtmpController(mCameraRecorder);
        mSwitchPush.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    if (!mEditText.getText().equals("") && !mRtmpController.startPush(mEditText.getText().toString())) {
                        mSwitchPush.setChecked(false);
                        Toast.makeText(getApplicationContext(), "推流失败！", Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });


        //TODO 更改rtmp地址
        mEditText.setText("rtmp://172.22.126.42:1935/test/live");
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
