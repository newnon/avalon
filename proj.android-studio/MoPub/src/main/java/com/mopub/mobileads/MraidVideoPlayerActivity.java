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
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import com.mopub.common.util.IntentUtils;

import static com.mopub.mobileads.EventForwardingBroadcastReceiver.ACTION_INTERSTITIAL_FAIL;
import static com.mopub.mobileads.EventForwardingBroadcastReceiver.broadcastAction;

public class MraidVideoPlayerActivity extends BaseVideoPlayerActivity implements BaseVideoViewController.BaseVideoViewControllerListener {
    private BaseVideoViewController mBaseVideoController;
    private long mBroadcastIdentifier;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        final AdConfiguration adConfiguration = getAdConfiguration();
        if (adConfiguration != null) {
            mBroadcastIdentifier = adConfiguration.getBroadcastIdentifier();
        } else {
            Log.d("MoPub", "Unable to obtain broadcast identifier. Video interactions cannot be tracked.");
        }

        try {
            mBaseVideoController = createVideoViewController();
        } catch (IllegalStateException e) {
            broadcastAction(this, mBroadcastIdentifier, ACTION_INTERSTITIAL_FAIL);
            finish();
            return;
        }

        mBaseVideoController.onCreate();
    }

    @Override
    protected void onPause() {
        mBaseVideoController.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mBaseVideoController.onResume();
    }

    @Override
    protected void onDestroy() {
        mBaseVideoController.onDestroy();
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        if (mBaseVideoController.backButtonEnabled()) {
            super.onBackPressed();
        }
    }

    @Override
    protected void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        mBaseVideoController.onActivityResult(requestCode, resultCode, data);
    }

    private AdConfiguration getAdConfiguration() {
        AdConfiguration adConfiguration;
        try {
            adConfiguration = (AdConfiguration) getIntent().getSerializableExtra(AdFetcher.AD_CONFIGURATION_KEY);
        } catch (ClassCastException e) {
            adConfiguration = null;
        }
        return adConfiguration;
    }

    private BaseVideoViewController createVideoViewController() throws IllegalStateException {
        String clazz = getIntent().getStringExtra(VIDEO_CLASS_EXTRAS_KEY);

        if ("vast".equals(clazz)) {
            return new VastVideoViewController(this, getIntent().getExtras(), mBroadcastIdentifier, this);
        } else if ("mraid".equals(clazz)) {
            return new MraidVideoViewController(this, getIntent().getExtras(), mBroadcastIdentifier, this);
        } else {
            throw new IllegalStateException("Unsupported video type: " + clazz);
        }
    }

    /**
     * Implementation of BaseVideoViewControllerListener
     */

    @Override
    public void onSetContentView(final View view) {
        setContentView(view);
    }

    @Override
    public void onSetRequestedOrientation(final int requestedOrientation) {
        setRequestedOrientation(requestedOrientation);
    }

    @Override
    public void onFinish() {
        finish();
    }

    @Override
    public void onStartActivityForResult(final Class<? extends Activity> clazz,
            final int requestCode,
            final Bundle extras) {
        if (clazz == null) {
            return;
        }

        final Intent intent = IntentUtils.getStartActivityIntent(this, clazz, extras);

        try {
            startActivityForResult(intent, requestCode);
        } catch (ActivityNotFoundException e) {
            Log.d("MoPub", "Activity " + clazz.getName() + " not found. Did you declare it in your AndroidManifest.xml?");
        }
    }

    @Deprecated // for testing
    BaseVideoViewController getBaseVideoViewController() {
        return mBaseVideoController;
    }

    @Deprecated // for testing
    void setBaseVideoViewController(final BaseVideoViewController baseVideoViewController) {
        mBaseVideoController = baseVideoViewController;
    }
}
