package com.avalon.flurry;

import com.flurry.android.FlurryAgent;
import org.cocos2dx.lib.Cocos2dxHelper;

public abstract class FlurryHelper {
	
	public static void onStartSession (String apiKey) {
		FlurryAgent.onStartSession(Cocos2dxHelper.getActivity().getApplicationContext(), apiKey);
	}
	public static void onEndSession () {
		FlurryAgent.onEndSession(Cocos2dxHelper.getActivity().getApplicationContext());
	}
}
