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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;

import static com.mopub.mobileads.CustomEventInterstitial.CustomEventInterstitialListener;
import static com.mopub.mobileads.MoPubErrorCode.NETWORK_INVALID_STATE;

class EventForwardingBroadcastReceiver extends BroadcastReceiver {
    private final CustomEventInterstitialListener mCustomEventInterstitialListener;
    private final long mBroadcastIdentifier;
    private Context mContext;

    static final String BROADCAST_IDENTIFIER_KEY = "broadcastIdentifier";
    static final String ACTION_INTERSTITIAL_FAIL = "com.mopub.action.interstitial.fail";
    static final String ACTION_INTERSTITIAL_SHOW = "com.mopub.action.interstitial.show";
    static final String ACTION_INTERSTITIAL_DISMISS = "com.mopub.action.interstitial.dismiss";
    static final String ACTION_INTERSTITIAL_CLICK = "com.mopub.action.interstitial.click";
    private static IntentFilter sIntentFilter;


    public EventForwardingBroadcastReceiver(CustomEventInterstitialListener customEventInterstitialListener, final long broadcastIdentifier) {
        mCustomEventInterstitialListener = customEventInterstitialListener;
        mBroadcastIdentifier = broadcastIdentifier;
        sIntentFilter = getHtmlInterstitialIntentFilter();
    }

    static void broadcastAction(final Context context, final long broadcastIdentifier, final String action) {
        Intent intent = new Intent(action);
        intent.putExtra(BROADCAST_IDENTIFIER_KEY, broadcastIdentifier);
        LocalBroadcastManager.getInstance(context.getApplicationContext()).sendBroadcast(intent);
    }

    static IntentFilter getHtmlInterstitialIntentFilter() {
        if (sIntentFilter == null) {
            sIntentFilter = new IntentFilter();
            sIntentFilter.addAction(ACTION_INTERSTITIAL_FAIL);
            sIntentFilter.addAction(ACTION_INTERSTITIAL_SHOW);
            sIntentFilter.addAction(ACTION_INTERSTITIAL_DISMISS);
            sIntentFilter.addAction(ACTION_INTERSTITIAL_CLICK);
        }
        return sIntentFilter;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        if (mCustomEventInterstitialListener == null) {
            return;
        }

        /**
         * Only consume this broadcast if the identifier on the received Intent and this broadcast
         * match up. This allows us to target broadcasts to the ad that spawned them. We include
         * this here because there is no appropriate IntentFilter condition that can recreate this
         * behavior.
         */
        final long receivedIdentifier = intent.getLongExtra(BROADCAST_IDENTIFIER_KEY, -1);
        if (mBroadcastIdentifier != receivedIdentifier) {
            return;
        }

        final String action = intent.getAction();
        if (ACTION_INTERSTITIAL_FAIL.equals(action)) {
            mCustomEventInterstitialListener.onInterstitialFailed(NETWORK_INVALID_STATE);
        } else if (ACTION_INTERSTITIAL_SHOW.equals(action)) {
            mCustomEventInterstitialListener.onInterstitialShown();
        } else if (ACTION_INTERSTITIAL_DISMISS.equals(action)) {
            mCustomEventInterstitialListener.onInterstitialDismissed();
            unregister();
        } else if (ACTION_INTERSTITIAL_CLICK.equals(action)) {
            mCustomEventInterstitialListener.onInterstitialClicked();
        }

    }

    public void register(Context context) {
        mContext = context;
        LocalBroadcastManager.getInstance(mContext).registerReceiver(this, sIntentFilter);
    }

    public void unregister() {
        if (mContext != null) {
            LocalBroadcastManager.getInstance(mContext).unregisterReceiver(this);
            mContext = null;
        }
    }
}
