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

package com.mopub.common.util;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

// Copy for dependency reasons
public class Streams {
    public static void copyContent(final InputStream inputStream, final OutputStream outputStream) throws IOException {
        if (inputStream == null || outputStream == null) {
            throw new IOException("Unable to copy from or to a null stream.");
        }

        byte[] buffer = new byte[16384];
        int length;

        while ((length = inputStream.read(buffer)) != -1) {
            outputStream.write(buffer, 0, length);
        }
    }

    public static void copyContent(final InputStream inputStream, final OutputStream outputStream, final long maxBytes) throws IOException {
        if (inputStream == null || outputStream == null) {
            throw new IOException("Unable to copy from or to a null stream.");
        }

        byte[] buffer = new byte[16384];
        int length;
        long totalRead = 0;

        while ((length = inputStream.read(buffer)) != -1) {
            totalRead += length;
            if (totalRead >= maxBytes) {
                throw new IOException("Error copying content: attempted to copy " +
                        totalRead + " bytes, with " + maxBytes + " maximum.");
            }

            outputStream.write(buffer, 0, length);
        }
    }

    public static void readStream(final InputStream inputStream, byte[] buffer) throws IOException {
        int offset = 0;
        int bytesRead = 0;
        int maxBytes = buffer.length;
        while ((bytesRead = inputStream.read(buffer, offset, maxBytes)) != -1) {
            offset += bytesRead;
            maxBytes -= bytesRead;
            if (maxBytes <= 0) {
                return;
            }
        }
    }

    public static void closeStream(Closeable stream) {
        if (stream == null) {
            return;
        }

        try {
            stream.close();
        } catch (IOException e) {
            // Unable to close the stream
        }
    }
}
