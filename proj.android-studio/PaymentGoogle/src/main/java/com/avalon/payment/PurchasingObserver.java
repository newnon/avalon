package com.avalon.payment;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.preference.PreferenceManager.OnActivityResultListener;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;

import com.example.android.trivialdrivesample.util.IabHelper;
import com.example.android.trivialdrivesample.util.IabResult;
import com.example.android.trivialdrivesample.util.Inventory;
import com.example.android.trivialdrivesample.util.Purchase;
import com.example.android.trivialdrivesample.util.SkuDetails;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import com.avalon.payment.Backend;

public class PurchasingObserver implements OnActivityResultListener
{
    static final String TAG = "avalon_payment_google";
    static final int RC_REQUEST = 10001;
    Activity activity = Cocos2dxHelper.getActivity();
    public static String base64EncodedPublicKey = "";
    IabHelper mHelper;
    private Map<String, Boolean> productIds;
    private Integer taskCount = 0;
    private boolean checkTaskCountOnConsumeFinished = false;
    private static boolean isIabHelperInitialize = false;

    String lastPurchase = null;
    
    public static final int ERROR_UNKNOWN = 0;
    public static final int ERROR_PAYMENTCANCELLED = 3;
    public static final int ERROR_PAYMENTINVALID = 4;
    public static final int ERROR_PAYMENTNOTALLOWED = 5;
    public static final int ERROR_STOREPRODUCTNOTAVAILABLE = 6;

    static private String generateToken(Purchase purchase)
    {
        JSONObject object = new JSONObject();
        try
        {
            object.put("data", purchase.getOriginalJson());
            object.put("sign", purchase.getSignature());
            return object.toString();
        }
        catch (JSONException exp)
        {
        }

        return "";
    }

    public PurchasingObserver(String data)
    {
    	base64EncodedPublicKey = data;
        if (base64EncodedPublicKey.isEmpty()) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    throw new RuntimeException("Required public key not set!");
                }
            });
        }

        mHelper = new IabHelper(activity, base64EncodedPublicKey);
        mHelper.enableDebugLogging(true);
        mHelper.startSetup(mSetupFinishedListener);
        Cocos2dxHelper.addOnActivityResultListener(this);
    }

    public boolean isInitialized()
    {
        if (mHelper != null)
            return isIabHelperInitialize;
        return false;
    }

    protected void finalize() throws Throwable {
        if (mHelper != null) {
            mHelper.dispose();
        }
        mHelper = null;
        super.finalize();
    }

    private boolean isConsumable(String sku)
    {
        return (productIds.containsKey(sku) && productIds.get(sku));
    }

    private String clearTitle(String title)
    {
        // "Iap Title (APP NAME)" ==> "Iap Title"
        int substrPos = title.lastIndexOf("(") - 1;
        if (substrPos > 0) {
            title = title.substring(0, substrPos);
        }
        return title;
    }

    @Override
    public boolean onActivityResult(int requestCode, int resultCode, Intent data)
    {
        return mHelper.handleActivityResult(requestCode, resultCode, data);
    }

    /**
     *
     * Async callback methods
     *
     */

    final IabHelper.OnIabSetupFinishedListener mSetupFinishedListener = new IabHelper.OnIabSetupFinishedListener()
    {
        @Override
        public void onIabSetupFinished(IabResult result) {
            if (!result.isSuccess()) {
                isIabHelperInitialize = false;
                Log.e(TAG, "onIabSetupFinished failed: " + result);
                return;
            }

            isIabHelperInitialize = true;
            threadOnInitialized();
        }
    };

    final IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener()
    {
        @Override
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            if (result.isFailure()) {
                Log.e(TAG, "onQueryInventoryFinished failed: " + result);
                threadDelegateRequestFail();
                return;
            }

            if (inventory.getAllDetailsSkus().isEmpty()) {
                Log.e(TAG, "onQueryInventoryFinished failed: Not a single detail returned! Google Play configured?");
                threadDelegateRequestFail();
                return;
            }

            threadDelegateItemData(inventory);
            threadDelegateRequestSucces();

            for (String sku : inventory.getAllOwnedSkus()) {
                if (isConsumable(sku)) {
                    threadConsumeAsync(inventory.getPurchase(sku));
                } else {
                    Purchase purchase = inventory.getPurchase(sku);
                    threadDelegateOnPurchaseSucceed(sku, purchase.getOrderId(), generateToken(purchase), true);
                }
            }

            checkTaskCountOnConsumeFinished = true;
        }
    };

    final IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener()
    {
        @Override
        public void onIabPurchaseFinished(IabResult result, final Purchase purchase) {
            if (result.isFailure()) {
                {
                    Log.e(TAG, "onIabPurchaseFinished failed: " + result);
                    int retCode = ERROR_UNKNOWN;
                    switch (result.getResponse()) {
                        case IabHelper.BILLING_RESPONSE_RESULT_USER_CANCELED:
                    	case IabHelper.IABHELPER_USER_CANCELLED:
                    	    retCode = ERROR_PAYMENTCANCELLED;
                            break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE:
                    	    retCode = ERROR_PAYMENTNOTALLOWED;
                        	break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE:
                    	    retCode = ERROR_STOREPRODUCTNOTAVAILABLE;
                        	break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_DEVELOPER_ERROR:
                    	    retCode = ERROR_UNKNOWN;
                    		break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_ERROR:
                    	    retCode = ERROR_PAYMENTINVALID;
                    		break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED:
                    	    retCode = ERROR_STOREPRODUCTNOTAVAILABLE;
                    		break;
                    	case IabHelper.BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED:
                    	    retCode = ERROR_STOREPRODUCTNOTAVAILABLE;
                			break;
                    }
                    threadDelegateOnPurchaseFail(purchase!=null?purchase.getSku():lastPurchase, retCode);
                }
            } else if (isConsumable(purchase.getSku())) {
                threadConsumeAsync(purchase);
            } else {
                threadDelegateOnPurchaseSucceed(purchase.getSku(), purchase.getOrderId(), generateToken(purchase), false);
            }

            threadDecrementTaskCounter();
        }
    };

    final IabHelper.OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener()
    {
        @Override
        public void onConsumeFinished(final Purchase purchase, IabResult result) {
            if (!result.isSuccess()) {
                Log.e(TAG, "onConsumeFinished failed: " + result);
                return;
            }

            threadDelegateOnPurchaseSucceed(purchase.getSku(), purchase.getOrderId(), generateToken(purchase), false);
            if (checkTaskCountOnConsumeFinished) {
                threadDecrementTaskCounter();
            }
        }
    };

    /**
     *
     * Helper methods that keeps to code above short and readable
     *
     */

    private void threadOnInitialized()
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                Backend.onInitialized();
            }
        });
    }

    private void threadDelegateOnServiceStarted()
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
            	Backend.delegateOnServiceStarted();
            }
        });
    }

    private void threadDelegateOnPurchaseFail(final String sku, int code)
    {
        final int lcode = code;
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
            	Backend.delegateOnPurchaseFail(sku, lcode);
            }
        });
    }

    private void threadConsumeAsync(final Purchase purchase)
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mHelper.consumeAsync(purchase, mConsumeFinishedListener);
            }
        });
    }

    private void threadDelegateOnPurchaseSucceed(final String sku, final String orderId, final String token, final boolean restored)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
            	Backend.delegateOnPurchaseSucceed(sku, orderId, token, restored);
            }
        });
    }

    private void threadDecrementTaskCounter()
    {
        --taskCount;
    }

    private void threadIncrementTaskCounter()
    {
        ++taskCount;
    }
    
    private void threadDelegateRequestFail()
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                Backend.onRequestProductsResult(false);
            }
        });
    }
    
    private void threadDelegateRequestSucces()
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                Backend.onRequestProductsResult(true);
            }
        });
    }

    private void threadDelegateItemData(final Inventory inventory)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                for (String sku : inventory.getAllDetailsSkus()) {
                    SkuDetails details = inventory.getSkuDetails(sku);

                    Backend.delegateOnItemData(
                        details.getSku(),
                        clearTitle(details.getTitle()),
                        details.getDescription(),
                        details.getPrice(),
                        details.getCurrencyCode(),
                        details.getPriceValue()
                    );
                }
            }
        });
    }

    /**
     *
     * PUBLIC API FOR com.avalon.payment.Backend
     *
     */

    public void purchase(final String sku)
    {
        lastPurchase = sku;
        threadIncrementTaskCounter();

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    mHelper.launchPurchaseFlow(activity, sku, RC_REQUEST, mPurchaseFinishedListener);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        });
    }

    public void startItemDataRequest(Map<String, Boolean> productIds)
    {
        this.productIds = productIds;
        final List<String> moreSkus = new ArrayList<String>(productIds.keySet());

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    mHelper.queryInventoryAsync(true, moreSkus, mGotInventoryListener);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        });
    }
}
