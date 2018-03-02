package com.avalon.payment;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.Map;
import java.util.HashMap;

public class Backend
{
    private static PurchasingObserver mPurchaseObserver = null;
    private static Map<String, Boolean> pendingItemData = new HashMap<String, Boolean>();
    private static int itemDataReturned = 0;

    /**
     *
     * Methods called from the C++ side
     *
     */

    public static boolean isInitialized()
    {
        if (mPurchaseObserver != null)
            return mPurchaseObserver.isInitialized();
        return false;
    }

    public static void initialize()
    {
        if (!isInitialized()) {
            int rsakeyId = Cocos2dxHelper.getActivity().getResources().getIdentifier("googleplay_rsa_key", "string", Cocos2dxHelper.getActivity().getPackageName());
            String rsakey = Cocos2dxHelper.getActivity().getResources().getString(rsakeyId);
            mPurchaseObserver = new PurchasingObserver(rsakey);
        }
    }

    public static void shutdown()
    {
    }

    public static void purchase(String productId)
    {
        if (mPurchaseObserver.isInitialized())
        {
            mPurchaseObserver.purchase(productId);
        }
    }

    public static boolean isPurchaseReady()
    {
        return isInitialized() && itemDataReturned > 0;
    }

    public static void addItemDataRequest(String productId, boolean isConsumable)
    {
        pendingItemData.put(productId, isConsumable);
    }
    
    public static void clearItemDataRequest()
    {
    	pendingItemData.clear();
    }

    public static void startItemDataRequest()
    {
        if (mPurchaseObserver.isInitialized())
        {
            mPurchaseObserver.startItemDataRequest(pendingItemData);
        }
    }

    public static void delegateOnItemData(String productId, String name, String desc, String priceStr, String curencyCode, float price)
    {
        ++itemDataReturned;
        onItemData(productId, name, desc, priceStr, curencyCode, price);
    }

    /**
     *
     * Methods to call back into the C++ side
     *
     */

    public static native void delegateOnServiceStarted();
    public static native void delegateOnPurchaseSucceed(String productId, String orderId, String token, boolean restored);
    public static native void delegateOnPurchaseFail(String productId, int code);
    public static native void onItemData(String productId, String name, String desc, String priceStr, String curencyCode, float price);
    public static native void onInitialized();
    public static native void onRequestProductsResult(boolean succes);

    /**
     *
     * Helper methods to integrate this lib into your app. Should be called in
     * your main activity - please read the docs provided with this library!
     *
     */

    public static void setPublicKey(final String publicKey)
    {
    	PurchasingObserver.base64EncodedPublicKey = publicKey;
    }
}
