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

package com.avalon.facebook;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.Profile;
import com.facebook.login.LoginBehavior;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;

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

//
public class FacebookHelper
{
    private enum ErrorType {SUCEESS, USER_CANCEL, ERROR}

    private static final String TAG = "C2DXFacebook";
    private static Cocos2dxActivity s_activity = (Cocos2dxActivity)Cocos2dxHelper.getActivity();
    private static CallbackManager callbackManager;

    public static native void delegateOnLogin(int errorType, String token, String errorText,
                                              String[] grantedReadPermissions, String[] grantedPublishPermissions, String[] deniedPermissions);

    public static native void delegateOnMyProfile(int preferedPictureSize, long userData, String[] keys, String[] values);

    private static void threadDelegateOnLogin(final ErrorType errorType, final String token, final String errorText,
                                              final String[] grantedReadPermissions, final String[] grantedPublishPermissions, final String[] deniedPermissions)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                FacebookHelper.delegateOnLogin(errorType.ordinal(), token, errorText,
                        grantedReadPermissions, grantedPublishPermissions, deniedPermissions);
            }
        });
    }

    private static void threadDelegateOnMyProfile(final int preferedPictureSize, final long userData, final String[] keys, final String[] values)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                FacebookHelper.delegateOnMyProfile(preferedPictureSize, userData, keys, values);
            }
        });
    }

//    Cocos2dxHelper.runOnGLThread(new Runnable() {
//        @Override
//        public void run() {
//            Notifications.delegateOnLocalNotification(lActive,lMessage,lSound,lNadgeNumber);
//        }
//    });

    public static void init()
    {
        callbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(callbackManager, new FacebookCallback<LoginResult>()
        {
            @Override public void onSuccess(LoginResult loginResult)
            {
                ArrayList<String> grantedReadPermission = new ArrayList<String>();
                ArrayList<String> grantedPublishPermission = new ArrayList<String>();

                for (String it : loginResult.getRecentlyGrantedPermissions())
                {
                    Method method = null;
                    try
                    {
                        method = LoginManager.class.getDeclaredMethod("isPublishPermission", new Class[]{String.class});

                        if (method != null)
                        {
                            method.setAccessible(true);
                            boolean value = (boolean)method.invoke(LoginManager.class, it);
                            if (value == true)
                            {
                                grantedPublishPermission.add(it);
                            }
                            else
                            {
                                grantedReadPermission.add(it);
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace();
                    }
                }

                String[] grantedReadPermissions = grantedReadPermission.toArray(new String[0]);
                String[] grantedPublishPermissions = grantedPublishPermission.toArray(new String[0]);
                String[] deniedPermissions = loginResult.getRecentlyDeniedPermissions().toArray(new String[0]);
                threadDelegateOnLogin(ErrorType.SUCEESS, loginResult.getAccessToken().getToken(), "", grantedReadPermissions, grantedPublishPermissions, deniedPermissions);
            }

            @Override public void onCancel()
            {
                String[] empty = {};
                threadDelegateOnLogin(ErrorType.USER_CANCEL, "", "", empty, empty, empty);
            }

            @Override public void onError(FacebookException exception)
            {
                String[] empty = {};
                threadDelegateOnLogin(ErrorType.ERROR, "", exception.toString(), empty, empty, empty);
            }
        });

        Cocos2dxHelper.addOnActivityResultListener(new PreferenceManager.OnActivityResultListener()
        {
            @Override public boolean onActivityResult(int requestCode, int resultCode, Intent data)
            {
                callbackManager.onActivityResult(requestCode, resultCode, data);
                return true;
            }
        });
    }

    public static void requestReadPermissions(String[] permissions, boolean debug)
    {
        LoginManager.getInstance().logInWithReadPermissions(s_activity, Arrays.asList(permissions));

        if (debug)
            Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::requestReadPermissions with permissions: " + Arrays.toString(permissions));
    }

    public static void requestPublishPermissions(String[] permissions, boolean debug)
    {
        LoginManager.getInstance().logInWithPublishPermissions(s_activity, Arrays.asList(permissions));

        if (debug)
            Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::requestPublishPermissions with permissions: " + Arrays.toString(permissions));
    }

    public static void logout(boolean debug)
    {
        LoginManager.getInstance().logOut();

        if (debug)
            Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::logout is success");
    }

    public static boolean isLoggedIn()
    {
        if (!AccessToken.getCurrentAccessToken().getToken().isEmpty())
            return true;
        return false;
    }

    public static String getUserID()
    {
        return AccessToken.getCurrentAccessToken().getUserId();
    }

    public static String getAccessToken()
    {
        return AccessToken.getCurrentAccessToken().getToken();
    }

    public static String getAppId()
    {
        int appId = s_activity.getResources().getIdentifier("facebook_app_id", "string", s_activity.getPackageName());
        String appIdStr = s_activity.getResources().getString(appId);
        return appIdStr;
    }

    public static void setLoginBehavior(int loginType)
    {
        LoginBehavior type = LoginBehavior.NATIVE_WITH_FALLBACK;
        switch (loginType)
        {
            case 2:
                type = LoginBehavior.WEB_ONLY;
                break;
            case 4:
                type = LoginBehavior.WEB_VIEW_ONLY;
                break;
            default:
                type = LoginBehavior.NATIVE_WITH_FALLBACK;
                break;
        }

        LoginManager.getInstance().setLoginBehavior(type);
    }

    public static void getMyProfile(String fields, final int preferedPictureSize, final long userData, final boolean debug)
    {
        if (debug)
            Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::getMyProfile request user fields: " + fields);

        GraphRequest request = GraphRequest.newMeRequest(AccessToken.getCurrentAccessToken(), new GraphRequest.GraphJSONObjectCallback()
        {
            @Override public void onCompleted(JSONObject object, GraphResponse response)
            {
                if (debug)
                    Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::getMyProfile is completed with response: " + object.toString());

                ArrayList<String> keys = new ArrayList<String>();
                ArrayList<String> values = new ArrayList<String>();

                for (int i = 0; i < object.names().length(); ++i)
                {
                    String name = null;
                    try {
                        name = object.names().getString(i);
                        keys.add(name);

                        String value = object.getString(name);

                        if (name.equalsIgnoreCase("picture"))
                        {
                            JSONObject picture = object.getJSONObject(name);
                            JSONObject data = picture.getJSONObject("data");
                            values.add(data.getString("url"));
                        }
                        else
                        {
                            values.add(value);
                        }

                    }
                    catch (Exception e)
                    {
                        e.printStackTrace();

                        if (debug)
                            Log.d("avalon_FacebookSocialPlugin", "FacebookSocialPlugin::getMyProfile exception: " + e.toString());
                    }
                }

                String[] stringKeys = keys.toArray(new String[0]);
                String[] stringValues = values.toArray(new String[0]);
                threadDelegateOnMyProfile(preferedPictureSize, userData, stringKeys, stringValues);
            }
        });

        Bundle parameters = new Bundle();
        parameters.putString("fields", fields);
        request.setParameters(parameters);
        request.executeAsync();
    }
}
