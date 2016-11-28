package com.mopub.nativeads;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;

import static com.mopub.nativeads.MoPubNative.MoPubNativeListener;

public final class AdapterHelper {
    private final Context mContext;
    private final int mStart;
    private final int mInterval;

    public AdapterHelper(final Context context, final int start, final int interval) throws IllegalArgumentException {
        if (context == null) {
            throw new IllegalArgumentException("Illegal argument: context was null.");
        } else if (start < 0) {
            throw new IllegalArgumentException("Illegal argument: negative starting position.");
        } else if (interval < 2) {
            throw new IllegalArgumentException("Illegal argument: interval must be at least 2.");
        }

        mContext = context.getApplicationContext();
        mStart = start;
        mInterval = interval;
    }

    public View getAdView(final View convertView,
                          final ViewGroup parent,
                          final NativeResponse nativeResponse,
                          final ViewBinder viewBinder,
                          final MoPubNativeListener moPubNativeListener) {
        return NativeAdViewHelper.getAdView(
                convertView,
                parent,
                mContext,
                nativeResponse,
                viewBinder,
                moPubNativeListener
        );
    }

    // Total number of content rows + ad rows
    public int shiftedCount(final int originalCount) {
        return originalCount + numberOfAdsThatCouldFitWithContent(originalCount);
    }

    // Shifted position of content in the backing list
    public int shiftedPosition(final int position) {
        return position - numberOfAdsSeenUpToPosition(position);
    }

    public boolean isAdPosition(final int position) {
        if (position < mStart) {
            return false;
        }

        return ((position - mStart) % mInterval == 0);
    }

    private int numberOfAdsSeenUpToPosition(final int position) {
        // This method takes a position from a list of content and ads mixed together
        // and calculates the number of ads seen up to that point

        if (position <= mStart) {
            return 0;
        }

        // Add 1 to the result since we start with an ad at start position and round down
        return (int) Math.floor((double) (position - mStart) / mInterval) + 1;
    }

    private int numberOfAdsThatCouldFitWithContent(final int contentRowCount) {
        // This method is passed the number of content rows from the backing list
        // and calculates how many ads could fit in with the content

        if (contentRowCount <= mStart) {
            return 0;
        }

        final int spacesBetweenAds = mInterval - 1;
        if ((contentRowCount - mStart) % spacesBetweenAds == 0) {
            // Don't add 1 to result since we never include an ad at the last position in the list
            return (contentRowCount - mStart) / spacesBetweenAds;
        } else {
            // Add 1 to the result since we start with an ad at start position and round down
            return (int) Math.floor((double) (contentRowCount - mStart) / spacesBetweenAds) + 1;
        }
    }
}
