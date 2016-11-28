package com.mopub.nativeads;

import android.graphics.Bitmap;
import android.view.View;
import android.widget.ImageView;

import com.mopub.common.util.MoPubLog;
import com.mopub.common.util.Utils;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Map;

import static com.mopub.nativeads.ImageService.ImageServiceListener;

class ImageViewService {
    // random large number so we hopefully don't collide with something a pub is using
    private static final int VIEW_TAG_MOPUB_KEY = 817491827;

    private ImageViewService(){}

    static void loadImageView(final String url, final ImageView imageView) {
        if (imageView == null) {
            return;
        }

        // Blank out previous image content while waiting for request to return
        imageView.setImageDrawable(null);

        if (url != null) {
            // Unique id to identify this async image request
            setImageViewUniqueId(imageView);
            long uniqueId = getImageViewUniqueId(imageView);

            // Async call to get image from memory cache, disk and then network
            ImageService.get(
                    Arrays.asList(url),
                    new MyImageViewServiceListener(url, imageView, uniqueId)
            );
        }
    }

    private static class MyImageViewServiceListener implements ImageServiceListener {
        private final WeakReference<ImageView> mImageView;
        private final String mUrl;
        private final long mUniqueId;

        MyImageViewServiceListener(final String url, final ImageView imageView, final long uniqueId) {
            mUrl = url;
            mImageView = new WeakReference<ImageView>(imageView);
            mUniqueId = uniqueId;
        }

        @Override
        public void onSuccess(final Map<String, Bitmap> bitmaps) {
            final ImageView imageView = mImageView.get();
            if (imageView == null || bitmaps == null || !bitmaps.containsKey(mUrl)) {
                return;
            }
            final Long uniqueId = getImageViewUniqueId(imageView);
            if (uniqueId != null && mUniqueId == uniqueId) {
                imageView.setImageBitmap(bitmaps.get(mUrl));
            }
        }

        @Override
        public void onFail() {
            MoPubLog.d("Failed to load image for ImageView");
        }
    }

    static void setImageViewUniqueId(final ImageView imageView) {
        if (imageView != null) {
            setViewTag(imageView, Utils.generateUniqueId());
        }
    }

    static Long getImageViewUniqueId(final ImageView imageView) {
        if (imageView != null) {
            Object object = getViewTag(imageView);
            if (object instanceof Long) {
                return (Long) object;
            }
        }
        return null;
    }

    static void setViewTag(final View view, final Object object) {
        view.setTag(VIEW_TAG_MOPUB_KEY, object);
    }

    static Object getViewTag(final View view) {
        return view.getTag(VIEW_TAG_MOPUB_KEY);
    }
}
