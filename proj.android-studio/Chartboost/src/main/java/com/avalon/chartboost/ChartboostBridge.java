/****************************************************************************
 Copyright (c) 2014      Jesús Sánchez
 
 http://jesanchezgo.hostwp.es
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

package com.avalon.chartboost;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.util.Log;

import com.chartboost.sdk.*;
import com.chartboost.sdk.Model.CBError.CBClickError;
import com.chartboost.sdk.Model.CBError.CBImpressionError;

public class ChartboostBridge {
    private static final String TAG = "C2DXChartboost";
    private static Cocos2dxActivity s_activity;
    private static boolean initChartboost = false;

    private static native boolean shouldRequestInterstitial(String location);
    private static native boolean shouldDisplayInterstitial(String location);
    private static native boolean didDisplayInterstitial(String location);
    private static native void didCacheInterstitial(String location);
    private static native void didFailToLoadInterstitial(String location, int error);
    private static native void didFailToRecordClick(String uri, int error);
    private static native void didDismissInterstitial(String location);
    private static native void didCloseInterstitial(String location);
    private static native void didClickInterstitial(String location);
    
    private static native boolean shouldDisplayRewardedVideo(String location);
    private static native void didDisplayRewardedVideo(String location);    
    private static native void didCacheRewardedVideo(String location);
    private static native void didFailToLoadRewardedVideo(String location,int error);
    private static native void didDismissRewardedVideo(String location);
    private static native void didCloseRewardedVideo(String location);
    private static native void didClickRewardedVideo(String location);
    private static native void didCompleteRewardedVideo(String location, int reward);
    
    // the method must be called in main thread, before any other method
    public static void initChartboostBridge(Cocos2dxActivity activity){
    	ChartboostBridge.s_activity = activity;
    }
    
    public static void startWithAppId(String appId, String appSignature) {
        // Init Chartboost
    	Chartboost.startWithAppId(ChartboostBridge.s_activity, appId, appSignature);
        // Set delegate
        Chartboost.setDelegate(ChartboostBridge.s_chartBoostDelegate);
        // On create
        Chartboost.onCreate(ChartboostBridge.s_activity);
        
        ChartboostBridge.initChartboost = true;
    }
    
    public static void cacheInterstitial(final String location) {
        Log.v(TAG, "cacheInterstitial(\"" + location + "\") is called...");
        
        ChartboostBridge.s_activity.runOnUiThread(new Runnable() {
            public void run() {
            	Chartboost.cacheInterstitial(location);
            }
        });
    }
        
    public static void showInterstitial(final String location) {
        Log.v(TAG, "showInterstitial(\"" + location + "\") is called...");

        ChartboostBridge.s_activity.runOnUiThread(new Runnable() {
            public void run() {
            	Chartboost.showInterstitial(location);
            }
        });
    }
        
    public static boolean hasInterstitial(final String location) {
        Log.v(TAG, "hasInterstitial(\"" + location + "\") is called...");
        
        return Chartboost.hasInterstitial(location);
    }
     
    public static void cacheRewardedVideo(final String location) {
        Log.v(TAG, "cacheRewardedVideo(\"" + location + "\") is called...");
        
        ChartboostBridge.s_activity.runOnUiThread(new Runnable() {
            public void run() {
            	Chartboost.cacheRewardedVideo(location);
            }
        });
    }
        
    public static void showRewardedVideo(final String location) {
        Log.v(TAG, "cacheRewardedVideo(\"" + location + "\") is called...");

        ChartboostBridge.s_activity.runOnUiThread(new Runnable() {
            public void run() {
            	Chartboost.showRewardedVideo(location);
            }
        });
    }
        
    public static boolean hasRewardedVideo(final String location) {
        Log.v(TAG, "hasInterstitial(\"" + location + "\") is called...");
        
        return Chartboost.hasRewardedVideo(location);
    }
    
    static private ChartboostDelegate s_chartBoostDelegate = new ChartboostDelegate() {
        /**
         * Interstital
         */
        @Override
        public boolean shouldRequestInterstitial(String location)
        {
            return ChartboostBridge.shouldRequestInterstitial(location);
        }

        @Override
        public boolean shouldDisplayInterstitial(String location)
        {
            return ChartboostBridge.shouldDisplayInterstitial(location);
        }
        
        @Override
        public void didDisplayInterstitial(final String location) {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didDisplayInterstitial(location);
                }
            });
        }
        
        @Override
        public void didCacheInterstitial(final String location) {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didCacheInterstitial(location);
                }
            });
        }
   
        @Override
        public void didFailToLoadInterstitial(final String location, final CBImpressionError error)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didFailToLoadInterstitial(location, error.ordinal());
                }
            });
        }

        @Override
        public void didFailToRecordClick(final String uri, final CBClickError error)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didFailToRecordClick(uri, error.ordinal());
                }
            });

        }
        
        @Override
        public void didDismissInterstitial(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didDismissInterstitial(location);
		        }
		    });
        }
        
        @Override
        public void didCloseInterstitial(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didCloseInterstitial(location);
		        }
		    });
        }

        @Override
        public void didClickInterstitial(final String location)
        {
        	ChartboostBridge.didClickInterstitial(location);
        }

        @Override
        public boolean shouldDisplayRewardedVideo(final String location) 
        {
        	return ChartboostBridge.shouldDisplayRewardedVideo(location);
        }
        
        @Override
        public void didDisplayRewardedVideo(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didDisplayRewardedVideo(location);
                }
            });
        }
        
        @Override
        public void didCacheRewardedVideo(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didCacheRewardedVideo(location);
                }
            });
        }
        
        @Override
        public void didFailToLoadRewardedVideo(final String location, final CBImpressionError error)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didFailToLoadRewardedVideo(location, error.ordinal());
                }
            });
        }
        
        @Override
        public void didDismissRewardedVideo(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didDismissRewardedVideo(location);
                }
            });
        }
        
        @Override
        public void didCloseRewardedVideo(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didCloseRewardedVideo(location);
                }
            });
        }
        
        @Override
        public void didClickRewardedVideo(final String location)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didClickRewardedVideo(location);
                }
            });
        }
        
        @Override
        public void didCompleteRewardedVideo(final String location, final int reward)
        {
        	ChartboostBridge.s_activity.runOnGLThread(new Runnable() {
                public void run() {
                	ChartboostBridge.didCompleteRewardedVideo(location, reward);
                }
            });
        }
    };
}
