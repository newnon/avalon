package com.mopub.nativeads;

import android.graphics.Bitmap;
import com.mopub.common.CacheService;

import java.util.*;

import static com.mopub.common.CacheService.DiskLruCacheGetListener;

class ImageDiskTaskManager extends ImageTaskManager {
    private final List<String> mUrls;

    ImageDiskTaskManager(final List<String> urls, final ImageTaskManagerListener imageTaskManagerListener)
            throws IllegalArgumentException {
        super(urls, imageTaskManagerListener);
        mUrls = urls;
    }

    @Override
    void execute() {
        if (mUrls.isEmpty()) {
            mImageTaskManagerListener.onSuccess(mImages);
        }

        ImageDiskTaskListener imageDiskTaskListener = new ImageDiskTaskListener();
        for (final String url : mUrls) {
            CacheService.getFromDiskCacheAsync(url, imageDiskTaskListener);
        }
    }

    void failAllTasks() {
        if (mFailed.compareAndSet(false, true)) {
            mImageTaskManagerListener.onFail();
        }
    }

    private class ImageDiskTaskListener implements DiskLruCacheGetListener {
        @Override
        public void onComplete(final String key, final byte[] content) {
            if (key == null) {
                failAllTasks();
                return;
            } else {
                Bitmap bitmap = null;
                if (content != null) {
                     bitmap = ImageService.byteArrayToBitmap(content);
                }
                mImages.put(key, bitmap);
            }

            if (mCompletedCount.incrementAndGet() == mSize) {
                mImageTaskManagerListener.onSuccess(mImages);
            }
        }
    }
}
