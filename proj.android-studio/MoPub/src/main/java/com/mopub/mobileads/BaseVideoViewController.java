/*
 * Copyright (c) 2010-2013, MoPub Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *  Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *  Neither the name of 'MoPub Inc.' nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.mopub.mobileads;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.VideoView;

import static com.mopub.mobileads.EventForwardingBroadcastReceiver.ACTION_INTERSTITIAL_CLICK;
import static com.mopub.mobileads.EventForwardingBroadcastReceiver.ACTION_INTERSTITIAL_FAIL;

abstract class BaseVideoViewController {
    private final Context mContext;
    private final RelativeLayout mLayout;
    private final BaseVideoViewControllerListener mBaseVideoViewControllerListener;
    private long mBroadcastIdentifier;

    interface BaseVideoViewControllerListener {
        void onSetContentView(final View view);
        void onSetRequestedOrientation(final int requestedOrientation);
        void onFinish();
        void onStartActivityForResult(final Class<? extends Activity> clazz,
                final int requestCode,
                final Bundle extras);
    }

    BaseVideoViewController(final Context context, final long broadcastIdentifier, final BaseVideoViewControllerListener baseVideoViewControllerListener) {
        mContext = context.getApplicationContext();
        mBroadcastIdentifier = broadcastIdentifier;
        mBaseVideoViewControllerListener = baseVideoViewControllerListener;
        mLayout = new RelativeLayout(mContext);
    }

     void onCreate() {
        final RelativeLayout.LayoutParams adViewLayout = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.FILL_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        adViewLayout.addRule(RelativeLayout.CENTER_IN_PARENT);
        mLayout.addView(getVideoView(), 0, adViewLayout);
        mBaseVideoViewControllerListener.onSetContentView(mLayout);
    }

    abstract VideoView getVideoView();
    abstract void onPause();
    abstract void onResume();
    abstract void onDestroy();

    boolean backButtonEnabled() {
        return true;
    }

    void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        // By default, the activity result is ignored
    }

    BaseVideoViewControllerListener getBaseVideoViewControllerListener() {
        return mBaseVideoViewControllerListener;
    }

    Context getContext() {
        return mContext;
    }

    ViewGroup getLayout() {
        return mLayout;
    }

    void videoError(boolean shouldFinish) {
        Log.d("MoPub", "Error: video can not be played.");
        broadcastAction(ACTION_INTERSTITIAL_FAIL);
        if (shouldFinish) {
           mBaseVideoViewControllerListener.onFinish();
        }
    }

    void videoCompleted(boolean shouldFinish) {
        if (shouldFinish) {
            mBaseVideoViewControllerListener.onFinish();
        }
    }

    void videoClicked() {
        broadcastAction(ACTION_INTERSTITIAL_CLICK);
    }

    void broadcastAction(final String action) {
        EventForwardingBroadcastReceiver.broadcastAction(mContext, mBroadcastIdentifier, action);
    }
}
