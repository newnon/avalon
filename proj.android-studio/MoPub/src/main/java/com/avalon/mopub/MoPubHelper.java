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

package com.avalon.mopub;

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import android.app.Activity;
import android.location.Location;
import android.os.Bundle;
import android.os.Looper;
import android.widget.AbsoluteLayout;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.mopub.mobileads.MoPubErrorCode;
import com.mopub.mobileads.MoPubInterstitial;
import com.mopub.mobileads.MoPubInterstitial.InterstitialAdListener;

import org.cocos2dx.lib.Cocos2dxHelper;

@SuppressWarnings("deprecation")
public abstract class MoPubHelper
{
	private static LinearLayout m_adsLayout = null;
	private static AbsoluteLayout m_BannerView = null;
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static HashMap<MoPubInterstitial,Long> _interstitials = new HashMap<MoPubInterstitial,Long>();
	
	public static native void delegateInterstitialLoaded(long object);
	public static native void delegateInterstitialFailed(long object, int error);
	public static native void delegateInterstitialShown(long object);
	public static native void delegateInterstitialClicked(long object);
	public static native void delegateInterstitialDismissed(long object);
	
    public static void onInterstitialLoaded(MoPubInterstitial interstitial)
    {
    	Long object = _interstitials.get(interstitial);
    	if(object == null)
    		return;
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				MoPubHelper.delegateInterstitialLoaded(lObject);
			}
		});
    }
    public static void onInterstitialFailed(MoPubInterstitial interstitial, MoPubErrorCode errorCode)
    {
    	Long object = _interstitials.get(interstitial);
    	if(object == null)
    		return;
    	final long lObject = object;
    	final int lErrorCode = errorCode.ordinal();
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				MoPubHelper.delegateInterstitialFailed(lObject,lErrorCode);
			}
		});
    }
    public static void onInterstitialShown(MoPubInterstitial interstitial)
    {
    	Long object = _interstitials.get(interstitial);
    	if(object == null)
    		return;
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				MoPubHelper.delegateInterstitialShown(lObject);
			}
		});
    }
    public static void onInterstitialClicked(MoPubInterstitial interstitial)
    {
    	Long object = _interstitials.get(interstitial);
    	if(object == null)
    		return;
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				MoPubHelper.delegateInterstitialClicked(lObject);
			}
		});
    }
    public static void onInterstitialDismissed(MoPubInterstitial interstitial)
    {
    	Long object = _interstitials.get(interstitial);
    	if(object == null)
    		return;
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				MoPubHelper.delegateInterstitialDismissed(lObject);
			}
		});
    }
	
	public static MoPubInterstitial createInterstitial(String adUnitID)
	{
		final String curAdUnitID = adUnitID;
		
		FutureTask<MoPubInterstitial> task = new FutureTask<MoPubInterstitial>(new Callable<MoPubInterstitial>() {
            @Override
            public MoPubInterstitial call() {
            	MoPubInterstitial interstitial = new MoPubInterstitial(Cocos2dxHelper.getActivity(),curAdUnitID);
        		return interstitial;
            }
        });
		
		activity.runOnUiThread(task);
		
		MoPubInterstitial ret = null;
		
		try{
			ret = task.get();
        }
		catch(Exception e){
        	
        }
		
		return ret;
	}
	
	static public void setDelegate(MoPubInterstitial interstitial, long reference)
	{
		
		final MoPubInterstitial curIntersititial = interstitial;
		final long curReference = reference;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				_interstitials.put(curIntersititial, curReference);
				curIntersititial.setInterstitialAdListener(new InterstitialAdListener() {
			        public void onInterstitialLoaded(MoPubInterstitial interstitial)
			        {
			        	MoPubHelper.onInterstitialLoaded(interstitial);
			        }
			        public void onInterstitialFailed(MoPubInterstitial interstitial, MoPubErrorCode errorCode)
			        {
			        	MoPubHelper.onInterstitialFailed(interstitial,errorCode);
			        }
			        public void onInterstitialShown(MoPubInterstitial interstitial)
			        {
			        	MoPubHelper.onInterstitialShown(interstitial);
			        }
			        public void onInterstitialClicked(MoPubInterstitial interstitial)
			        {
			        	MoPubHelper.onInterstitialClicked(interstitial);
			        }
			        public void onInterstitialDismissed(MoPubInterstitial interstitial)
			        {
			        	MoPubHelper.onInterstitialDismissed(interstitial);
			        }
			    });
			}
		});
	}
	
	static public void setKeywords(MoPubInterstitial interstitial, String keywords)
	{
		
		final String curKeywords = keywords;
		final MoPubInterstitial curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.setKeywords(curKeywords);
			}
		});
	}
	
	static public void setTesting(MoPubInterstitial interstitial, boolean testing)
	{
		
		final boolean curtTesting = testing;
		final MoPubInterstitial curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.setTesting(curtTesting);
			}
		});
	}
	static public void loadInterstitial(MoPubInterstitial interstitial)
	{
		
		final MoPubInterstitial curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.load();
			}
		});
	}
	static public void showInterstitial(MoPubInterstitial interstitial)
	{
		
		final MoPubInterstitial curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curInterstitial.show();
			}
		});
	}
	static public boolean getInterstitialReady(MoPubInterstitial interstitial)
	{
		return interstitial.isReady();
	}
	
	static public void RemoveInterstitial(MoPubInterstitial interstitial)
	{
		
		final MoPubInterstitial curInterstitial = interstitial;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				_interstitials.remove(curInterstitial);
			}
		});
	}
	
}
