package com.mopub.nativeads;

import android.graphics.Bitmap;

import com.mopub.common.DownloadResponse;
import com.mopub.common.DownloadTask;
import com.mopub.common.HttpResponses;
import com.mopub.common.util.AsyncTasks;
import com.mopub.common.util.MoPubLog;

import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpUriRequest;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.mopub.common.DownloadTask.DownloadTaskListener;

import static java.util.Map.Entry;

class ImageDownloadTaskManager extends ImageTaskManager {

    private final Map<HttpUriRequest, DownloadTask> mDownloadTasks;

    ImageDownloadTaskManager(final List<String> urls, final ImageTaskManagerListener imageTaskManagerListener)
            throws IllegalArgumentException {
        super(urls, imageTaskManagerListener);

        final DownloadTaskListener downloadTaskListener = new ImageDownloadTaskListener();
        mDownloadTasks = new HashMap<HttpUriRequest, DownloadTask>(urls.size());
        for (final String url : urls) {
            final HttpGet httpGet = new HttpGet(url);
            mDownloadTasks.put(httpGet, new DownloadTask(downloadTaskListener));
        }
    }

    @Override
    void execute() {
        if (mDownloadTasks.isEmpty()) {
            mImageTaskManagerListener.onSuccess(mImages);
        }

        for (final Entry<HttpUriRequest, DownloadTask> entry : mDownloadTasks.entrySet()) {
            final HttpUriRequest httpUriRequest = entry.getKey();
            final DownloadTask downloadTask = entry.getValue();

            AsyncTasks.safeExecuteOnExecutor(downloadTask, httpUriRequest);
        }
    }

    void failAllTasks() {
        if (mFailed.compareAndSet(false, true)) {
            for (final DownloadTask downloadTask : mDownloadTasks.values()) {
                downloadTask.cancel(true);
            }
            mImageTaskManagerListener.onFail();
        }
    }

    private class ImageDownloadTaskListener implements DownloadTaskListener {
        @Override
        public void onComplete(final String url, final DownloadResponse downloadResponse) {
            if (downloadResponse == null || downloadResponse.getStatusCode() != HttpStatus.SC_OK) {
                MoPubLog.d("Failed to download image: " + url);
                failAllTasks();
                return;
            }

            final Bitmap bitmap = HttpResponses.asBitmap(downloadResponse);

            if (bitmap == null) {
                MoPubLog.d("Failed to decode bitmap from response for image: " + url);
                failAllTasks();
                return;
            }

            MoPubLog.d("Successfully downloaded image: " + url);
            mImages.put(url, bitmap);
            if (mCompletedCount.incrementAndGet() == mSize) {
                mImageTaskManagerListener.onSuccess(mImages);
            }
        }
    }
}
