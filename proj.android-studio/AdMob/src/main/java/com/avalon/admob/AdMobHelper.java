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

package com.avalon.admob;

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import android.app.Activity;
import android.content.pm.PackageManager.NameNotFoundException;
import android.location.Location;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.LinearLayout.LayoutParams;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.mediation.NetworkExtras;
import com.google.android.gms.ads.mediation.admob.AdMobExtras;

import org.cocos2dx.lib.Cocos2dxHelper;

public abstract class AdMobHelper
{
	private static final Activity activity = Cocos2dxHelper.getActivity();
	
	private static String m_keyWords[] = null;
	private static String m_testDevices[] = null;
	private static NetworkExtras m_networkExtras = null;
	private static Date m_Birthday = null;
	private static int m_Gender = 0;
	private static Location m_Location;
	private static Boolean m_TagForChildDirectedTreatment = null;
	
	public static native void delegateBannerDidReceiveAd(long object);
	public static native void delegateBannerDidFailToReceiveAd(long object, int code, int nativeCode, String description);
	public static native void delegateBannerWillPresentScreen(long object);
	public static native void delegateBannerWillDismissScreen(long object);
	public static native void delegateBannerWillLeaveApplication(long object);

	public static native void delegateInterstitialDidReceiveAd(long object);
	public static native void delegateInterstitialDidFailToReceiveAd(long object, int code, int nativeCode, String description);
	public static native void delegateInterstitialWillPresentScreen(long object);
	public static native void delegateInterstitialWillDismissScreen(long object);
	public static native void delegateInterstitialWillLeaveApplication(long object);
	
	final private static int kAdsErrorInternalError = 0;
	final private static int kAdsErrorInvalidRequest = 1;
	final private static int kAdsErrorNoFill = 2;
	final private static int kAdsErrorNetworkError = 3;
	final private static int kAdsErrorServerError = 4;
	final private static int kAdsErrorOSVersionTooLow = 5;
	final private static int kAdsErrorTimeout = 6;
	final private static int kAdsErrorToFrequently = 7;
	final private static int kAdsErrorMediationError = 8;
	
	public static class AdmobInterstitialAd extends AdListener
	{
		private InterstitialAd _interstitial = null;
		private long _pointer = 0;
		private Timer timer = new Timer();
		private boolean _visible = false;
		String _adUnitID;
		private void createInterstitial()
		{
			_interstitial = new InterstitialAd(Cocos2dxHelper.getActivity());
			_interstitial.setAdUnitId(_adUnitID);
			_interstitial.setAdListener(this);
			_interstitial.loadAd(createRequest());
		}
		public AdmobInterstitialAd(String adUnitID, long pointer)
		{
			_adUnitID = adUnitID;
			createInterstitial();
		}
		public void done()
		{
			_interstitial.setAdListener(null);
			_pointer = 0;
			timer.cancel();
		}
		@Override
		public void onAdLoaded()
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateInterstitialDidReceiveAd(lObject);
					}
				});
			}
		}
		@Override
		public void onAdFailedToLoad(int errorCode)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        	activity.runOnUiThread(new Runnable() {
		                @Override
		                public void run() {
		                	_interstitial.loadAd(createRequest());
		                }
		            });
		        }
		    };

		    timer.schedule(action, 10000);
			
		    if(_pointer != 0)
		    {
		    	int error = kAdsErrorInternalError;
		    	String description = "Internal error";
		    	switch(errorCode)
		    	{
		    	case AdRequest.ERROR_CODE_INTERNAL_ERROR:
		    		error = kAdsErrorInternalError;
		    		description = "Internal error";
		    		break;
		    	case AdRequest.ERROR_CODE_INVALID_REQUEST:
		    		error = kAdsErrorInvalidRequest;
		    		description = "Invalid request";
		    		break;
		    	case AdRequest.ERROR_CODE_NETWORK_ERROR:
		    		error = kAdsErrorNetworkError;
		    		description = "Network error";
		    		break;
		    	case AdRequest.ERROR_CODE_NO_FILL:
		    		error = kAdsErrorNoFill;
		    		description = "No fill";
		    		break;
		    	}
		    	final long lObject = _pointer;
		    	final int lCode = error;
		    	final int lNativeCode = errorCode;
		    	final String lDescription = description;

		    	Cocos2dxHelper.runOnGLThread(new Runnable() {
		    		@Override
		    		public void run() {
		    			AdMobHelper.delegateInterstitialDidFailToReceiveAd(lObject, lCode, lNativeCode, lDescription);
		    		}
		    	});
		    }
		}
		@Override
		public void onAdOpened()
		{
			_visible = true;
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateInterstitialWillPresentScreen(lObject);
					}
				});
			}
		}
		@Override
		public void onAdClosed()
		{
			_visible = false;
			_interstitial.setAdListener(null);;
			createInterstitial();
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateInterstitialWillDismissScreen(lObject);
					}
				});
			}
		}
		@Override
		public void onAdLeftApplication()
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateInterstitialWillLeaveApplication(lObject);
					}
				});
			}
		}
		public boolean isReady()
		{
			return _interstitial.isLoaded();
		}
		public boolean isVisible()
		{
			return _visible;
		}
		public boolean show()
		{
			if(!_interstitial.isLoaded())
				return false;
			_interstitial.show();
			return true;
		}
	};
	
	public static class AdmobBanner extends AdListener
	{
		private AdView _banner;
		private boolean _ready = false;
		private long _pointer = 0;
		private Timer timer = new Timer();
		private static RelativeLayout _layout = null;
		private RelativeLayout _bannerLayout = null;
		
		final private static int kGADAdSizeInvalid = 0;
		final private static int kGADAdSizeBanner = 1;
		final private static int kGADAdSizeMediumRectangle = 2;
		final private static int kGADAdSizeFullBanner = 3;
		final private static int kGADAdSizeLeaderboard = 4;
		final private static int kGADAdSizeSkyscraper = 5;
		final private static int kGADAdSizeSmartBannerPortrait = 6;
		final private static int kGADAdSizeSmartBannerLandscape = 7;
		
		private static final int kBannerGravityTopLeft = 0;
		private static final int kBannerGravityCenterLeft = 1;
		private static final int kBannerGravityBottomLeft = 2;
		private static final int kBannerGravityTopCenter = 3;
		private static final int kBannerGravityCenter = 4;
		private static final int kBannerGravityBottomCenter = 5;
		private static final int kBannerGravityTopRight = 6;
		private static final int kBannerGravityCenterRight = 7;
		private static final int kBannerGravityBottomRight = 8;
		
		public AdmobBanner(String adUnitID, int size, long pointer)
		{
			_pointer = pointer;
			AdSize bannerSize = AdSize.BANNER;
			switch(size)
			{
				case kGADAdSizeBanner:
					bannerSize = AdSize.BANNER;
					break;
				case kGADAdSizeMediumRectangle:
					bannerSize = AdSize.MEDIUM_RECTANGLE;
					break;
				case kGADAdSizeFullBanner:
					bannerSize = AdSize.FULL_BANNER;
					break;
				case kGADAdSizeLeaderboard:
					bannerSize = AdSize.LEADERBOARD;
					break;
				case kGADAdSizeSkyscraper:
					bannerSize = AdSize.WIDE_SKYSCRAPER;
					break;
				case kGADAdSizeSmartBannerPortrait:
					bannerSize = AdSize.SMART_BANNER;
					break;
				case kGADAdSizeSmartBannerLandscape:
					bannerSize = AdSize.SMART_BANNER;
					break;
				case kGADAdSizeInvalid:
				default:
					break;
			}
			
			_banner = new AdView(activity);
			_banner.setAdUnitId(adUnitID);
			_banner.setAdSize(bannerSize);
			_banner.setAdListener(this);
			_banner.loadAd(createRequest());
		}
		public void done()
		{
			_banner.setAdListener(null); 
			_pointer = 0;
			timer.cancel();
		}
		@Override
		public void onAdLoaded()
		{
			_ready = true;
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateBannerDidReceiveAd(lObject);
					}
				});
			}
		}
		@Override
		public void onAdFailedToLoad(int errorCode)
		{
			timer.cancel(); //this will cancel the current task. if there is no active task, nothing happens
		    timer = new Timer();

		    TimerTask action = new TimerTask() {
		        public void run() {
		        	activity.runOnUiThread(new Runnable() {
		                @Override
		                public void run() {
		                	_banner.loadAd(createRequest());
		                }
		            });
		        }
		    };

		    timer.schedule(action, 10000);
		    
		    _ready = false;
			
		    if(_pointer != 0)
		    {
		    	int error = kAdsErrorInternalError;
		    	String description = "Internal error";
		    	switch(errorCode)
		    	{
		    	case AdRequest.ERROR_CODE_INTERNAL_ERROR:
		    		error = kAdsErrorInternalError;
		    		description = "Internal error";
		    		break;
		    	case AdRequest.ERROR_CODE_INVALID_REQUEST:
		    		error = kAdsErrorInvalidRequest;
		    		description = "Invalid request";
		    		break;
		    	case AdRequest.ERROR_CODE_NETWORK_ERROR:
		    		error = kAdsErrorNetworkError;
		    		description = "Network error";
		    		break;
		    	case AdRequest.ERROR_CODE_NO_FILL:
		    		error = kAdsErrorNoFill;
		    		description = "No fill";
		    		break;
		    	}
		    	final long lObject = _pointer;
		    	final int lCode = error;
		    	final int lNativeCode = errorCode;
		    	final String lDescription = description;

		    	Cocos2dxHelper.runOnGLThread(new Runnable() {
		    		@Override
		    		public void run() {
		    			AdMobHelper.delegateBannerDidFailToReceiveAd(lObject, lCode, lNativeCode, lDescription);
		    		}
		    	});
		    }
		}
		@Override
		public void onAdOpened()
		{
			//_visible = true;
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateInterstitialWillPresentScreen(lObject);
					}
				});
			}
		}
		@Override
		public void onAdClosed()
		{
			//_visible = false;
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateBannerWillDismissScreen(lObject);
					}
				});
			}
		}
		@Override
		public void onAdLeftApplication()
		{
			if(_pointer != 0)
			{
				final long lObject = _pointer;
				Cocos2dxHelper.runOnGLThread(new Runnable() {
					@Override
					public void run() {
						AdMobHelper.delegateBannerWillLeaveApplication(lObject);
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
				
				_bannerLayout = new RelativeLayout(activity);
				RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(width, height);
				params.leftMargin = x;
				params.topMargin = parent.getHeight() - y - height;
				_layout.addView(_bannerLayout, params);
				
				switch (gravity) {
					case kBannerGravityCenterLeft:
						_bannerLayout.setGravity(Gravity.CENTER_VERTICAL|Gravity.LEFT);
						break;
					case kBannerGravityBottomLeft:
						_bannerLayout.setGravity(Gravity.BOTTOM|Gravity.LEFT); 
						break;
					case kBannerGravityTopCenter:
						_bannerLayout.setGravity(Gravity.TOP|Gravity.CENTER_HORIZONTAL); 
						break;
					case kBannerGravityCenter:
						_bannerLayout.setGravity(Gravity.CENTER);
						break;
					case kBannerGravityBottomCenter:
						_bannerLayout.setGravity(Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL);
						break;
					case kBannerGravityTopRight:
						_bannerLayout.setGravity(Gravity.TOP|Gravity.RIGHT);
						break;
					case kBannerGravityCenterRight:
						_bannerLayout.setGravity(Gravity.CENTER_VERTICAL|Gravity.RIGHT);
						break;
					case kBannerGravityBottomRight:
						_bannerLayout.setGravity(Gravity.BOTTOM|Gravity.RIGHT);
						break;
					case kBannerGravityTopLeft:
					default:
						_bannerLayout.setGravity(Gravity.TOP|Gravity.LEFT);
						break;
				}
				
				_bannerLayout.addView(_banner, new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
				return true;
			}
			return false;
		}
		public boolean hide()
		{
			if(_layout != null && _bannerLayout!=null && _banner.getParent() != null)
			{
				_bannerLayout.removeView(_banner);
				_layout.removeView(_bannerLayout);
				_bannerLayout = null;
				return true;
			}
			return false;
		}
	};
	
	public static void setKeyWords(String keyWords[])
	{
		m_keyWords = keyWords;
	}
	
	public static void setTestDevices(String devices[])
	{
		m_testDevices = devices;
	}
	
	public static void setAdNetworkExtras(int network, Map<String,String> extras)
	{
		if(network == 0)
		{
			Bundle bundle = new Bundle();
			for (Map.Entry<String, String> entry : extras.entrySet())
			{
				bundle.putString(entry.getKey(),entry.getValue());
			}
			m_networkExtras = new AdMobExtras(bundle);
		}
	}
	public static void setBirthday(int year, int month, int day)
	{
		Calendar cal = GregorianCalendar.getInstance();
		cal.set(year, month, day);
		m_Birthday = cal.getTime();
	}
	
	public static void setGender(int gender)
	{
		m_Gender = gender;
	}
	
	public static void setLocation(float latitude, float longitude, float accuracyInMeters)
	{
		m_Location = new Location("custom");
		m_Location.setLatitude(latitude);
		m_Location.setLongitude(longitude);
		m_Location.setAccuracy(accuracyInMeters);
	}
	
	public static void setTagForChildDirectedTreatment(boolean tag)
	{
		m_TagForChildDirectedTreatment = tag;
	}
	
	private static AdRequest createRequest()
	{
		AdRequest.Builder builder = new AdRequest.Builder();
		if(m_networkExtras != null)
			builder.addNetworkExtras(m_networkExtras);
		if(m_keyWords != null)
			for (String keyWord : m_keyWords)
				builder.addKeyword(keyWord);
		if(m_testDevices != null)
			for (String testDevice : m_testDevices)
				builder.addTestDevice(testDevice);
		builder.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);
		if(m_Birthday != null)
			builder.setBirthday(m_Birthday);
		builder.setGender(m_Gender);
		if(m_Location != null)
			builder.setLocation(m_Location);
		if(m_TagForChildDirectedTreatment != null)
			builder.tagForChildDirectedTreatment(m_TagForChildDirectedTreatment);
		return builder.build(); 
	}
	
	public static AdmobInterstitialAd createInterstitial(String adUnitID, long pointer)
	{
		final String curAdUnitID = adUnitID;
		final long curPointer = pointer;
		
		FutureTask<AdmobInterstitialAd> task = new FutureTask<AdmobInterstitialAd>(new Callable<AdmobInterstitialAd>() {
            @Override
            public AdmobInterstitialAd call() {
            	return new AdmobInterstitialAd(curAdUnitID, curPointer);
            }
        });
		
		activity.runOnUiThread(task);
		
		AdmobInterstitialAd ret = null;
		
		try{
			ret = task.get();
        }
		catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static void interstitialDone(AdmobInterstitialAd interstitial)
	{
		final AdmobInterstitialAd curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.done();
			}
		});
	}
	
	public static boolean interstitialShow(AdmobInterstitialAd interstitial)
	{
		final AdmobInterstitialAd curInterstitial = interstitial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
            	return curInterstitial.show();
            }
        });
		
		activity.runOnUiThread(task);
		
		boolean ret = false;
		
		try{
			ret =  task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static boolean interstitialIsReady(AdmobInterstitialAd intersititial)
	{
		final AdmobInterstitialAd curInterstitial = intersititial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
            	return curInterstitial.isReady();
            }
        });
		
		activity.runOnUiThread(task);
		
		boolean ret = false;
		
		try{
			ret =  task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static boolean interstitialIsVisible(AdmobInterstitialAd intersititial)
	{
		final AdmobInterstitialAd curInterstitial = intersititial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
            	return curInterstitial.isVisible();
            }
        });
		
		activity.runOnUiThread(task);
		
		boolean ret = false;
		
		try{
			ret =  task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	public static AdmobBanner createBanner(String adUnitID, int size, long pointer)
	{
		final String curAdUnitID = adUnitID;
		final int curSize = size;
		final long curPointer = pointer;
		
		FutureTask<AdmobBanner> task = new FutureTask<AdmobBanner>(new Callable<AdmobBanner>() {
            @Override
            public AdmobBanner call() {
            	return new AdmobBanner(curAdUnitID, curSize, curPointer);
            }
        });
		
		activity.runOnUiThread(task);
		
		AdmobBanner ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	
	
	public static void bannerDone(AdmobBanner banner)
	{
		final AdmobBanner curBanner = banner;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curBanner.done();
			}
		});
	}
	
	public static boolean bannerShow(AdmobBanner banner, int x, int y, int width, int height, int scaleType, int gravity)
	{
		final AdmobBanner curBanner = banner;
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
	
	public static boolean bannerHide(AdmobBanner banner)
	{
		final AdmobBanner curBanner = banner;
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
	
	public static boolean bannerIsReady(AdmobBanner banner)
	{
		final AdmobBanner curBanner = banner;
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
	
	public static boolean bannerIsVisible(AdmobBanner banner)
	{
		final AdmobBanner curBanner = banner;
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
		try {
			return activity.getPackageManager().getPackageInfo("com.google.android.gms", 0 ).versionName;
		} catch (NameNotFoundException e) {
			return "unknown";
		}
	}
}
