package com.mopub.nativeads;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.mopub.common.MoPubBrowser;
import com.mopub.common.util.MoPubLog;

import java.lang.ref.SoftReference;
import java.util.Arrays;
import java.util.Iterator;

import static android.view.View.GONE;
import static android.view.View.OnClickListener;
import static android.view.View.VISIBLE;
import static com.mopub.common.HttpClient.makeTrackingHttpRequest;
import static com.mopub.common.util.IntentUtils.deviceCanHandleIntent;
import static com.mopub.common.util.IntentUtils.isDeepLink;

import static com.mopub.nativeads.MoPubNative.MoPubNativeListener;
import static com.mopub.nativeads.UrlResolutionTask.UrlResolutionListener;

class NativeAdViewHelper {
    private NativeAdViewHelper() {}

    static View getAdView(View convertView,
                          final ViewGroup parent,
                          final Context context,
                          final NativeResponse nativeResponse,
                          final ViewBinder viewBinder,
                          final MoPubNativeListener moPubNativeListener) {

        if (viewBinder == null) {
            MoPubLog.d("ViewBinder is null, returning empty view.");
            return new View(context);
        }

        if (convertView == null) {
            convertView = createConvertView(context, parent, viewBinder);
        }

        final NativeViewHolder nativeViewHolder = getOrCreateNativeViewHolder(convertView, viewBinder);

        // Clean up previous state of view
        removeClickListeners(convertView, nativeViewHolder);
        ImpressionTrackingManager.removeView(convertView);

        if (nativeResponse == null) {
            // If we don't have content for the view, then hide the view for now
            MoPubLog.d("NativeResponse is null, returning hidden view.");
            convertView.setVisibility(GONE);
        } else if (nativeResponse.isDestroyed()) {
            MoPubLog.d("NativeResponse is destroyed, returning hidden view.");
            convertView.setVisibility(GONE);
        } else if (nativeViewHolder == null) {
            MoPubLog.d("Could not create NativeViewHolder, returning hidden view.");
            convertView.setVisibility(GONE);
        } else {
            populateConvertViewSubViews(convertView, nativeViewHolder, nativeResponse, viewBinder);
            attachClickListeners(context, convertView, nativeViewHolder, nativeResponse, moPubNativeListener);
            convertView.setVisibility(VISIBLE);
            ImpressionTrackingManager.addView(convertView, nativeResponse, moPubNativeListener);
        }

        return convertView;
    }

    private static View createConvertView(final Context context, final ViewGroup parent, final ViewBinder viewBinder) {
        final View convertView = LayoutInflater
                .from(context)
                .inflate(viewBinder.layoutId, parent, false);
        return convertView;
    }

    static NativeViewHolder getOrCreateNativeViewHolder(final View convertView, final ViewBinder viewBinder) {
        // Create view holder and put it in the view tag
        Object object = ImageViewService.getViewTag(convertView);
        if (object == null || !(object instanceof NativeViewHolder)) {
            final NativeViewHolder nativeViewHolder = NativeViewHolder.fromViewBinder(convertView, viewBinder);
            ImageViewService.setViewTag(convertView, nativeViewHolder);
            return nativeViewHolder;
        } else {
            return (NativeViewHolder) object;
        }
    }

    private static void populateConvertViewSubViews(final View convertView,
                                                    final NativeViewHolder nativeViewHolder,
                                                    final NativeResponse nativeResponse,
                                                    final ViewBinder viewBinder) {
        nativeViewHolder.update(nativeResponse);
        nativeViewHolder.updateExtras(convertView, nativeResponse, viewBinder);
    }

    private static void removeClickListeners(final View view,
                                             final NativeViewHolder nativeViewHolder) {
        if (view == null) {
            return;
        }

        view.setOnClickListener(null);
        setCtaClickListener(nativeViewHolder, null);
    }

    private static void attachClickListeners(final Context context,
                                             final View view,
                                             final NativeViewHolder nativeViewHolder,
                                             final NativeResponse nativeResponse,
                                             final MoPubNativeListener moPubNativeListener) {
        if (view == null || nativeResponse == null) {
            return;
        }

        final String clickTrackerUrl = nativeResponse.getClickTracker();
        final String destinationUrl = nativeResponse.getClickDestinationUrl();
        final NativeViewClickListener nativeViewClickListener
                = new NativeViewClickListener(context, clickTrackerUrl, destinationUrl, moPubNativeListener);
        view.setOnClickListener(nativeViewClickListener);
        setCtaClickListener(nativeViewHolder, nativeViewClickListener);
    }

    private static void setCtaClickListener(final NativeViewHolder nativeViewHolder,
                                            final NativeViewClickListener nativeViewClickListener) {
        if (nativeViewHolder == null || nativeViewClickListener == null) {
            return;
        }

        // CTA widget could be a button and buttons don't inherit click listeners from parents
        // So we have to set it manually here if so
        if (nativeViewHolder.callToActionView != null && nativeViewHolder.callToActionView instanceof Button) {
            nativeViewHolder.callToActionView.setOnClickListener(nativeViewClickListener);
        }
    }

    static class NativeViewClickListener implements OnClickListener {
        private final Context mContext;
        private final String mClickTrackerUrl;
        private final String mDestinationUrl;
        private final MoPubNativeListener mMoPubNativeListener;

        NativeViewClickListener(final Context context,
                final String clickTrackerUrl,
                final String destinationUrl,
                final MoPubNativeListener moPubNativeListener) {
            mContext = context.getApplicationContext();
            mClickTrackerUrl = clickTrackerUrl;
            mDestinationUrl = destinationUrl;
            mMoPubNativeListener = moPubNativeListener;
        }

        @Override
        public void onClick(View view) {
            final SpinningProgressView spinningProgressView = new SpinningProgressView(mContext);
            spinningProgressView.addToRoot(view);

            // Fire and forget click tracker
            makeTrackingHttpRequest(mClickTrackerUrl);

            if (mDestinationUrl != null) {
                final Iterator<String> urlIterator = Arrays.asList(mDestinationUrl).iterator();
                final ClickDestinationUrlResolutionListener urlResolutionListener = new ClickDestinationUrlResolutionListener(
                        mContext,
                        urlIterator,
                        spinningProgressView,
                        mMoPubNativeListener,
                        view
                );
                UrlResolutionTask.getResolvedUrl(urlIterator.next(), urlResolutionListener);
            }
        }
    }

    private static class ClickDestinationUrlResolutionListener implements UrlResolutionListener {
        private final Context mContext;
        private final Iterator<String> mUrlIterator;
        private final SoftReference<SpinningProgressView> mSpinningProgressView;
        private final MoPubNativeListener mMoPubNativeListener;
        private final SoftReference<View> mView;

        public ClickDestinationUrlResolutionListener(final Context context,
                final Iterator<String> urlIterator,
                final SpinningProgressView spinningProgressView,
                final MoPubNativeListener moPubNativeListener,
                final View view) {
            mContext = context;
            mUrlIterator = urlIterator;
            mSpinningProgressView = new SoftReference<SpinningProgressView>(spinningProgressView);
            mMoPubNativeListener = (moPubNativeListener == null)
                    ? MoPubNativeListener.EMPTY_MOPUB_NATIVE_LISTENER
                    : moPubNativeListener;
            mView = new SoftReference<View>(view);
        }

        @Override
        public void onSuccess(String result) {
            final Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(result));
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

            if (isDeepLink(result)) {
                if (deviceCanHandleIntent(mContext, intent)) {
                    mMoPubNativeListener.onNativeClick(mView.get());
                    mContext.startActivity(intent);
                } else {
                    if (mUrlIterator.hasNext()) {
                        UrlResolutionTask.getResolvedUrl(mUrlIterator.next(), this);
                    } else {
                        mMoPubNativeListener.onNativeClick(mView.get());
                        MoPubBrowser.open(mContext, result);
                    }
                }
            } else {
                mMoPubNativeListener.onNativeClick(mView.get());
                MoPubBrowser.open(mContext, result);
            }

            if (mSpinningProgressView.get() != null) {
                mSpinningProgressView.get().removeFromRoot();
            }
        }

        @Override
        public void onFailure() {
            MoPubLog.d("Failed to resolve URL for click.");
            if (mSpinningProgressView.get() != null) {
                mSpinningProgressView.get().removeFromRoot();
            }
        }
    }
}
