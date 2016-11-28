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
import android.view.Gravity;
import android.view.View;
import android.widget.RelativeLayout;
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
	
	public static native void delegateBannerOnError(long object, int code, int nativeCode, String descriptrion);
	public static native void delegateBannerOnLoaded(long object);
	public static native void delegateBannerOnClicked(long object);
	
	public static native void delegateInterstitialOnError(long object, int code, int nativeCode, String descriptrion);
	public static native void delegateInterstitialOnLoaded(long object);
	public static native void delegateInterstitialOnClicked(long object);
	public static native void delegateInterstitialOnDisplayed(long object);
	public static native void delegateInterstitialOnDismissed(long object);
	
	final private static int kAdsErrorInternalError = 0;
	final private static int kAdsErrorInvalidRequest = 1;
	final private static int kAdsErrorNoFill = 2;
	final private static int kAdsErrorNetworkError = 3;
	final private static int kAdsErrorServerError = 4;
	final private static int kAdsErrorOSVersionTooLow = 5;
	final private static int kAdsErrorTimeout = 6;
	final private static int kAdsErrorToFrequently = 7;
	final private static int kAdsErrorMediationError = 8;
	
	public static class FBInterstitialAd implements InterstitialAdListener
	{
		private InterstitialAd _interstitial;
		private long _pointer = 0;
		private Timer timer = new Timer();
		private boolean _visible = false;
		public FBInterstitialAd(String adUnitID, long pointer)
		{
			_pointer = pointer;
			_interstitial = new InterstitialAd(Cocos2dxHelper.getActivity(), adUnitID);
			_interstitial.setAdListener(this);
			_interstitial.loadAd();
		}
		public void done()
		{
			_interstitial.setAdListener(null);
			_pointer = 0;
			timer.cancel();
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        	activity.runOnUiThread(new Runnable() {
		                @Override
		                public void run() {
		                	_interstitial.loadAd();
		                }
		            });
		        }
		    };

		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		    if(_pointer != 0)
		    {
		    	final long lObject = _pointer;
		    	final int lNativeCode = paramAdError.getErrorCode();
		    	final String lDescription = paramAdError.getErrorMessage();
		    	int code = kAdsErrorInternalError;
		    	if(paramAdError==AdError.NO_FILL)
		    		code = kAdsErrorInternalError;
		    	if(paramAdError==AdError.LOAD_TOO_FREQUENTLY)
		    		code = kAdsErrorToFrequently;
		    	final int lCode = code;

		    	Cocos2dxHelper.runOnGLThread(new Runnable() {
		    		@Override
		    		public void run() {
		    			FacebookAdsHelper.delegateInterstitialOnError(lObject, lCode, lNativeCode, lDescription);
		    		}
		    	});
		    }
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateInterstitialOnLoaded(lObject);
					}
				});
			}
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateInterstitialOnClicked(lObject);
					}
				});
			}
		}
		@Override
		public void onInterstitialDisplayed(Ad paramAd)
		{
	    	_visible = true;
	    	if(_pointer != 0)
			{
	    		final long lObject = _pointer;
	    		Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateInterstitialOnDisplayed(lObject);
					}
				});
			}
		}
		@Override
		public void onInterstitialDismissed(Ad paramAd)
		{
	    	_visible = false;
	    	if(_pointer != 0)
			{
	    		final long lObject = _pointer;
	    		Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateInterstitialOnDismissed(lObject);
					}
				});
			}
		}
		public boolean isReady()
		{
			return _interstitial.isAdLoaded();
		}
		public boolean isVisible()
		{
			return _visible;
		}
		public boolean show()
		{
			if(!_interstitial.isAdLoaded())
				return false;
			_interstitial.show();
			return true;
		}
	};
	
	public static class FBBanner implements AdListener
	{
		private AdView _banner;
		private boolean _ready = false;
		private long _pointer = 0;
		private Timer timer = new Timer();
		private static RelativeLayout _layout = null;
		
		private static final int kAdSize320x50 = 0;
		private static final int kAdSizeHeight50Banner = 1;
		private static final int kAdSizeHeight90Banner = 2;
		
		private static final int kBannerGravityTopLeft = 0;
		private static final int kBannerGravityCenterLeft = 1;
		private static final int kBannerGravityBottomLeft = 2;
		private static final int kBannerGravityTopCenter = 3;
		private static final int kBannerGravityCenter = 4;
		private static final int kBannerGravityBottomCenter = 5;
		private static final int kBannerGravityTopRight = 6;
		private static final int kBannerGravityCenterRight = 7;
		private static final int kBannerGravityBottomRight = 8;
		
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
	            	throw new AssertionError();
	        }
	        _banner = new AdView(Cocos2dxHelper.getActivity(), adUnitID, bannerSize);
	        _banner.setAdListener(this);
	        _banner.loadAd();
		}
		public void done()
		{
			_banner.setAdListener(null); 
			_pointer = 0;
			timer.cancel();
		}
		@Override
		public void onError(Ad paramAd, AdError paramAdError)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        	activity.runOnUiThread(new Runnable() {
		                @Override
		                public void run() {
		                	_banner.loadAd();
		                }
		            });
		        }
		    };
		    _ready = false;
		    timer.schedule(action, (paramAdError==AdError.NO_FILL || paramAdError==AdError.LOAD_TOO_FREQUENTLY)?60000:10000);
		    if(_pointer != 0)
		    {
		    	final long lObject = _pointer;
		    	final int lNativeCode = paramAdError.getErrorCode();
		    	final String lDescription = paramAdError.getErrorMessage();
		    	int code = kAdsErrorInternalError;
		    	if(paramAdError==AdError.NO_FILL)
		    		code = kAdsErrorInternalError;
		    	if(paramAdError==AdError.LOAD_TOO_FREQUENTLY)
		    		code = kAdsErrorToFrequently;
		    	final int lCode = code;

		    	Cocos2dxHelper.runOnGLThread(new Runnable() {
		    		@Override
		    		public void run() {
		    			FacebookAdsHelper.delegateBannerOnError(lObject, lCode, lNativeCode, lDescription);
		    		}
		    	});
		    }
		}
		@Override
		public void onAdLoaded(Ad paramAd)
		{
			_ready = true;
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateBannerOnLoaded(lObject);
					}
				});
			}
		}
		@Override
		public void onAdClicked(Ad paramAd)
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						FacebookAdsHelper.delegateBannerOnClicked(lObject);
					}
				});
			}
		}
		public boolean isReady()
		{
			return _ready;
		}
		public boolean isVisible() 
		{
			if(_layout != null && _banner.getParent() != null)
			{
				return true;
			}
			return false;
		}
		public boolean show(int x, int y, int width, int height, int scaleType, int gravity)
		{
			if(_layout == null)
			{
				_layout = new RelativeLayout(activity);
				activity.addContentView(_layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
				_layout.setGravity(Gravity.TOP|Gravity.LEFT);
			}
			if(_banner.getParent() == null)
			{
				View parent = (View)_layout.getParent();
				
				RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(width, height);
				params.leftMargin = x;
				params.topMargin = parent.getHeight() - y - height;
				
				switch (gravity) {
					case kBannerGravityCenterLeft:
						_banner.setGravity(Gravity.CENTER_VERTICAL|Gravity.LEFT);
						break;
					case kBannerGravityBottomLeft:
						_banner.setGravity(Gravity.BOTTOM|Gravity.LEFT); 
						break;
					case kBannerGravityTopCenter:
						_banner.setGravity(Gravity.TOP|Gravity.CENTER_HORIZONTAL); 
						break;
					case kBannerGravityCenter:
						_banner.setGravity(Gravity.CENTER);
						break;
					case kBannerGravityBottomCenter:
						_banner.setGravity(Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL);
						break;
					case kBannerGravityTopRight:
						_banner.setGravity(Gravity.TOP|Gravity.RIGHT);
						break;
					case kBannerGravityCenterRight:
						_banner.setGravity(Gravity.CENTER_VERTICAL|Gravity.RIGHT);
						break;
					case kBannerGravityBottomRight:
						_banner.setGravity(Gravity.BOTTOM|Gravity.RIGHT);
						break;
					case kBannerGravityTopLeft:
					default:
						_banner.setGravity(Gravity.TOP|Gravity.LEFT);
						break;
				}
				
				_layout.addView(_banner, params);
				return true;
			}
			return false;
		}
		public boolean hide()
		{
			if(_layout != null && _banner.getParent() != null)
			{
				_layout.removeView(_banner);
				return true;
			}
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
	
	public static boolean interstitialIsVisible(FBInterstitialAd interstitial)
	{
		final FBInterstitialAd curInterstitial = interstitial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curInterstitial.isVisible();
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
	
	public static boolean bannerShow(FBBanner banner, int x, int y, int width, int height, int scaleType, int gravity)
	{
		final FBBanner curBanner = banner;
		final int curX = x;
		final int curY = y;
		final int curWidth = width;
		final int curHeight = height;
		final int curScaleType = scaleType;
		final int curGravity = gravity;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curBanner.show(curX, curY, curWidth, curHeight, curScaleType, curGravity);
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
	
	public static boolean bannerHide(FBBanner banner)
	{
		final FBBanner curBanner = banner;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curBanner.hide();
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
	
	public static boolean bannerIsReady(FBBanner banner)
	{
		final FBBanner curBanner = banner;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curBanner.isReady();
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
	
	public static boolean bannerIsVisible(FBBanner banner)
	{
		final FBBanner curBanner = banner;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curBanner.isVisible();
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
	
	public static String getSdkVersion()
	{
		return AdSdkVersion.BUILD;
	}
}
