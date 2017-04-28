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

package com.avalon.vkontakte;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;


import com.vk.sdk.VKAccessToken;
import com.vk.sdk.VKCallback;
import com.vk.sdk.VKSdk;
import com.vk.sdk.api.VKApi;
import com.vk.sdk.api.VKApiConst;
import com.vk.sdk.api.VKError;
import com.vk.sdk.api.VKParameters;
import com.vk.sdk.api.VKRequest;
import com.vk.sdk.api.VKResponse;
import com.vk.sdk.api.model.VKScopes;
import com.vk.sdk.util.VKUtil;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

//
public class VkontakteHelper
{
    private static final String TAG = "C2DXVkontakte";
    private static Cocos2dxActivity s_activity = (Cocos2dxActivity)Cocos2dxHelper.getActivity();

    public static native void delegateOnLogin(int errorType, String token, String errorText);
    public static native void delegateOnMyProfile(long userData, String[] keys, String[] values);

    private static void threadDelegateOnLogin(final int errorType, final String token, final String errorText)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable()
        {
            @Override public void run()
            {
                VkontakteHelper.delegateOnLogin(errorType, token, errorText);
            }
        });
    }

    private static void threadDelegateOnMyProfile(final long userData, final String[] keys, final String[] values)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable()
        {
            @Override public void run()
            {
                VkontakteHelper.delegateOnMyProfile(userData, keys, values);
            }
        });
    }

    public static void init()
    {
        Method method = null;
        try
        {
            method = VKSdk.class.getDeclaredMethod("initialize", new Class[]{Context.class, int.class, String.class});

            if (method != null)
            {
                int appId = s_activity.getResources().getIdentifier(VKSdk.SDK_APP_ID, "integer", s_activity.getPackageName());

                method.setAccessible(true);
                method.invoke(VKSdk.class, s_activity, s_activity.getResources().getInteger(appId), "");
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        Cocos2dxHelper.addOnActivityResultListener(new PreferenceManager.OnActivityResultListener()
        {
            @Override public boolean onActivityResult(int requestCode, int resultCode, Intent data)
            {
                VKSdk.onActivityResult(requestCode, resultCode, data, new VKCallback<VKAccessToken>()
                {
                    @Override public void onResult(VKAccessToken res)
                    {
                        threadDelegateOnLogin(0, res.accessToken, "");
                    }
                    @Override public void onError(VKError error)
                    {
                        threadDelegateOnLogin(error.errorCode, "", error.toString());
                    }
                });
                return true;
            }
        });
    }

    public static void requestReadPermissions(String[] permissions, boolean debug)
    {
        VKSdk.login(s_activity, permissions);

        if (debug)
        {
            Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::requestReadPermissions with permissions: " + Arrays.toString(permissions));

            String[] fingerprints = VKUtil.getCertificateFingerprint(s_activity, s_activity.getPackageName());
            Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::requestReadPermissions sdk_fingerprint: " + Arrays.toString(fingerprints));
        }
    }

    public static void logout(boolean debug)
    {
        VKSdk.logout();

        if (debug)
            Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::logout is success");
    }

    public static boolean isLoggedIn()
    {
        return VKSdk.isLoggedIn();
    }

    public static String getUserID()
    {
        if (isLoggedIn())
            return VKAccessToken.currentToken().userId;
        return "";
    }

    public static String getAccessToken()
    {
        if (isLoggedIn())
            return VKAccessToken.currentToken().accessToken;
        return "";
    }

    public static String getAppId()
    {
        int appId = s_activity.getResources().getIdentifier(VKSdk.SDK_APP_ID, "integer", s_activity.getPackageName());
        String appIdStr = s_activity.getResources().getString(appId);
        return appIdStr;
    }

    public static void getMyProfile(String fields, final long userData, final boolean debug)
    {
        if (debug)
            Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::getMyProfile request user fields: " + fields);

        VKRequest request = VKApi.users().get(VKParameters.from(VKApiConst.USER_ID, getUserID(), VKApiConst.FIELDS, fields));
        request.executeWithListener(new VKRequest.VKRequestListener()
        {
            @Override public void onComplete(VKResponse response)
            {
                if (debug)
                    Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::getMyProfile is completed with response: " + response.json.toString());

                try
                {
                    ArrayList<String> keys = new ArrayList<String>();
                    ArrayList<String> values = new ArrayList<String>();
                    JSONArray array = response.json.getJSONArray(response.json.names().getString(0));

                    for (int i = 0; i < array.length(); ++i)
                    {
                        JSONObject object = array.getJSONObject(i);
                        for (int j = 0; j < object.names().length(); ++j)
                        {
                            String name = object.names().getString(j);
                            String value = object.getString(name);
                            keys.add(name);
                            values.add(value);
                        }
                    }

                    String[] stringKeys = keys.toArray(new String[0]);
                    String[] stringValues = values.toArray(new String[0]);
                    threadDelegateOnMyProfile(userData, stringKeys, stringValues);
                }
                catch (Exception e)
                {
                    e.printStackTrace();

                    if (debug)
                        Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::getMyProfile exception: " + e.toString());
                }
            }
            @Override public void onError(VKError error)
            {
                if (debug)
                    Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::getMyProfile error: " + error.toString());

                String[] empty = {};
                threadDelegateOnMyProfile(userData, empty, empty);
            }
            @Override public void attemptFailed(VKRequest request, int attemptNumber, int totalAttempts)
            {
                if (debug)
                    Log.d("avalon_VKSocialPlugin", "VKSocialPlugin::getMyProfile attempt failed " + request.toString() + " number " + attemptNumber + " total " + totalAttempts);

                String[] empty = {};
                threadDelegateOnMyProfile(userData, empty, empty);
            }
        });
    }
}
