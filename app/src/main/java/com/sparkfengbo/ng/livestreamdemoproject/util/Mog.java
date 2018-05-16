package com.sparkfengbo.ng.livestreamdemoproject.util;

import android.util.Log;

/**
 * Created by fengbo on 2017/7/31.
 */

public class Mog {
    private static String TAG = "fengbo";

    public static void i(String arg) {
        Log.i(TAG, generateStackTrace() + " : " +arg);
    }

    public static void d(String arg) {
        Log.d(TAG, generateStackTrace() + " : " +arg);
    }

    public static void e(String arg) {
        Log.e(TAG, generateStackTrace() + " : " +arg);
    }

    private static String generateStackTrace() {
        StackTraceElement[] elements = Thread.currentThread().getStackTrace();
        if (elements.length < 5) {
            return "";
        }
        StackTraceElement element = elements[4];
        String methodName = element.getMethodName();
        String className = element.getClassName();

        StringBuffer fullMsg = new StringBuffer(100);
        fullMsg.append(className);
        fullMsg.append(":");
        fullMsg.append(methodName);
        fullMsg.append(":");
        fullMsg.append(className);
        return fullMsg.toString();
    }
}
