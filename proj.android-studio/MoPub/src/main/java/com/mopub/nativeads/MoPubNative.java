package com.mopub.nativeads;

import android.content.Context;
import android.graphics.Bitmap;
import android.view.View;

import com.mopub.common.DownloadResponse;
import com.mopub.common.DownloadTask;
import com.mopub.common.GpsHelper;
import com.mopub.common.HttpResponses;
import com.mopub.common.util.AsyncTasks;
import com.mopub.common.util.DeviceUtils;
import com.mopub.common.util.ManifestUtils;
import com.mopub.common.util.MoPubLog;

import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpUriRequest;
import org.json.JSONObject;

import java.util.Map;

import static com.mopub.common.GpsHelper.GpsHelperListener;
import static com.mopub.common.GpsHelper.asyncFetchAdvertisingInfo;

import static com.mopub.nativeads.MoPubNative.MoPubNativeListener.EMPTY_MOPUB_NATIVE_LISTENER;
import static com.mopub.nativeads.NativeErrorCode.CONNECTION_ERROR;
import static com.mopub.nativeads.NativeErrorCode.EMPTY_AD_RESPONSE;
import static com.mopub.nativeads.NativeErrorCode.IMAGE_DOWNLOAD_FAILURE;
import static com.mopub.nativeads.NativeErrorCode.INVALID_JSON;
import static com.mopub.nativeads.NativeErrorCode.INVALID_REQUEST_URL;
import static com.mopub.nativeads.NativeErrorCode.SERVER_ERROR_RESPONSE_CODE;
import static com.mopub.nativeads.NativeErrorCode.UNEXPECTED_RESPONSE_CODE;
import static com.mopub.nativeads.NativeErrorCode.UNSPECIFIED;

public final class MoPubNative {
    public interface MoPubNativeListener {
        public void onNativeLoad(final NativeResponse nativeResponse);
        public void onNativeFail(final NativeErrorCode errorCode);
        public void onNativeImpression(final View view);
        public void onNativeClick(final View view);

        public static final MoPubNativeListener EMPTY_MOPUB_NATIVE_LISTENER = new MoPubNativeListener() {
            @Override public void onNativeLoad(final NativeResponse nativeResponse) {}
            @Override public void onNativeFail(final NativeErrorCode errorCode) {}
            @Override public void onNativeImpression(final View view) {}
            @Override public void onNativeClick(final View view) {}
        };
    }

    private NativeResponse mNativeResponse;
    private final Context mContext;
    private final String mAdUnitId;
    private MoPubNativeListener mMoPubNativeListener;

    public MoPubNative(final Context context, final String adUnitId, final MoPubNativeListener moPubNativeListener) {
        ImpressionTrackingManager.start();

        if (context == null) {
            throw new IllegalArgumentException("Context may not be null.");
        } else if (adUnitId == null) {
            throw new IllegalArgumentException("AdUnitId may not be null.");
        } else if (moPubNativeListener == null) {
            throw new IllegalArgumentException("MoPubNativeListener may not be null.");
        }

        ManifestUtils.checkNativeActivitiesDeclared(context);

        mContext = context.getApplicationContext();
        mAdUnitId = adUnitId;
        mMoPubNativeListener = moPubNativeListener;
        
        // warm up cache for google play services info
        asyncFetchAdvertisingInfo(mContext);
    }

    public void makeRequest() {
        makeRequest(null);
    }

    public void makeRequest(final RequestParameters requestParameters) {
        makeRequest(requestParameters, new NativeGpsHelperListener(requestParameters));
    }

    void makeRequest(final RequestParameters requestParameters,
            NativeGpsHelperListener nativeGpsHelperListener) {
        if (!DeviceUtils.isNetworkAvailable(mContext)) {
            mMoPubNativeListener.onNativeFail(CONNECTION_ERROR);
            return;
        }

        // If we have access to Google Play Services (GPS) but the advertising info
        // is not cached then guarantee we get it before building the ad request url
        // in the callback, this is a requirement from Google
        GpsHelper.asyncFetchAdvertisingInfoIfNotCached(
                mContext,
                nativeGpsHelperListener
        );
    }
    public void destroy() {
        mMoPubNativeListener = EMPTY_MOPUB_NATIVE_LISTENER;
    }

    private void loadNativeAd(final RequestParameters requestParameters) {
        String endpointUrl = new NativeUrlGenerator(mContext)
                .withAdUnitId(mAdUnitId)
                .withRequest(requestParameters)
                .generateUrlString(Constants.NATIVE_HOST);

        if (endpointUrl != null) {
            MoPubLog.d("Loading ad from: " + endpointUrl);
        }

        final HttpGet httpGet;
        try {
            httpGet = new HttpGet(endpointUrl);
        } catch (IllegalArgumentException e) {
            mMoPubNativeListener.onNativeFail(INVALID_REQUEST_URL);
            return;
        }

        downloadJson(httpGet);
    }

    private void downloadJson(final HttpUriRequest httpUriRequest) {
        final DownloadTask jsonDownloadTask = new DownloadTask(new DownloadTask.DownloadTaskListener() {
            @Override
            public void onComplete(final String url, final DownloadResponse downloadResponse) {
                if (downloadResponse == null) {
                    mMoPubNativeListener.onNativeFail(UNSPECIFIED);
                } else if (downloadResponse.getStatusCode() >= 500 &&
                        downloadResponse.getStatusCode() < 600) {
                    mMoPubNativeListener.onNativeFail(SERVER_ERROR_RESPONSE_CODE);
                } else if (downloadResponse.getStatusCode() != HttpStatus.SC_OK) {
                    mMoPubNativeListener.onNativeFail(UNEXPECTED_RESPONSE_CODE);
                } else if (downloadResponse.getContentLength() == 0) {
                    mMoPubNativeListener.onNativeFail(EMPTY_AD_RESPONSE);
                } else {

                    final JSONObject jsonObject = HttpResponses.asJsonObject(downloadResponse);

                    if (jsonObject == null) {
                        mMoPubNativeListener.onNativeFail(INVALID_JSON);
                    } else {
                        try {
                            mNativeResponse = new NativeResponse(jsonObject);
                        } catch (IllegalArgumentException e) {
                            mMoPubNativeListener.onNativeFail(INVALID_JSON);
                            return;
                        }
                        downloadImages();
                    }
                }
            }
        });

        AsyncTasks.safeExecuteOnExecutor(jsonDownloadTask, httpUriRequest);
    }

    private void downloadImages() {
        ImageService.get(mContext, mNativeResponse.getAllImageUrls(), new ImageService.ImageServiceListener() {
            @Override
            public void onSuccess(Map<String, Bitmap> bitmaps) {
                mMoPubNativeListener.onNativeLoad(mNativeResponse);
            }

            @Override
            public void onFail() {
                mMoPubNativeListener.onNativeFail(IMAGE_DOWNLOAD_FAILURE);
            }
        });
    }

    // Do not store this class as a member of MoPubNative; will result in circular reference
    class NativeGpsHelperListener implements GpsHelperListener {
        private RequestParameters mRequestParameters;
        NativeGpsHelperListener(RequestParameters requestParameters) {
            mRequestParameters = requestParameters;
        }
        @Override
        public void onFetchAdInfoCompleted() {
            loadNativeAd(mRequestParameters);
        }
    }
}
