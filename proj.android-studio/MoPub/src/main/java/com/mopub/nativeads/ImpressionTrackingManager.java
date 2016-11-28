package com.mopub.nativeads;

import android.graphics.Rect;
import android.os.SystemClock;
import android.view.View;

import java.util.ConcurrentModificationException;
import java.util.Iterator;
import java.util.Map;
import java.util.WeakHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import static com.mopub.common.HttpClient.makeTrackingHttpRequest;
import static com.mopub.nativeads.MoPubNative.MoPubNativeListener;

class ImpressionTrackingManager {
    private static final int PERIOD = 250;

    private static WeakHashMap<View, NativeResponseWrapper> sKeptViews = new WeakHashMap<View, NativeResponseWrapper>(10);

    private static final ScheduledExecutorService sScheduledExecutorService = Executors.newSingleThreadScheduledExecutor();
    private static final VisibilityCheck sVisibilityCheck = new VisibilityCheck();
    private static AtomicBoolean mIsStarted = new AtomicBoolean(false);

    static void start() {
        if (mIsStarted.compareAndSet(false, true)) {
            /**
             * Scheduling with fixed delay means that the delay is calculated after the task
             * finishes running. This means that we will never have two tasks running at the same
             * time.
             */
            sScheduledExecutorService.scheduleWithFixedDelay(sVisibilityCheck, 0, PERIOD, TimeUnit.MILLISECONDS);
        }
    }

    static void stop() {
        if (mIsStarted.compareAndSet(true, false)) {
            sScheduledExecutorService.shutdownNow();
        }
    }

    static void addView(final View view, final NativeResponse nativeResponse, final MoPubNativeListener moPubNativeListener) {
        if (view != null && nativeResponse != null) {
            sKeptViews.put(view, new NativeResponseWrapper(nativeResponse, moPubNativeListener));
        }
    }

    static void removeView(final View view) {
        sKeptViews.remove(view);
    }

    static class VisibilityCheck implements Runnable {
        private static final double MINIMUM_PERCENT_VISIBLE = 50;
        private static final long MINIMUM_IMPRESSION_TIME = 1000;

        @Override
        public void run() {
            final Iterator<Map.Entry<View, NativeResponseWrapper>> entryIterator = sKeptViews.entrySet().iterator();

            while (entryIterator.hasNext()) {
                final Map.Entry<View, NativeResponseWrapper> entry = entryIterator.next();
                final View view = entry.getKey();
                final NativeResponseWrapper wrapper = entry.getValue();

                // if our wrapper or its response is null, skip
                if (wrapper == null || wrapper.mNativeResponse == null) {
                    try {
                        entryIterator.remove();
                    } catch (ConcurrentModificationException e) {
                        // continue
                    }
                    continue;
                }

                if (wrapper.mNativeResponse.isDestroyed()) {
                    try {
                        entryIterator.remove();
                    } catch (ConcurrentModificationException e) {
                        // continue
                    }
                    continue;
                }

                // if this response has already recorded an impression, skip
                if (wrapper.mNativeResponse.getRecordedImpression()) {
                    try {
                        entryIterator.remove();
                    } catch (ConcurrentModificationException e) {
                        // continue
                    }
                    continue;
                }

                // if the view is not sufficiently visible, reset the visible timestamp, and skip
                if (!isVisible(view)) {
                    wrapper.mFirstVisibleTimestamp = 0;
                    continue;
                }

                // if it just became visible, set the firstChecked timestamp, and skip
                if (wrapper.mFirstVisibleTimestamp == 0) {
                    wrapper.mFirstVisibleTimestamp = SystemClock.uptimeMillis();
                    continue;
                }

                // if not enough time has elapsed, skip
                if (SystemClock.uptimeMillis() - wrapper.mFirstVisibleTimestamp < MINIMUM_IMPRESSION_TIME) {
                    continue;
                }

                // otherwise, record an impression
                wrapper.mNativeResponse.recordImpression();

                // Fire and forget impression trackers
                for (final String impressionTracker : wrapper.mNativeResponse.getImpressionTrackers()) {
                    makeTrackingHttpRequest(impressionTracker);
                }

                // Notify the developer
                if (wrapper.mMoPubNativeListener != null) {
                    wrapper.mMoPubNativeListener.onNativeImpression(view);
                    try {
                        entryIterator.remove();
                    } catch (ConcurrentModificationException e) {
                        // continue
                    }
                }
            }
        }

        static boolean isVisible(final View view) {
            if (view == null || view.getVisibility() != View.VISIBLE) {
                return false;
            }

            final Rect visibleRect = new Rect();
            view.getGlobalVisibleRect(visibleRect);

            int visibleViewArea = visibleRect.width() * visibleRect.height();
            int totalViewArea = view.getWidth() * view.getHeight();

            if (totalViewArea <= 0) {
                return false;
            }

            double visiblePercent = 100 * visibleViewArea / totalViewArea;

            return visiblePercent >= MINIMUM_PERCENT_VISIBLE;
        }
    }

    static class NativeResponseWrapper {
        final NativeResponse mNativeResponse;
        final MoPubNativeListener mMoPubNativeListener;
        long mFirstVisibleTimestamp;

        NativeResponseWrapper(final NativeResponse nativeResponse,
                final MoPubNativeListener moPubNativeListener) {
            mNativeResponse = nativeResponse;
            mMoPubNativeListener = moPubNativeListener;
            mFirstVisibleTimestamp = 0;
        }
    }

    @Deprecated // for testing
    static void purgeViews() {
        sKeptViews.clear();
    }

    @Deprecated // for testing
    static Map<View, NativeResponseWrapper> getKeptViews() {
        return sKeptViews;
    }
}
