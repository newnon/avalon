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

import com.mopub.common.GpsHelper;

public class AdTypeTranslator {

    public enum CustomEventType {
        ADMOB_BANNER("admob_native_banner", "com.mopub.mobileads.GoogleAdMobBanner"),
        ADMOB_INTERSTITIAL("admob_full_interstitial", "com.mopub.mobileads.GoogleAdMobInterstitial"),
        GOOGLE_PLAY_BANNER("google_play_banner", "com.mopub.mobileads.GooglePlayServicesBanner"),
        GOOGLE_PLAY_INTERSTITIAL("google_play_interstitial", "com.mopub.mobileads.GooglePlayServicesInterstitial"),
        MILLENNIAL_BANNER("millennial_native_banner", "com.mopub.mobileads.MillennialBanner"),
        MILLENNIAL_INTERSTITIAL("millennial_full_interstitial", "com.mopub.mobileads.MillennialInterstitial"),
        MRAID_BANNER("mraid_banner", "com.mopub.mobileads.MraidBanner"),
        MRAID_INTERSTITIAL("mraid_interstitial", "com.mopub.mobileads.MraidInterstitial"),
        HTML_BANNER("html_banner", "com.mopub.mobileads.HtmlBanner"),
        HTML_INTERSTITIAL("html_interstitial", "com.mopub.mobileads.HtmlInterstitial"),
        VAST_VIDEO_INTERSTITIAL("vast_interstitial", "com.mopub.mobileads.VastVideoInterstitial"),

        UNSPECIFIED("", null);

        private final String mKey;
        private final String mClassName;

        private CustomEventType(String key, String className) {
            mKey = key;
            mClassName = className;
        }

        private static CustomEventType fromString(String key) {
            for (CustomEventType customEventType : CustomEventType.values()) {
                if (customEventType.mKey.equals(key)) {
                    return customEventType;
                }
            }

            return UNSPECIFIED;
        }

        @Override
        public String toString() {
            return mClassName;
        }
    }

    static String getAdNetworkType(String adType, String fullAdType) {
        String adNetworkType = "interstitial".equals(adType) ? fullAdType : adType;
        return adNetworkType != null ? adNetworkType : "unknown";
    }

    static String getCustomEventNameForAdType(MoPubView moPubView, String adType, String fullAdType) {
        CustomEventType customEventType;

        if ("html".equals(adType) || "mraid".equals(adType)) {
            customEventType = (isInterstitial(moPubView))
                    ? CustomEventType.fromString(adType + "_interstitial")
                    : CustomEventType.fromString(adType + "_banner");
        } else {
            customEventType = ("interstitial".equals(adType))
                    ? CustomEventType.fromString(fullAdType + "_interstitial")
                    : CustomEventType.fromString(adType + "_banner");

            if (moPubView != null) {
                customEventType = GpsHelper.convertAdMobToGooglePlayServices(moPubView.getContext(), customEventType);
            }
        }

        return customEventType.toString();
    }

    private static boolean isInterstitial(MoPubView moPubView) {
        return moPubView instanceof MoPubInterstitial.MoPubInterstitialView;
    }
}
