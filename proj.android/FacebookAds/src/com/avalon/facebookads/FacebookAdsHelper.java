/*
 * Copyright (c) 2014 Sergey Perepelitsa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

package com.avalon.facebookads;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.widget.AbsoluteLayout;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.facebook.ads.internal.AdSdkVersion;
import com.facebook.ads.Ad;
import com.facebook.ads.AdError;
import com.facebook.ads.InterstitialAd;
import com.facebook.ads.AdSize;
import com.facebook.ads.AdView;
import com.facebook.ads.InterstitialAdListener;
import com.facebook.ads.AdListener;

public abstract class FacebookAdsHelper
{
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static final int kAdSize320x50 = 0;
	private static final int kAdSizeHeight50Banner = 1;
	private static final int kAdSizeHeight90Banner = 2;
	
	public static class adViewAdListener implements AdListener {
		private long pointer;
		private Timer timer = new Timer();
		public adViewAdListener(long value)
		{
			pointer = value;
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			final WeakReference<adViewAdListener> thisRef = new WeakReference<adViewAdListener>(this);
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		            if(thisRef != null)
		            {
		            }
		        }

		    };

		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			
		}
	}
	
	public static class interstitialListener implements InterstitialAdListener {
		private long pointer;
		private Timer timer = new Timer();
		public interstitialListener(long value)
		{
			pointer = value;
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			final WeakReference<interstitialListener> thisRef = new WeakReference<interstitialListener>(this);
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		            if(thisRef != null)
		            {
		            }
		        }

		    };

		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			
		}
		@Override
		public void onInterstitialDisplayed(Ad paramAd)
		{
			
		}
		@Override
		public void onInterstitialDismissed(Ad paramAd)
		{
			
		}
	}
	
	public static InterstitialAd createInterstitial(String adUnitID, long pointer)
	{
		final String curAdUnitID = adUnitID;
		final long curPointer = pointer;
		
		FutureTask<InterstitialAd> task = new FutureTask<InterstitialAd>(new Callable<InterstitialAd>() {
            @Override
            public InterstitialAd call() {
            	InterstitialAd interstitial = new InterstitialAd(Cocos2dxHelper.getActivity(), curAdUnitID);
            	interstitial.setAdListener(new interstitialListener(curPointer));
            	interstitial.loadAd();
        		return interstitial;
            }
        });
		
		activity.runOnUiThread(task);
		
		InterstitialAd ret = null;
		
		try{
			ret = task.get();
        }
		catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static AdView createBannerView(String adUnitID, int size)
	{
		final String curAdUnitID = adUnitID;
		AdSize bannerSize = null;
        switch (size) {
            case kAdSize320x50:
                bannerSize = AdSize.BANNER_320_50;
                break;
            case kAdSizeHeight50Banner:
                bannerSize = AdSize.BANNER_HEIGHT_50;
                break;
            case kAdSizeHeight90Banner:
                bannerSize = AdSize.BANNER_HEIGHT_90;
                break;
            default:
            	assert false : size;
                break;
        }
		
		final AdSize curAdSize = bannerSize;
		
		FutureTask<AdView> task = new FutureTask<AdView>(new Callable<AdView>() {
            @Override
            public AdView call() {
				return null;
            }
        });
		
		activity.runOnUiThread(task);
		
		AdView ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static String getSdkVersion()
	{
		return AdSdkVersion.BUILD;
	}
}
