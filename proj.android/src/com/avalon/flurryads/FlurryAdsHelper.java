package com.avalon.flurryads;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.widget.AbsoluteLayout;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.flurry.android.FlurryAdSize;
import com.flurry.android.FlurryAds;

@SuppressWarnings("deprecation")
public abstract class FlurryAdsHelper {
	private static final Activity activity = Cocos2dxHelper.getActivity();
	private static LinearLayout m_adsLayout;
	private static AbsoluteLayout mBannerView = null;
	
	public static void fetchAdForSpace(String space, int x,int y,int width,int heignt, int size)
    {
		final String curSpace = space;
		final int curX = x;
		final int curY = x;
		final int curWidth = x;
		final int curHeight = x;
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
   
	public static void fetchAndDisplayAdForSpace(String space, int x,int y,int width,int heignt, int size, int timeout)
    {
		final String curSpace = space;
		final int curX = x;
		final int curY = x;
		final int curWidth = x;
		final int curHeight = x;
		final FlurryAdSize curSize = FlurryAdSize.values()[size-1];
		final int curTimeout = timeout;
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

				FlurryAds.getAd(activity, curSpace,
						mBannerView, curSize, curTimeout);
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
   
	public static void initialize()
	{
		FlurryAds.initializeAds(activity);
	}
   
};

