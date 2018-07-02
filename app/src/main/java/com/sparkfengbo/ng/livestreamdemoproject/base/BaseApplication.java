package com.sparkfengbo.ng.livestreamdemoproject.base;

import com.sparkfengbo.ng.livestreamdemoproject.util.AppCrashHandler;
import android.app.Application;

/**
 * Created by fengbo on 2018/6/27.
 */

public class BaseApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();

        //TODO 添加Java的CrashHandler
        AppCrashHandler crashHandler = AppCrashHandler.getInstance();
        crashHandler.init(this);

        //TODO 添加JNI的CrashHandler
//        NativeCrashHandler nativeCrashHandler = new NativeCrashHandler();
//        nativeCrashHandler.registerForNativeCrash(this);
    }
}