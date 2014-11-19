package com.avalon.adgeneration;

import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import android.app.Activity;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.RelativeLayout;

import com.socdm.d.adgeneration.ADG;
import com.socdm.d.adgeneration.ADG.AdFrameSize;
import com.socdm.d.adgeneration.ADGListener;

import org.cocos2dx.lib.Cocos2dxHelper;

public class ADGBanner{
	private static final Activity activity = Cocos2dxHelper.getActivity();
	
	private ADG m_banner = null;
	private static RelativeLayout m_layout = null;
	
	final private static int kADGSP = 0;
	final private static int kADGLARGE= 1;
	final private static int kADGRECT = 2;
	final private static int kADTABLET = 3;
	
	public static native void delegateOnReceiveAd(long object);
	public static native void delegateOnFailedToReceiveAd(long object);
	public static native void delegateOnInternalBrowserOpen(long object);
	public static native void delegateOnInternalBrowserClose(long object);
	
	public static void onReceiveAd(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				ADGBanner.delegateOnReceiveAd(lObject);
			}
		});
    }
	
	public static void onFailedToReceiveAd(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				ADGBanner.delegateOnFailedToReceiveAd(lObject);
			}
		});
    }
	
	public static void onInternalBrowserOpen(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				ADGBanner.delegateOnInternalBrowserOpen(lObject);
			}
		});
    }
	
	public static void onInternalBrowserClose(long object)
    {
    	final long lObject = object;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				ADGBanner.delegateOnInternalBrowserClose(lObject);
			}
		});
    }
	
	public static class adViewADGListener extends ADGListener {
		private long pointer;
		public adViewADGListener(long value)
		{
			pointer = value;
		}
		
		@Override
		public void onReceiveAd()
		{
			ADGBanner.onReceiveAd(pointer);
		}

		@Override
		public void onFailedToReceiveAd()
		{
			ADGBanner.onFailedToReceiveAd(pointer);
		}

		@Override
		public void onInternalBrowserOpen()
		{
			ADGBanner.onInternalBrowserOpen(pointer);
		}

		@Override
		public void onInternalBrowserClose()
		{
			ADGBanner.onInternalBrowserClose(pointer);
		}
	}
	
	private ADGBanner(String adUnitID, AdFrameSize size, long delegate)
	{
		m_banner = new ADG(activity);
		m_banner.setLocationId(adUnitID);
		m_banner.setAdFrameSize(size);
		if(delegate != 0)
		{
			m_banner.setAdListener(new ADGBanner.adViewADGListener(delegate));
		}
	}
	
	private void show(int x, int y, int width, int height)
	{
		if(m_layout == null)
		{
			m_layout = new RelativeLayout(activity);
			activity.addContentView(m_layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
			m_layout.setGravity(Gravity.TOP|Gravity.LEFT);
		}
		if(m_banner.getParent() == null)
		{
			RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(width, height);
			params.leftMargin = x;
			params.topMargin = m_layout.getHeight() - y - height;
			m_layout.addView(m_banner, params);
		}
	}
	private void hide()
	{
		if(m_layout != null && m_banner.getParent() != null)
		{
			m_layout.removeView(m_banner);
		}
	}
	
	public static ADGBanner createBannerView(String adUnitID, int size, long delegate)
	{
		final String curAdUnitID = adUnitID;
		AdFrameSize tempSize = AdFrameSize.SP;
		final long curDelegate = delegate;
		switch(size)
		{
			case kADGSP:
				tempSize = AdFrameSize.SP;
				break;
			case kADGLARGE:
				tempSize = AdFrameSize.LARGE;
				break;
			case kADGRECT:
				tempSize = AdFrameSize.RECT;
				break;
			case kADTABLET:
				tempSize = AdFrameSize.TABLET;
				break;
			default:
				tempSize = AdFrameSize.FREE;
				float DP = activity.getResources().getDisplayMetrics().density;
				tempSize.setSize((int)((size>>16) / DP), (int)((size & 0xFFFF) / DP));
				break;
		}
		final AdFrameSize curAdSize = tempSize;
		
		FutureTask<ADGBanner> task = new FutureTask<ADGBanner>(new Callable<ADGBanner>() {
            @Override
            public ADGBanner call() {
				return new ADGBanner(curAdUnitID, curAdSize, curDelegate);
            }
        });
		
		activity.runOnUiThread(task);
		
		ADGBanner ret = null;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
	public static void showBanner(ADGBanner banner, int x, int y, int width, int height)
	{
		final ADGBanner curBanner = banner;
		final int curX = x;
		final int curY = y;
		final int curWidth = width;
		final int curHeight = height;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curBanner.show(curX, curY, curWidth, curHeight);
			}
		});
	}
	public static void hideBanner(ADGBanner banner)
	{
		final ADGBanner curBanner = banner;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				curBanner.hide();
			}
		});
	}
	
	public static boolean isVisible(ADGBanner banner)
	{
		final ADGBanner curBanner = banner;
		FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
				return curBanner.m_banner.getVisibility() == View.VISIBLE;
            }
        });
		
        activity.runOnUiThread(task);
		
        Boolean ret = false;
		
		try{
			ret = task.get();
        }catch(Exception e){
        	
        }
		
		return ret;
	}
}