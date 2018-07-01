/*
 * MIT License
 *
 * Copyright (c) 2017 Huawque
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author:weichang
 * https://github.com/javandoc/MediaPlus
 */

package com.sparkfengbo.ng.livestreamdemoproject.nativehandler;

import com.sparkfengbo.ng.livestreamdemoproject.util.Mog;

import android.content.Context;
import android.content.Intent;

/**
 * Shell M. Shrader - 1/17/16
 * https://github.com/SalomonBrys/__deprecated__Native-Crash-Handler
 * All Rights Reserved
 */
public class NativeCrashHandler {
    Context ctx;

    static {
        System.loadLibrary("native_crash_lib");
    }

    public NativeCrashHandler() {
    }

    private void makeCrashReport(String reason, StackTraceElement[] stack, int threadID) {
        if(stack != null) {
            NativeError.natSt = stack;
        }

        Mog.e("reason : " + reason + "stack trace : " + stack.toString() + " thread id : "  + threadID);

        NativeError e = new NativeError(reason, threadID);
        Intent intent = new Intent(this.ctx, NativeCrashActivity.class);
//        intent.addFlags(268435456);
        intent.putExtra("error", e);
        this.ctx.startActivity(intent);
    }

    public void registerForNativeCrash(Context ctx) {
        this.ctx = ctx;
        if(!this.nRegisterForNativeCrash()) {
            throw new RuntimeException("Could not register for native crash as nativeCrashHandler_onLoad was not called in JNI context");
        }
    }

    public void unregisterForNativeCrash() {
        this.ctx = null;
        this.nUnregisterForNativeCrash();
    }

    private native boolean nRegisterForNativeCrash();

    private native void nUnregisterForNativeCrash();
}