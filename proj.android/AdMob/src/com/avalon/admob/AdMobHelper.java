/*
 * Copyright (c) 2013 Daniel Rakos
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
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import android.app.Activity;
import android.location.Location;
import android.os.Bundle;
import android.widget.AbsoluteLayout;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.mediation.NetworkExtras;
import com.google.android.gms.ads.mediation.admob.AdMobExtras;

import org.cocos2dx.lib.Cocos2dxHelper;

@SuppressWarnings("deprecation")
public abstract class AdMobHelper
{
	private static LinearLayout m_adsLayout = null;
	private static AbsoluteLayout m_BannerView = null;
	private static final Activity activity = Cocos2dxHelper.getActivity();
	
	private static String m_keyWords[] = null;
	private static String m_testDevices[] = null;
	private static NetworkExtras m_networkExtras = null;
	private static Date m_Birthday = null;
	private static int m_Gender = 0;
	private static Location m_Location;
	private static Boolean m_TagForChildDirectedTreatment = null;
	
	public static native void delegateAdViewDidReceiveAd(long object);
	public static native void delegateAdViewDidFailToReceiveAd(long object, String error);
	public static native void delegateAdViewWillPresentScreen(long object);
	public static native void delegateAdViewWillDismissScreen(long object);
	public static native void delegateAdViewWillLeaveApplication(long object);

	public static native void delegateInterstitialDidReceiveAd(long object);
	public static native void delegateInterstitialDidFailToReceiveAd(long object, String error);
	public static native void delegateInterstitialWillPresentScreen(long object);
	public static native void delegateInterstitialWillDismissScreen(long object);
	public static native void delegateInterstitialWillLeaveApplication(long object);
	
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
	
	public static void adViewReceiveAd(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateAdViewDidReceiveAd(lObject);
			}
		});
    }
	
	public static void adViewDidFailToReceive(long object, String error)
    {
    	final long lObject = object;
    	final String lError = error;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateAdViewDidFailToReceiveAd(lObject, lError);
			}
		});
    }
	
	public static void adViewWillPresentScreen(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateAdViewWillPresentScreen(lObject);
			}
		});
    }
	
	public static void adViewWillDismissScreen(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateAdViewWillDismissScreen(lObject);
			}
		});
    }
	
	public static void adViewWillLeaveApplication(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateAdViewWillLeaveApplication(lObject);
			}
		});
    }
	
	public static void interstitialReceiveAd(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateInterstitialDidReceiveAd(lObject);
			}
		});
    }
	
	public static void interstitialDidFailToReceive(long object, String error)
    {
    	final long lObject = object;
    	final String lError = error;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateInterstitialDidFailToReceiveAd(lObject, lError);
			}
		});
    }
	
	public static void interstitialWillPresentScreen(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateInterstitialWillPresentScreen(lObject);
			}
		});
    }
	
	public static void interstitialWillDismissScreen(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateInterstitialWillDismissScreen(lObject);
			}
		});
    }
	
	public static void interstitialWillLeaveApplication(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				AdMobHelper.delegateInterstitialWillLeaveApplication(lObject);
			}
		});
    }
	
	public static class adViewAdListener extends AdListener {
		private long pointer;
		public adViewAdListener(long value)
		{
			pointer = value;
		}
		public void onAdLoaded()
		{
			AdMobHelper.adViewReceiveAd(pointer);
		}
		public void onAdFailedToLoad(int errorCode)
		{
			String error = "";
			switch(errorCode)
			{
				case AdRequest.ERROR_CODE_INTERNAL_ERROR:
					error = "ERROR_CODE_INTERNAL_ERROR";
					break;
				case AdRequest.ERROR_CODE_INVALID_REQUEST:
					error = "ERROR_CODE_INVALID_REQUEST";
					break;
				case AdRequest.ERROR_CODE_NETWORK_ERROR:
					error = "ERROR_CODE_NETWORK_ERROR";
					break;
				case AdRequest.ERROR_CODE_NO_FILL:
					error = "ERROR_CODE_NO_FILL";
					break;
			}
			AdMobHelper.adViewDidFailToReceive(pointer, error);
		}
		public void onAdOpened()
		{
			AdMobHelper.adViewWillPresentScreen(pointer);
		}
		public void onAdClosed()
		{
			AdMobHelper.adViewWillDismissScreen(pointer);
		}
		public void onAdLeftApplication()
		{
			AdMobHelper.adViewWillLeaveApplication(pointer);
		}
	}
	
	public static class interstitialListener extends AdListener {
		private long pointer;
		public interstitialListener(long value)
		{
			pointer = value;
		}
		@Override
		protected void finalize( ) throws Throwable 
		{ 
			System.out.println("d1 object is not eligible for garbage collection and is still active");  
		}
		@Override
		public void onAdLoaded()
		{
			AdMobHelper.interstitialReceiveAd(pointer);
		}
		@Override
		public void onAdFailedToLoad(int errorCode)
		{
			String error = "";
			switch(errorCode)
			{
				case AdRequest.ERROR_CODE_INTERNAL_ERROR:
					error = "ERROR_CODE_INTERNAL_ERROR";
					break;
				case AdRequest.ERROR_CODE_INVALID_REQUEST:
					error = "ERROR_CODE_INVALID_REQUEST";
					break;
				case AdRequest.ERROR_CODE_NETWORK_ERROR:
					error = "ERROR_CODE_NETWORK_ERROR";
					break;
				case AdRequest.ERROR_CODE_NO_FILL:
					error = "ERROR_CODE_NO_FILL";
					break;
			}
			AdMobHelper.interstitialDidFailToReceive(pointer, error);
		}
		@Override
		public void onAdOpened()
		{
			AdMobHelper.interstitialWillPresentScreen(pointer);
		}
		@Override
		public void onAdClosed()
		{
			AdMobHelper.interstitialWillDismissScreen(pointer);
		}
		@Override
		public void onAdLeftApplication()
		{
			AdMobHelper.interstitialWillLeaveApplication(pointer);
		}
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
	
	public static void addInterstitialDelegate(InterstitialAd intersititial, long value)
	{
		final InterstitialAd curIntersititial = intersititial;
		final long curValue = value;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curIntersititial.setAdListener(new AdMobHelper.interstitialListener(curValue));
			}
		});
	}
	
	public static InterstitialAd createInterstitial(String adUnitID)
	{
		final String curAdUnitID = adUnitID;
		
		FutureTask<InterstitialAd> task = new FutureTask<InterstitialAd>(new Callable<InterstitialAd>() {
            @Override
            public InterstitialAd call() {
            	InterstitialAd interstitial = new InterstitialAd(Cocos2dxHelper.getActivity());
        		interstitial.setAdUnitId(curAdUnitID);

        	    interstitial.loadAd(createRequest());
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
	
	public static boolean isInterstitialReady(InterstitialAd intersititial)
	{
		final InterstitialAd curInterstitial = intersititial;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
            	return curInterstitial.isLoaded();
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
	
	public static void showInterstitial(InterstitialAd intersititial)
	{
		final InterstitialAd curInterstitial = intersititial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.show();
			}
		});
	}
	
	public static void addAdViewDelegate(AdView adView, long value)
	{
		adView.setAdListener(new AdMobHelper.adViewAdListener(value));
	}
	
	public static AdView createBannerView(String adUnitID, int size, int x, int y, int width, int height)
	{
		final String curAdUnitID = adUnitID;
		final int curX = x;
		final int curY = y;
		final int curWidth = width;
		final int curHeight = height;
		AdSize tempSize = AdSize.BANNER;
		switch(size)
		{
			case 1:
				tempSize = AdSize.BANNER;
				break;
			case 2:
				tempSize = AdSize.MEDIUM_RECTANGLE;
				break;
			case 3:
				tempSize = AdSize.FULL_BANNER;
				break;
			case 4:
				tempSize = AdSize.LEADERBOARD;
				break;
			case 5:
				tempSize = AdSize.WIDE_SKYSCRAPER;
				break;
			case 6:
				tempSize = AdSize.SMART_BANNER;
				break;
			case 7:
				tempSize = AdSize.SMART_BANNER;
				break;
			default:
				break;
		}
		final AdSize curAdSize = tempSize;
		
		FutureTask<AdView> task = new FutureTask<AdView>(new Callable<AdView>() {
            @Override
            public AdView call() {
            	if (m_adsLayout == null) {
					m_adsLayout = new LinearLayout(activity);
					activity.addContentView(m_adsLayout, new LayoutParams(
							LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
				}

				if (null != m_BannerView) {
					m_adsLayout.removeView(m_BannerView);
					m_BannerView = null;
				}
				m_BannerView = new AbsoluteLayout(activity);
				m_adsLayout.addView(m_BannerView);

				LinearLayout.LayoutParams linearParams = (LinearLayout.LayoutParams) m_BannerView
						.getLayoutParams();
				linearParams.leftMargin = curX;
				linearParams.topMargin = curY;
				linearParams.width = curWidth;
				linearParams.height = curHeight;
				m_BannerView.setLayoutParams(linearParams);
				
				AdView adView = new AdView(activity);
				adView.setAdUnitId(curAdUnitID);
			    adView.setAdSize(curAdSize);
			    
			    m_BannerView.addView(adView);
			    
			    adView.loadAd(createRequest());
				return adView;
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
}
