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


import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import com.mopub.common.MoPubBrowser;
import com.mopub.common.util.IntentUtils;
import com.mopub.mobileads.util.Utils;

class MoPubBrowserController extends MraidAbstractController {
    private static final String LOGTAG = "MoPubBrowserController";
    private Context mContext;

    MoPubBrowserController(MraidView view) {
        super(view);
        mContext = view.getContext();
    }
    
    protected void open(String url) {
        Log.d(LOGTAG, "Opening url: " + url);
        
        final MraidView mraidView = getMraidView();
        if (mraidView.getMraidListener() != null) {
            mraidView.getMraidListener().onOpen(mraidView);
        }

        // this is added because http/s can also be intercepted
        if (!isWebSiteUrl(url) && IntentUtils.canHandleApplicationUrl(mContext, url)) {
            launchApplicationUrl(url);
            return;
        }

        Intent i = new Intent(mContext, MoPubBrowser.class);
        i.putExtra(MoPubBrowser.DESTINATION_URL_KEY, url);
        i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(i);
    }

    private boolean launchApplicationUrl(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        String errorMessage = "Unable to open intent.";

        return Utils.executeIntent(getMraidView().getContext(), intent, errorMessage);
    }

    private boolean isWebSiteUrl(String url) {
        return url.startsWith("http://") || url.startsWith("https://");
    }
}
