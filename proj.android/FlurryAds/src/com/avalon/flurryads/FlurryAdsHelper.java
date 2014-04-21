package com.avalon.flurryads;

import java.util.HashSet;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.widget.AbsoluteLayout;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.flurry.android.FlurryAdSize;
import com.flurry.android.FlurryAds;
import com.flurry.android.FlurryAdListener;
import com.flurry.android.FlurryAdType;

@SuppressWarnings("deprecation")
public abstract class FlurryAdsHelper {
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static LinearLayout m_adsLayout = null;
	private static AbsoluteLayout mBannerView = null;
	private static boolean delegateEnabled = false;
	private static FlurryAdListener listener = null;
	private static HashSet<String> needToDisplay = new HashSet<String>();
	
    public static native boolean delegateShouldDisplayAd(String adSpace, boolean interstitial);
    public static native void delegateOnAdClosed(String adSpace, boolean interstitial);
    public static native void delegateOnApplicationExit(String adSpace);
    public static native void delegateOnRendered(String adSpace);
    public static native void delegateOnRenderFailed(String adSpace, String error);
    public static native void delegateOnDidReceiveAd(String adSpace);
    public static native void delegateOnDidFailToReceiveAd(String adSpace, String error);
    public static native void delegateOnAdClicked(String adSpace);
    public static native void delegateOnAdOpened(String adSpace);
    public static native void delegateOnVideoCompleted(String adSpace);
	
    public static boolean shouldDisplayAd(String adSpace, boolean interstitial)
    {
    	if(!delegateEnabled)
    		return true;
    	
    	final String lSpace = adSpace;
    	final boolean lInterstitial = interstitial;
    	
    	FutureTask<Boolean> task = new FutureTask<Boolean>(new Callable<Boolean>() {
            @Override
            public Boolean call() {
                return FlurryAdsHelper.delegateShouldDisplayAd(lSpace, lInterstitial);
            }
        });
    	
		Cocos2dxHelper.runOnGLThread(task);
		
		boolean ret = true;
		
		try{
			ret = task.get();
        }catch(Exception e){

        }
		
		return ret;
    }
    public static void onAdClosed(String adSpace, boolean interstitial)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	final boolean lInterstitial = interstitial;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnAdClosed(lSpace,lInterstitial);
			}
		});
    }
    public static void onApplicationExit(String adSpace)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnApplicationExit(lSpace);
			}
		});
    }
    public static void onRendered(String adSpace)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnRendered(lSpace);
			}
		});
    }
    public static void onRenderFailed(String adSpace, String error)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	final String lError = error;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnRenderFailed(lSpace, lError);
			}
		});
    }
    public static void onDidReceiveAd(String adSpace)
    {
    	
    	if(FlurryAdsHelper.needToDisplay.contains(adSpace))
    	{
    		FlurryAdsHelper.needToDisplay.remove(adSpace);
    	    FlurryAds.displayAd(activity, adSpace, mBannerView);
    	}
    	
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnDidReceiveAd(lSpace);
			}
		});
    }
    public static void onDidFailToReceiveAd(String adSpace, String error)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	final String lError = error;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnDidFailToReceiveAd(lSpace, lError);
			}
		});
    }
    public static void onAdClicked(String adSpace)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnAdClicked(lSpace);
			}
		});
    }
    public static void onAdOpened(String adSpace)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnAdOpened(lSpace);
			}
		});
    }
    public static void onVideoCompleted(String adSpace)
    {
    	if(!delegateEnabled)
    		return;
    	
    	final String lSpace = adSpace;
    	
		Cocos2dxHelper.runOnGLThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.delegateOnVideoCompleted(lSpace);
			}
		});
    }
	
	public static void fetchAdForSpace(String space, int x,int y,int width,int heignt, int size)
    {
		final String curSpace = space;
		final int curX = x;
		final int curY = y;
		final int curWidth = width;
		final int curHeight = heignt;
		final FlurryAdSize curSize = FlurryAdSize.values()[size-1];
		
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (m_adsLayout == null) {
					m_adsLayout = new LinearLayout(activity);
					activity.addContentView(m_adsLayout, new LayoutParams(
							LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
				}

				if (null != mBannerView) {
					m_adsLayout.removeView(mBannerView);
					mBannerView = null;
				}
				mBannerView = new AbsoluteLayout(activity);
				m_adsLayout.addView(mBannerView);

				LinearLayout.LayoutParams linearParams = (LinearLayout.LayoutParams) mBannerView
						.getLayoutParams();
				linearParams.leftMargin = curX;
				linearParams.topMargin = curY;
				linearParams.width = curWidth;
				linearParams.height = curHeight;
				mBannerView.setLayoutParams(linearParams);
				FlurryAdsHelper.needToDisplay.remove(curSpace);
				FlurryAds.fetchAd(activity, curSpace,
						mBannerView, curSize);
			}
		});
	}
	
	public static boolean adReadyForSpace(String space)
	{
		return FlurryAds.isAdReady(space);
	}
	
	public static void displayAdForSpace(String space)
	{
		final String curSpace = space;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				FlurryAdsHelper.needToDisplay.remove(curSpace);
				if (null == mBannerView) {
					return;
				}
				FlurryAds.displayAd(activity, curSpace,
						mBannerView);
			}
		});
	}
    

	public static void displayAdForSpaceModally(String space)
    {
		final String curSpace = space;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (null == mBannerView) {
					return;
				}
				FlurryAds.displayAd(activity, curSpace,
						mBannerView);
			}
		});
    }
   
	public static void fetchAndDisplayAdForSpace(String space, int x,int y,int width,int heignt, int size)
    {
		final String curSpace = space;
		final int curX = x;
		final int curY = y;
		final int curWidth = width;
		final int curHeight = heignt;
		final FlurryAdSize curSize = FlurryAdSize.values()[size-1];
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (m_adsLayout == null) {
					m_adsLayout = new LinearLayout(activity);
					activity.addContentView(m_adsLayout, new LayoutParams(
							LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
				}

				if (null != mBannerView) {
					m_adsLayout.removeView(mBannerView);
					mBannerView = null;
				}
				mBannerView = new AbsoluteLayout(activity);
				m_adsLayout.addView(mBannerView);

				LinearLayout.LayoutParams linearParams = (LinearLayout.LayoutParams) mBannerView
						.getLayoutParams();
				linearParams.leftMargin = curX;
				linearParams.topMargin = curY;
				linearParams.width = curWidth;
				linearParams.height = curHeight;
				mBannerView.setLayoutParams(linearParams);

				needToDisplay.add(curSpace);
				FlurryAds.fetchAd(activity, curSpace, mBannerView, curSize);
			}	
		});
    }
    
   
	public static void removeAdFromSpace(String space)
	{
		final String curSpace = space;
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (null == mBannerView) {
					return;
				}
				FlurryAds.removeAd(activity, curSpace,
						mBannerView);
			}
		});
	}
	
	public static void setDelegate(boolean enabled)
	{
		delegateEnabled = enabled;
	}
   
	public static void initialize()
	{
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				listener = new FlurryAdListener()
				{
					private FlurryAdType lastType = FlurryAdType.WEB_BANNER;
					@Override
					public boolean shouldDisplayAd(String adSpace, FlurryAdType type)
					{
						lastType = type;
						return FlurryAdsHelper.shouldDisplayAd(adSpace, lastType != FlurryAdType.WEB_BANNER);
					}
					public void onAdClosed (String adSpace)
					{
						FlurryAdsHelper.onAdClosed(adSpace, lastType != FlurryAdType.WEB_BANNER);
					}

					public void onApplicationExit (String adSpace)
					{
						FlurryAdsHelper.onApplicationExit(adSpace);
					}

					public void onRendered (String adSpace)
					{
						FlurryAdsHelper.onRendered(adSpace);
					}

					public void onRenderFailed (String adSpace)
					{
						FlurryAdsHelper.onRenderFailed(adSpace, "");
					}

					public void spaceDidReceiveAd (String adSpace)
					{
						FlurryAdsHelper.onDidReceiveAd(adSpace);
					}

					public void spaceDidFailToReceiveAd (String adSpace)
					{
						FlurryAdsHelper.onDidFailToReceiveAd(adSpace, "");
					}

					public void onAdClicked (String adSpace)
					{
						FlurryAdsHelper.onAdClicked(adSpace);
					}

					public void onAdOpened (String adSpace)
					{
						FlurryAdsHelper.onAdOpened(adSpace);
					}

					public void onVideoCompleted (String adSpace)
					{
						FlurryAdsHelper.onVideoCompleted(adSpace);
					}
				};
				FlurryAds.initializeAds(activity);
				FlurryAds.setAdListener(listener);
			}
		});
	}
   
};

