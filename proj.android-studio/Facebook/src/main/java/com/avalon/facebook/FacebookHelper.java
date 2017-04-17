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
import android.preference.PreferenceManager;
import android.util.Log;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.Profile;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.Arrays;
import java.util.Collection;

//
public class FacebookHelper {
    private static final String TAG = "C2DXFacebook";
    private static Cocos2dxActivity s_activity = (Cocos2dxActivity)Cocos2dxHelper.getActivity();
    private static CallbackManager callbackManager;

    public static native void delegateOnLogin(String token);

    private static void threadDelegateOnLogin(final String token)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                FacebookHelper.delegateOnLogin(token);
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

        LoginManager.getInstance().registerCallback(callbackManager, new FacebookCallback<LoginResult>() {
                    @Override
                    public void onSuccess(LoginResult loginResult) {
                        // App code
                        threadDelegateOnLogin(loginResult.getAccessToken().getToken());
                    }

                    @Override
                    public void onCancel() {
                        // App code
                    }

                    @Override
                    public void onError(FacebookException exception) {
                        // App code
                    }
                });

        Cocos2dxHelper.addOnActivityResultListener(new PreferenceManager.OnActivityResultListener(){
            @Override
            public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
                callbackManager.onActivityResult(requestCode, resultCode, data);
                return true;
            }
        });
    }

    public static void requestReadPermissions(String[] permissions)
    {
        LoginManager.getInstance().logInWithReadPermissions(s_activity, Arrays.asList(permissions));
    }

    public static void requestPublishPermissions(String[] permissions)
    {
        LoginManager.getInstance().logInWithReadPermissions(s_activity, Arrays.asList(permissions));
    }

    public static void logout()
    {
        LoginManager.getInstance().logOut();
    }

    public static boolean isLoggedIn()
    {
        if (!AccessToken.getCurrentAccessToken().getToken().isEmpty())
            return true;
        return false;
    }

}
