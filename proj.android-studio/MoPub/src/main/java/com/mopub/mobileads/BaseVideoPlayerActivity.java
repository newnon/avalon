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
import android.content.Context;
import android.content.Intent;

import com.mopub.common.util.MoPubLog;
import com.mopub.mobileads.util.vast.VastVideoConfiguration;

import static android.content.Intent.FLAG_ACTIVITY_NEW_TASK;
import static com.mopub.mobileads.AdFetcher.AD_CONFIGURATION_KEY;
import static com.mopub.mobileads.VastVideoViewController.VAST_VIDEO_CONFIGURATION;

class BaseVideoPlayerActivity extends Activity {
    static final String VIDEO_CLASS_EXTRAS_KEY = "video_view_class_name";
    static final String VIDEO_URL = "video_url";

    static void startMraid(final Context context, final String videoUrl, final AdConfiguration adConfiguration) {
        final Intent intentVideoPlayerActivity = createIntentMraid(context, videoUrl, adConfiguration);
        try {
            context.startActivity(intentVideoPlayerActivity);
        } catch (ActivityNotFoundException e) {
            MoPubLog.d("Activity MraidVideoPlayerActivity not found. Did you declare it in your AndroidManifest.xml?");
        }
    }

    static Intent createIntentMraid(final Context context,
            final String videoUrl,
            final AdConfiguration adConfiguration) {
        final Intent intentVideoPlayerActivity = new Intent(context, MraidVideoPlayerActivity.class);
        intentVideoPlayerActivity.setFlags(FLAG_ACTIVITY_NEW_TASK);
        intentVideoPlayerActivity.putExtra(VIDEO_CLASS_EXTRAS_KEY, "mraid");
        intentVideoPlayerActivity.putExtra(VIDEO_URL, videoUrl);
        intentVideoPlayerActivity.putExtra(AD_CONFIGURATION_KEY, adConfiguration);
        return intentVideoPlayerActivity;
    }

    static void startVast(final Context context,
            final VastVideoConfiguration vastVideoConfiguration,
            final AdConfiguration adConfiguration) {
        final Intent intentVideoPlayerActivity = createIntentVast(context, vastVideoConfiguration, adConfiguration);
        try {
            context.startActivity(intentVideoPlayerActivity);
        } catch (ActivityNotFoundException e) {
            MoPubLog.d("Activity MraidVideoPlayerActivity not found. Did you declare it in your AndroidManifest.xml?");
        }
    }

    static Intent createIntentVast(final Context context,
            final VastVideoConfiguration vastVideoConfiguration,
            final AdConfiguration adConfiguration) {
        final Intent intentVideoPlayerActivity = new Intent(context, MraidVideoPlayerActivity.class);
        intentVideoPlayerActivity.setFlags(FLAG_ACTIVITY_NEW_TASK);
        intentVideoPlayerActivity.putExtra(VIDEO_CLASS_EXTRAS_KEY, "vast");
        intentVideoPlayerActivity.putExtra(VAST_VIDEO_CONFIGURATION, vastVideoConfiguration);
        intentVideoPlayerActivity.putExtra(AD_CONFIGURATION_KEY, adConfiguration);
        return intentVideoPlayerActivity;
    }
}

