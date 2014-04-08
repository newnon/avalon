package com.avalon.web;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
//import android.util.Log;
import android.webkit.WebSettings;
//import android.webkit;
import android.webkit.WebViewClient;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

public abstract class WebView {
	private static LinearLayout m_webLayout;
	private static android.webkit.WebView m_webView;
	private static final Activity activity = Cocos2dxHelper.getActivity();
    
    //WebView
    public static void displayWebView(final int x, final int y, final int width, final int height) {
    	//Log.e("WebView", "showWebView");
    	
    	activity.runOnUiThread(new Runnable() {
            public void run() {
            	
            	if(m_webLayout == null)
            	{
            		m_webLayout = new LinearLayout(activity);
            		activity.addContentView(m_webLayout, 
            				new LayoutParams(LayoutParams.FILL_PARENT,LayoutParams.FILL_PARENT));
            	}
            	
            	m_webView = new android.webkit.WebView(activity);
            	m_webLayout.addView(m_webView);
            	
            	LinearLayout.LayoutParams linearParams = (LinearLayout.LayoutParams) m_webView.getLayoutParams();
            	linearParams.leftMargin = x;
            	linearParams.topMargin = y;
            	linearParams.width = width;
            	linearParams.height = height;
            	m_webView.setLayoutParams(linearParams);

            	m_webView.setBackgroundColor(0);
            	m_webView.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);
            	m_webView.getSettings().setAppCacheEnabled(false);
            	//m_webView.setBackgroundResource(R.drawable.yourImage);
            	
            	m_webView.setWebViewClient(new WebViewClient(){
                    @Override
                    public boolean shouldOverrideUrlLoading(android.webkit.WebView view, String url){
                            
                            return false;
                            
                    }
                });
            	
            }
        });
    }
    
    public static void updateURL(final String url) {
    	//Log.e("WebView", "updateURL"+url);
    	
    	activity.runOnUiThread(new Runnable() {
            public void run() {
            	m_webView.loadUrl(url);
            }
        });
    }
    
    public static void removeWebView() {
    	//Log.e("WebView", "removeWebView");

    	activity.runOnUiThread(new Runnable() {
            public void run() {
            	m_webLayout.removeView(m_webView);
            	m_webView.destroy();
            }
        });
    }
}
