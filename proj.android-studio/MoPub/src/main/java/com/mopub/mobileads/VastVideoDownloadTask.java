/*
 * Copyright (c) 2010-2013, MoPub Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *  Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *  Neither the name of 'MoPub Inc.' nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.mopub.mobileads;

import android.net.http.AndroidHttpClient;
import android.os.AsyncTask;

import com.mopub.common.CacheService;
import com.mopub.common.HttpClient;
import com.mopub.common.util.MoPubLog;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

public class VastVideoDownloadTask extends AsyncTask<String, Void, Boolean> {
    private static final int MAX_VIDEO_SIZE = 25 * 1024 * 1024; // 25 MiB

    public interface VastVideoDownloadTaskListener {
        public void onComplete(boolean success);
    }

    private final VastVideoDownloadTaskListener mVastVideoDownloadTaskListener;

    public VastVideoDownloadTask(final VastVideoDownloadTaskListener listener) {
        mVastVideoDownloadTaskListener = listener;
    }

    @Override
    protected Boolean doInBackground(final String... params) {
        if (params == null || params[0] == null) {
            return false;
        }

        final String videoUrl = params[0];
        AndroidHttpClient httpClient = null;
        try {
            httpClient = HttpClient.getHttpClient();
            final HttpGet httpget = new HttpGet(videoUrl);
            final HttpResponse response = httpClient.execute(httpget);

            if (response == null || response.getEntity() == null) {
                throw new IOException("Obtained null response from video url: " + videoUrl);
            }

            if (response.getEntity().getContentLength() > MAX_VIDEO_SIZE) {
                throw new IOException("Video exceeded max download size");
            }

            final InputStream inputStream = new BufferedInputStream(response.getEntity().getContent());
            final boolean diskPutResult = CacheService.putToDiskCache(videoUrl, inputStream);
            inputStream.close();
            return diskPutResult;
        } catch (Exception e) {
            MoPubLog.d("Failed to download video: " + e.getMessage());
            return false;
        } finally {
            if (httpClient != null) {
                httpClient.close();
            }
        }
    }

    @Override
    protected void onCancelled() {
        onPostExecute(false);
    }

    @Override
    protected void onPostExecute(final Boolean success) {
        if (mVastVideoDownloadTaskListener != null) {
            mVastVideoDownloadTaskListener.onComplete(success);
        }
    }
}
