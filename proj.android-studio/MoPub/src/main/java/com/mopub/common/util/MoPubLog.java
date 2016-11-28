package com.mopub.common.util;

import android.util.Log;

public class MoPubLog {
    private static final String LOGTAG = "MoPub";

    public static int d(final String message) {
        return MoPubLog.d(message, null);
    }

    public static int d(final String message, final Throwable throwable) {
        return Log.d(LOGTAG, message, throwable);
    }

    public static int w(final String message) {
        return MoPubLog.w(message, null);
    }

    public static int w(final String message, final Throwable throwable) {
        return Log.w(LOGTAG, message, throwable);
    }
}
