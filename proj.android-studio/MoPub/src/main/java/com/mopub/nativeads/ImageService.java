package com.mopub.nativeads;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.mopub.common.util.MoPubLog;
import com.mopub.common.CacheService;
import com.mopub.common.util.Streams;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.mopub.nativeads.ImageTaskManager.ImageTaskManagerListener;
import static java.util.Map.Entry;

class ImageService {
    private static int COMPRESSION_QUALITY = 25;

    interface ImageServiceListener {
        void onSuccess(Map<String, Bitmap> bitmaps);
        void onFail();
    }

    static void get(final Context context, final List<String> urls, final ImageServiceListener imageServiceListener) {
        CacheService.initializeCaches(context);
        get(urls, imageServiceListener);
    }

    static void get(final List<String> urls, final ImageServiceListener imageServiceListener) {
        final Map<String, Bitmap> cacheBitmaps = new HashMap<String, Bitmap>(urls.size());
        final List<String> urlCacheMisses = getBitmapsFromMemoryCache(urls, cacheBitmaps);

        if (urlCacheMisses.isEmpty()) {
            imageServiceListener.onSuccess(cacheBitmaps);
            return;
        }

        final ImageDiskTaskManager imageDiskTaskManager;
        try {
            imageDiskTaskManager = new ImageDiskTaskManager(
                    urlCacheMisses,
                    new ImageDiskTaskManagerListener(imageServiceListener, cacheBitmaps)
            );
        } catch (IllegalArgumentException e) {
            MoPubLog.d("Unable to initialize ImageDiskTaskManager", e);
            imageServiceListener.onFail();
            return;
        }

        imageDiskTaskManager.execute();
    }

    static void putBitmapsInCache(final Map<String, Bitmap> bitmaps) {
        for (final Entry<String, Bitmap> entry : bitmaps.entrySet()) {
            MoPubLog.d("Caching bitmap: " + entry.getKey());
            putBitmapInCache(entry.getKey(), entry.getValue());
        }
    }

    static void putBitmapInCache(final String key, final Bitmap bitmap) {
        final byte[] bytes = bitmapToByteArray(bitmap);
        CacheService.put(key, bytes);
    }

    static List<String> getBitmapsFromMemoryCache(final List<String> urls, final Map<String, Bitmap> hits) {
        final List<String> cacheMisses = new ArrayList<String>();
        for (final String url : urls) {
            Bitmap bitmap = getBitmapFromMemoryCache(url);

            if (bitmap != null) {
                hits.put(url, bitmap);
            } else {
                cacheMisses.add(url);
            }
        }

        return cacheMisses;
    }

    static Bitmap getBitmapFromMemoryCache(final String key) {
        Bitmap bitmap = null;
        byte[] bytes = CacheService.getFromMemoryCache(key);
        if (bytes != null) {
            bitmap = byteArrayToBitmap(bytes);
        }
        return bitmap;
    }

    static Bitmap byteArrayToBitmap(final byte[] bytes) {
        return BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
    }

    static byte[] bitmapToByteArray(final Bitmap bitmap) {
        ByteArrayOutputStream byteArrayOutputStream = null;
        try {
            byteArrayOutputStream = new ByteArrayOutputStream();
            bitmap.compress(Bitmap.CompressFormat.JPEG, COMPRESSION_QUALITY, byteArrayOutputStream);
            return byteArrayOutputStream.toByteArray();
        } finally {
            Streams.closeStream(byteArrayOutputStream);
        }
    }

    private static class ImageDiskTaskManagerListener implements ImageTaskManagerListener {
        final private ImageServiceListener mImageServiceListener;
        final private Map<String, Bitmap> mBitmaps;

        ImageDiskTaskManagerListener(final ImageServiceListener imageServiceListener,
                final Map<String, Bitmap> bitmaps) {
            mImageServiceListener = imageServiceListener;
            mBitmaps = bitmaps;
        }

        @Override
        public void onSuccess(final Map<String, Bitmap> diskBitmaps) {
            final List<String> urlDiskMisses = new ArrayList<String>();
            for (final Entry <String, Bitmap> entry : diskBitmaps.entrySet()) {
                if (entry.getValue() == null) {
                    urlDiskMisses.add(entry.getKey());
                } else {
                    putBitmapInCache(entry.getKey(), entry.getValue());
                    mBitmaps.put(entry.getKey(), entry.getValue());
                }
            }

            if (urlDiskMisses.isEmpty()) {
                mImageServiceListener.onSuccess(mBitmaps);
            } else {

                final ImageDownloadTaskManager imageDownloadTaskManager;
                try {
                    imageDownloadTaskManager = new ImageDownloadTaskManager(
                            urlDiskMisses,
                            new ImageNetworkTaskManagerListener(mImageServiceListener, mBitmaps)
                    );
                } catch (IllegalArgumentException e) {
                    MoPubLog.d("Unable to initialize ImageDownloadTaskManager", e);
                    mImageServiceListener.onFail();
                    return;
                }

                imageDownloadTaskManager.execute();
            }
        }

        @Override
        public void onFail() {
            mImageServiceListener.onFail();
        }
    }

    private static class ImageNetworkTaskManagerListener implements ImageTaskManagerListener {
        private final ImageServiceListener mImageServiceListener;
        private final Map<String, Bitmap> mBitmaps;

        ImageNetworkTaskManagerListener(final ImageServiceListener imageServiceListener,
                final Map<String, Bitmap> bitmaps) {
            mImageServiceListener = imageServiceListener;
            mBitmaps = bitmaps;
        }

        @Override
        public void onSuccess(final Map<String, Bitmap> images) {
            putBitmapsInCache(images);
            mBitmaps.putAll(images);
            mImageServiceListener.onSuccess(mBitmaps);
        }

        @Override
        public void onFail() {
            mImageServiceListener.onFail();
        }
    }

    // Testing, also performs disk IO
    @Deprecated
    static Bitmap getBitmapFromDiskCache(final String key) {
        Bitmap bitmap = null;
        byte[] bytes = CacheService.getFromDiskCache(key);
        if (bytes != null) {
            bitmap = byteArrayToBitmap(bytes);
        }
        return bitmap;
    }
}
