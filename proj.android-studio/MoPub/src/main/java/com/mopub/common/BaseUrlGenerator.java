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

package com.mopub.common;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.net.Uri;
import android.provider.Settings;

import com.mopub.common.util.Utils;

import static com.mopub.common.util.Strings.isEmpty;

public abstract class BaseUrlGenerator {
    private static final String IFA_PREFIX = "ifa:";
    private static final String SHA_PREFIX = "sha:";

    private StringBuilder mStringBuilder;
    private boolean mFirstParam;

    public abstract String generateUrlString(String serverHostname);

    protected void initUrlString(String serverHostname, String handlerType) {
        mStringBuilder = new StringBuilder("http://" + serverHostname + handlerType);
        mFirstParam = true;
    }

    protected String getFinalUrlString() {
        return mStringBuilder.toString();
    }

    protected void addParam(String key, String value) {
        if (value == null || isEmpty(value)) {
            return;
        }

        mStringBuilder.append(getParamDelimiter());
        mStringBuilder.append(key);
        mStringBuilder.append("=");
        mStringBuilder.append(Uri.encode(value));
    }

    private String getParamDelimiter() {
        if (mFirstParam) {
            mFirstParam = false;
            return "?";
        }
        return "&";
    }

    protected void setApiVersion(String apiVersion) {
        addParam("v", apiVersion);
    }

    protected void setAppVersion(String appVersion) {
        addParam("av", appVersion);
    }

    protected void setExternalStoragePermission(boolean isExternalStoragePermissionGranted) {
        addParam("android_perms_ext_storage", isExternalStoragePermissionGranted ? "1" : "0");
    }

    protected void setDeviceInfo(String... info) {
        StringBuilder result = new StringBuilder();
        if (info == null || info.length < 1) {
            return;
        }

        for (int i=0; i<info.length-1; i++) {
            result.append(info[i]).append(",");
        }
        result.append(info[info.length-1]);

        addParam("dn", result.toString());
    }

    protected void setDoNotTrack(boolean dnt) {
        if (dnt) {
            addParam("dnt", "1");
        }
    }

    protected void setUdid(String udid) {
        addParam("udid", udid);
    }

    protected String getUdidFromContext(Context context) {
        /*
         * try to use the android id from Google Play Services if available
         * if not fall back on the device id
         */
        final String androidId = GpsHelper.getAdvertisingId(context);

        if (androidId != null) {
            return IFA_PREFIX + androidId;
        } else {
            String deviceId = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.ANDROID_ID);
            deviceId = (deviceId == null) ? "" : Utils.sha1(deviceId);
            return SHA_PREFIX + deviceId;
        }
    }

    protected String getAppVersionFromContext(Context context) {
        try {
            String packageName = context.getPackageName();
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(packageName, 0);
            return packageInfo.versionName;
        } catch (Exception exception) {
            return null;
        }
    }
}
