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
	
	public static class BannerListener implements AdListener {
		private FBBanner _listener;
		public BannerListener(FBBanner listener)
		{
			_listener = listener;
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			_listener.onError(paramAdError);
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			_listener.onAdLoaded();
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			_listener.onAdClicked();
		}
	}
	
	public static class InterstitialListener implements InterstitialAdListener {
		private FBInterstitialAd _listener;
		public InterstitialListener(FBInterstitialAd listener)
		{
			_listener = listener;
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			_listener.onError(paramAdError);
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			_listener.onAdLoaded();
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			_listener.onAdClicked();
		}
		@Override
		public void onInterstitialDisplayed(Ad paramAd)
		{
			_listener.onInterstitialDisplayed();
		}
		@Override
		public void onInterstitialDismissed(Ad paramAd)
		{
			_listener.onInterstitialDismissed();
		}
	}
	
	public static class FBInterstitialAd
	{
		private InterstitialAd _interstitial;
		private InterstitialListener _listener;
		private long _pointer;
		private Timer timer = new Timer();
		public FBInterstitialAd(String adUnitID, long pointer)
		{
			_pointer = pointer;
			_interstitial = new InterstitialAd(Cocos2dxHelper.getActivity(), adUnitID);
			_listener = new InterstitialListener(this);
			_interstitial.setAdListener(_listener);
			_interstitial.loadAd();
		}
		public void done()
		{
			_interstitial.setAdListener(null);
			_pointer = 0;
		}
		public void onError(AdError paramAdError)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        }
		    };

		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		}
		public void onAdLoaded()
		{
			
		}
		public void onAdClicked()
		{
			
		}
		public void onInterstitialDisplayed()
		{
			
		}
		public void onInterstitialDismissed()
		{
			
		}
		public boolean isReady()
		{
			return _interstitial.isAdLoaded();
		}
		public boolean show()
		{
			if(!_interstitial.isAdLoaded())
				return false;
			_interstitial.show();
			return true;
		}
	};
	
	public static class FBBanner
	{
		private AdView _banner;
		private BannerListener _listener;
		private long _pointer;
		private Timer timer = new Timer();
		public FBBanner(String adUnitID, int size, long pointer)
		{
			_pointer = pointer;
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
	        _banner = new AdView(Cocos2dxHelper.getActivity(), adUnitID, bannerSize);
	        _listener = new BannerListener(this);
	        _banner.setAdListener(_listener);
	        _banner.loadAd();
		}
		public void done()
		{
			_banner.setAdListener(null); 
			_pointer = 0;
		}
		public void onError(AdError paramAdError)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        }
		    };

		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		}
		public void onAdLoaded()
		{
			
		}
		public void onAdClicked()
		{
			
		}
		public boolean isReady()
		{
			return false;
		}
		public boolean isVisible() 
		{
			return false;
		}
		public boolean show(int x, int y, int width, int height, int scaleType, int gravity)
		{
			return false;
		}
		public boolean show(int scaleType, int gravity)
		{
			return false;
		}
		public boolean hide()
		{
			return false;
		}
	};
	
	public static FBInterstitialAd createInterstitial(String adUnitID, long pointer)
	{
		final String curAdUnitID = adUnitID;
		final long curPointer = pointer;
		
		FutureTask<FBInterstitialAd> task = new FutureTask<FBInterstitialAd>(new Callable<FBInterstitialAd>() {
            @Override
            public FBInterstitialAd call() {
            	return new FBInterstitialAd(curAdUnitID, curPointer);
            }
        });
		
		activity.runOnUiThread(task);
		
		FBInterstitialAd ret = null;
		
		try{
			ret = task.get();
        }
		catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static void interstitialDone(FBInterstitialAd interstitial)
	{
		final FBInterstitialAd curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.done();
			}
		});
	}
	
	public static boolean interstitialShow(FBInterstitialAd interstitial)
	{
		final FBInterstitialAd curInterstitial = interstitial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curInterstitial.show();
            }
        });
		
		activity.runOnUiThread(task);
		
		Boolean ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static boolean interstitialIsReady(FBInterstitialAd interstitial)
	{
		final FBInterstitialAd curInterstitial = interstitial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curInterstitial.isReady();
            }
        });
		
		activity.runOnUiThread(task);
		
		Boolean ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static FBBanner createBanner(String adUnitID, int size, long pointer)
	{
		final String curAdUnitID = adUnitID;
		final long curPointer = pointer;
		final int curSize = size;
		
		FutureTask<FBBanner> task = new FutureTask<FBBanner>(new Callable<FBBanner>() {
            @Override
            public FBBanner call() {
				return new FBBanner(curAdUnitID, curSize, curPointer);
            }
        });
		
		activity.runOnUiThread(task);
		
		FBBanner ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static void bannerDone(FBBanner banner)
	{
		final FBBanner curBanner = banner;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curBanner.done();
			}
		});
	}
	
	public static String getSdkVersion()
	{
		return AdSdkVersion.BUILD;
	}
}
