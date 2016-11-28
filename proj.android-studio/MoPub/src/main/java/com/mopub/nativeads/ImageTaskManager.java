package com.mopub.nativeads;

import android.graphics.Bitmap;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

abstract class ImageTaskManager {
    protected final ImageTaskManagerListener mImageTaskManagerListener;
    protected final int mSize;
    protected final Map<String, Bitmap> mImages;

    protected final AtomicInteger mCompletedCount;
    protected final AtomicBoolean mFailed;

    interface ImageTaskManagerListener {
        void onSuccess(final Map<String, Bitmap> images);
        void onFail();
    }

    ImageTaskManager(final List<String> urls, final ImageTaskManagerListener imageTaskManagerListener)
            throws IllegalArgumentException {
        if (urls == null) {
            throw new IllegalArgumentException("Urls list cannot be null");
        } else if (urls.contains(null)) {
            throw new IllegalArgumentException("Urls list cannot contain null");
        } else if (imageTaskManagerListener == null) {
            throw new IllegalArgumentException("ImageTaskManagerListener cannot be null");
        }

        mSize = urls.size();

        mImageTaskManagerListener = imageTaskManagerListener;
        mCompletedCount = new AtomicInteger(0);
        mFailed = new AtomicBoolean(false);
        mImages = Collections.synchronizedMap(new HashMap<String, Bitmap>(mSize));
    }

    abstract void execute();
}
