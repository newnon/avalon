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
import android.graphics.drawable.StateListDrawable;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.VideoView;
import com.mopub.common.util.Dips;

import static android.view.View.GONE;
import static android.view.View.OnClickListener;
import static android.view.View.VISIBLE;
import static com.mopub.common.util.Drawables.INTERSTITIAL_CLOSE_BUTTON_NORMAL;
import static com.mopub.common.util.Drawables.INTERSTITIAL_CLOSE_BUTTON_PRESSED;
import static com.mopub.mobileads.BaseVideoPlayerActivity.VIDEO_URL;

public class MraidVideoViewController extends BaseVideoViewController {
    private static final float CLOSE_BUTTON_SIZE = 50f;
    private static final float CLOSE_BUTTON_PADDING = 8f;

    private final VideoView mVideoView;
    private ImageButton mCloseButton;
    private int mButtonPadding;
    private int mButtonSize;

    MraidVideoViewController(final Context context, final Bundle bundle, final long broadcastIdentifier, final BaseVideoViewControllerListener baseVideoViewControllerListener) {
        super(context, broadcastIdentifier, baseVideoViewControllerListener);

        mVideoView = new VideoView(context);
        mVideoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                mCloseButton.setVisibility(VISIBLE);
                videoCompleted(true);
            }
        });

        mVideoView.setOnErrorListener(new MediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(MediaPlayer mediaPlayer, int what, int extra) {
                mCloseButton.setVisibility(VISIBLE);
                videoError(false);

                return false;
            }
        });

        mVideoView.setVideoPath(bundle.getString(VIDEO_URL));
    }

    @Override
    void onCreate() {
        super.onCreate();
        mButtonSize = Dips.asIntPixels(CLOSE_BUTTON_SIZE, getContext());
        mButtonPadding = Dips.asIntPixels(CLOSE_BUTTON_PADDING, getContext());
        createInterstitialCloseButton();
        mCloseButton.setVisibility(GONE);
        mVideoView.start();
    }

    @Override
    VideoView getVideoView() {
        return mVideoView;
    }

    @Override
    void onDestroy() {}

    @Override
    void onPause() {}

    @Override
    void onResume() {}

    private void createInterstitialCloseButton() {
        mCloseButton = new ImageButton(getContext());
        StateListDrawable states = new StateListDrawable();
        states.addState(new int[] {-android.R.attr.state_pressed}, INTERSTITIAL_CLOSE_BUTTON_NORMAL.decodeImage(getContext()));
        states.addState(new int[] {android.R.attr.state_pressed}, INTERSTITIAL_CLOSE_BUTTON_PRESSED.decodeImage(getContext()));
        mCloseButton.setImageDrawable(states);
        mCloseButton.setBackgroundDrawable(null);
        mCloseButton.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                getBaseVideoViewControllerListener().onFinish();
            }
        });

        RelativeLayout.LayoutParams buttonLayout = new RelativeLayout.LayoutParams(mButtonSize, mButtonSize);
        buttonLayout.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        buttonLayout.setMargins(mButtonPadding, 0, mButtonPadding, 0);
        getLayout().addView(mCloseButton, buttonLayout);
    }
}
