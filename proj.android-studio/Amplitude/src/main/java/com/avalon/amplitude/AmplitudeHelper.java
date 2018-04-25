package com.avalon.amplitude;

import com.amplitude.api.Amplitude;
import com.amplitude.api.Revenue;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONObject;

import java.util.Map;

public class AmplitudeHelper
{
	private static Cocos2dxActivity s_activity = (Cocos2dxActivity) Cocos2dxHelper.getActivity();

	public static void init()
	{
        int apiKey = s_activity.getResources().getIdentifier("amplitude_api_key", "string", s_activity.getPackageName());
        String apiKeyStr = s_activity.getResources().getString(apiKey);
        Amplitude.getInstance().initialize(s_activity, apiKeyStr).enableForegroundTracking(s_activity.getApplication());
	}

	public static void logEvent(String eventType, Map<String, String> eventProperties)
    {
        Amplitude.getInstance().logEvent(eventType, new JSONObject(eventProperties));
    }

    public static void logRevenue(String productId, int quantity, double price, String revenueType, String receipt, String receiptSig, Map<String, String> eventProperties)
    {
        Revenue revenue = new Revenue();
        if (productId != null)
            revenue.setProductId(productId);
        revenue.setQuantity(quantity);
        revenue.setPrice(price);
        if (revenueType != null)
            revenue.setRevenueType(revenueType);
        if (receipt != null && receiptSig != null)
            revenue.setReceipt(receipt, receiptSig);
        revenue.setEventProperties(new JSONObject(eventProperties));
        Amplitude.getInstance().logRevenueV2(revenue);
    }
}
