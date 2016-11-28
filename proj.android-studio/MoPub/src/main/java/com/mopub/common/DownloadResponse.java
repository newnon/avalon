package com.mopub.common;

import com.mopub.common.util.Streams;

import org.apache.http.HttpResponse;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;

public class DownloadResponse {
    private byte[] bytes = new byte[0];
    private final int statusCode;
    private final long contentLength;

    public DownloadResponse(final HttpResponse httpResponse) throws Exception {
        final ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        BufferedInputStream inputStream = null;
        try {
            inputStream = new BufferedInputStream(httpResponse.getEntity().getContent());
            Streams.copyContent(inputStream, outputStream);
            bytes = outputStream.toByteArray();
        } finally {
            Streams.closeStream(inputStream);
            Streams.closeStream(outputStream);
        }

        this.statusCode = httpResponse.getStatusLine().getStatusCode();
        this.contentLength = bytes.length;
    }

    public byte[] getByteArray() {
        return bytes;
    }

    public int getStatusCode() {
        return statusCode;
    }

    public long getContentLength() {
        return contentLength;
    }
}
