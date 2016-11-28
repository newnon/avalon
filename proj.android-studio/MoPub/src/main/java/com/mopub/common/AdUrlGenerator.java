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
import android.content.res.Configuration;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.telephony.TelephonyManager;

import com.mopub.common.util.DateAndTime;
import com.mopub.common.util.IntentUtils;

import java.text.SimpleDateFormat;

import static android.Manifest.permission.ACCESS_NETWORK_STATE;
import static android.content.pm.PackageManager.PERMISSION_GRANTED;
import static android.net.ConnectivityManager.TYPE_MOBILE;
import static android.net.ConnectivityManager.TYPE_WIFI;
import static com.mopub.common.AdUrlGenerator.MoPubNetworkType.ETHERNET;
import static com.mopub.common.AdUrlGenerator.MoPubNetworkType.MOBILE;
import static com.mopub.common.AdUrlGenerator.MoPubNetworkType.UNKNOWN;
import static com.mopub.common.AdUrlGenerator.MoPubNetworkType.WIFI;

public abstract class AdUrlGenerator extends BaseUrlGenerator {
    private static TwitterAppInstalledStatus sTwitterAppInstalledStatus = TwitterAppInstalledStatus.UNKNOWN;
    public static final String DEVICE_ORIENTATION_PORTRAIT = "p";
    public static final String DEVICE_ORIENTATION_LANDSCAPE = "l";
    public static final String DEVICE_ORIENTATION_SQUARE = "s";
    public static final String DEVICE_ORIENTATION_UNKNOWN = "u";

    // From ConnectivityManager
    public static final int TYPE_DUMMY = 0x8;
    public static final int TYPE_ETHERNET = 0x9;
    public static final int TYPE_MOBILE_DUN = 0x4;
    public static final int TYPE_MOBILE_HIPRI = 0x5;
    public static final int TYPE_MOBILE_MMS = 0x2;
    public static final int TYPE_MOBILE_SUPL = 0x3;

    protected Context mContext;
    protected TelephonyManager mTelephonyManager;
    protected ConnectivityManager mConnectivityManager;
    protected String mAdUnitId;
    protected String mKeywords;
    protected Location mLocation;
    protected boolean mFacebookSupportEnabled;

    public static enum TwitterAppInstalledStatus {
        UNKNOWN,
        NOT_INSTALLED,
        INSTALLED,
    }

    public static enum MoPubNetworkType {
        UNKNOWN,
        ETHERNET,
        WIFI,
        MOBILE;

        @Override
        public String toString() {
            return Integer.toString(ordinal());
        }
    }

    public AdUrlGenerator(Context context) {
        mContext = context;
        mTelephonyManager = (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        mConnectivityManager = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
    }

    public AdUrlGenerator withAdUnitId(String adUnitId) {
        mAdUnitId = adUnitId;
        return this;
    }

    public AdUrlGenerator withKeywords(String keywords) {
        mKeywords = keywords;
        return this;
    }

    public AdUrlGenerator withFacebookSupported(boolean enabled) {
        mFacebookSupportEnabled = enabled;
        return this;
    }

    public AdUrlGenerator withLocation(Location location) {
        mLocation = location;
        return this;
    }

    protected void setAdUnitId(String adUnitId) {
        addParam("id", adUnitId);
    }

    protected void setSdkVersion(String sdkVersion) {
        addParam("nv", sdkVersion);
    }

    protected void setKeywords(String keywords) {
        addParam("q", keywords);
    }

    protected void setLocation(Location location) {
        if (location != null) {
            addParam("ll", location.getLatitude() + "," + location.getLongitude());
            addParam("lla", "" + (int) location.getAccuracy());
        }
    }

    protected void setTimezone(String timeZoneOffsetString) {
        addParam("z", timeZoneOffsetString);
    }

    protected void setOrientation(int orientation) {
        String orString = DEVICE_ORIENTATION_UNKNOWN;
        if (orientation == Configuration.ORIENTATION_PORTRAIT) {
            orString = DEVICE_ORIENTATION_PORTRAIT;
        } else if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
            orString = DEVICE_ORIENTATION_LANDSCAPE;
        } else if (orientation == Configuration.ORIENTATION_SQUARE) {
            orString = DEVICE_ORIENTATION_SQUARE;
        }
        addParam("o", orString);
    }

    protected void setDensity(float density) {
        addParam("sc_a", "" + density);
    }

    protected void setMraidFlag(boolean mraid) {
        if (mraid) addParam("mr", "1");
    }

    protected void setMccCode(String networkOperator) {
        String mcc = networkOperator == null ? "" : networkOperator.substring(0, mncPortionLength(networkOperator));
        addParam("mcc", mcc);
    }

    protected void setMncCode(String networkOperator) {
        String mnc = networkOperator == null ? "" : networkOperator.substring(mncPortionLength(networkOperator));
        addParam("mnc", mnc);
    }

    protected void setIsoCountryCode(String networkCountryIso) {
        addParam("iso", networkCountryIso);
    }

    protected void setCarrierName(String networkOperatorName) {
        addParam("cn", networkOperatorName);
    }

    protected void setNetworkType(int type) {
        switch(type) {
            case TYPE_ETHERNET:
                addParam("ct", ETHERNET);
                break;
            case TYPE_WIFI:
                addParam("ct", WIFI);
                break;
            case TYPE_MOBILE:
            case TYPE_MOBILE_DUN:
            case TYPE_MOBILE_HIPRI:
            case TYPE_MOBILE_MMS:
            case TYPE_MOBILE_SUPL:
                addParam("ct", MOBILE);
                break;
            default:
                addParam("ct", UNKNOWN);
        }
    }

    private void addParam(String key, MoPubNetworkType value) {
        addParam(key, value.toString());
    }
    protected String getNetworkOperator() {
        String networkOperator = mTelephonyManager.getNetworkOperator();
        if (mTelephonyManager.getPhoneType() == TelephonyManager.PHONE_TYPE_CDMA &&
                mTelephonyManager.getSimState() == TelephonyManager.SIM_STATE_READY) {
            networkOperator = mTelephonyManager.getSimOperator();
        }
        return networkOperator;
    }

    private int mncPortionLength(String networkOperator) {
        return Math.min(3, networkOperator.length());
    }

    protected static String getTimeZoneOffsetString() {
        SimpleDateFormat format = new SimpleDateFormat("Z");
        format.setTimeZone(DateAndTime.localTimeZone());
        return format.format(DateAndTime.now());
    }

    protected int getActiveNetworkType() {
        if (mContext.checkCallingOrSelfPermission(ACCESS_NETWORK_STATE) == PERMISSION_GRANTED) {
            NetworkInfo activeNetworkInfo = mConnectivityManager.getActiveNetworkInfo();
            return activeNetworkInfo != null ? activeNetworkInfo.getType() : TYPE_DUMMY;
        }
        return TYPE_DUMMY;
    }

    protected void setTwitterAppInstalledFlag() {
        if (sTwitterAppInstalledStatus == TwitterAppInstalledStatus.UNKNOWN) {
            sTwitterAppInstalledStatus = getTwitterAppInstallStatus();
        }

        if (sTwitterAppInstalledStatus == TwitterAppInstalledStatus.INSTALLED) {
            addParam("ts", "1");
        }
    }

    public TwitterAppInstalledStatus getTwitterAppInstallStatus() {
        return IntentUtils.canHandleTwitterUrl(mContext) ? TwitterAppInstalledStatus.INSTALLED : TwitterAppInstalledStatus.NOT_INSTALLED;
    }

    @Deprecated // for testing
    public static void setTwitterAppInstalledStatus(TwitterAppInstalledStatus status) {
        sTwitterAppInstalledStatus = status;
    }
}
