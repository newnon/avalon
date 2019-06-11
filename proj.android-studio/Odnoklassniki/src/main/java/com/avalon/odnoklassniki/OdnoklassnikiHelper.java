package com.avalon.odnoklassniki;

import android.content.Intent;
import android.preference.PreferenceManager;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;
import java.lang.Void;

import ru.ok.android.sdk.Odnoklassniki;
import ru.ok.android.sdk.OkAuthListener;
import ru.ok.android.sdk.OkListener;
import ru.ok.android.sdk.OkRequestMode;
import ru.ok.android.sdk.util.OkAuthType;

/**
 * Created by Vladimir Shlobin on 03.04.2018.
 */

public class OdnoklassnikiHelper
{
    private enum ErrorType {SUCCESS, USER_CANCEL, ERROR}
    private static final String TAG = "C2DXOdnoklassniki";
    private static Cocos2dxActivity s_activity = (Cocos2dxActivity) Cocos2dxHelper.getActivity();

    public static native void delegateOnLogin(int errorType, String token, String userId, String errorText);

    public static native void delegateOnMyProfile(int errorType, long userData, String pictureField, String[] keys, String[] values, String errorText);

    private static void threadDelegateOnLogin(final ErrorType errorType, final String token, final String userId, final String errorText)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable()
        {
            @Override
            public void run()
            {
                OdnoklassnikiHelper.delegateOnLogin(errorType.ordinal(), token, userId, errorText);
            }
        });
    }

    private static void threadDelegateOnMyProfile(final ErrorType errorType, final long userData, final String pictureField, final String[] keys, final String[] values, final String errorText)
    {
        Cocos2dxHelper.runOnGLThread(new Runnable()
        {
            @Override
            public void run()
            {
                OdnoklassnikiHelper.delegateOnMyProfile(errorType.ordinal(), userData, pictureField, keys, values, errorText);
            }
        });
    }

    public static void init()
    {
        FutureTask<Void> task = new FutureTask<>(new Callable<Void>()
        {
            @Override
            public Void call()
            {
                Cocos2dxHelper.addOnActivityResultListener(new PreferenceManager.OnActivityResultListener()
                {
                    @Override
                    public boolean onActivityResult(int requestCode, int resultCode, Intent data)
                    {
                        if (Odnoklassniki.getInstance().isActivityRequestOAuth(requestCode))
                            Odnoklassniki.getInstance().onAuthActivityResult(requestCode, resultCode, data, getAuthListener());
                        return true;
                    }
                });
                return null;
            }
        });

        s_activity.runOnUiThread(task);

        try
        {
            task.get();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    private static OkAuthListener getAuthListener()
    {
        return new OkAuthListener()
        {
            @Override
            public void onSuccess(final JSONObject authJson)
            {
                try
                {
                    final String accessToken = authJson.getString("access_token");
                    Odnoklassniki.getInstance().requestAsync("users.getLoggedInUser", null, OkRequestMode.getDEFAULT(), new OkListener()
                    {
                        @Override
                        public void onSuccess(JSONObject userIdJson)
                        {
                            try
                            {
                                String userId = userIdJson.getString("result");
                                threadDelegateOnLogin(ErrorType.SUCCESS, accessToken, userId.substring(1, userId.length()-1) , "");
                            }
                            catch (JSONException e)
                            {
                                e.printStackTrace();
                            }
                        }

                        @Override
                        public void onError(String error)
                        {
                            threadDelegateOnLogin(ErrorType.ERROR, "", "", error);
                        }
                    });
                }
                catch (JSONException e)
                {
                    e.printStackTrace();
                }
            }

            @Override
            public void onCancel(String error)
            {
                threadDelegateOnLogin(ErrorType.USER_CANCEL, "", "", error);
            }

            @Override
            public void onError(String error)
            {
                threadDelegateOnLogin(ErrorType.ERROR, "", "", error);
            }
        };
    }

    public static void requestReadPermissions(final String[] permissions, boolean debug)
    {
        s_activity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                String appId = getAppId();
                final String redirectUri = "okauth://ok" + appId;
                Odnoklassniki.createInstance(s_activity, appId, getAppKey());
                Odnoklassniki.getInstance().checkValidTokens(new OkListener()
                {
                    @Override
                    public void onSuccess(JSONObject json)
                    {
                        try
                        {
                            String accessToken = json.getString("access_token");
                            String userId = json.getString("logged_in_user");
                            threadDelegateOnLogin(ErrorType.SUCCESS, accessToken, userId.substring(1, userId.length()-1) , "");
                        }
                        catch (Exception e)
                        {
                            Odnoklassniki.getInstance().requestAuthorization(s_activity, redirectUri, OkAuthType.ANY, permissions);
                        }
                    }

                    @Override
                    public void onError(String error)
                    {
                        Odnoklassniki.getInstance().requestAuthorization(s_activity, redirectUri, OkAuthType.ANY, permissions);
                    }
                });
            }
        });
    }

    public static void logout(boolean debug)
    {
        FutureTask<Void> task = new FutureTask<>(new Callable<Void>()
        {
            @Override
            public Void call()
            {
                Odnoklassniki.getInstance().clearTokens();
                return null;
            }
        });

        s_activity.runOnUiThread(task);

        try
        {
            task.get();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    public static String getAppId()
    {
        int appId = s_activity.getResources().getIdentifier("odnoklassniki_app_id", "string", s_activity.getPackageName());
        return s_activity.getResources().getString(appId);
    }

    public static String getAppKey()
    {
        int appKey = s_activity.getResources().getIdentifier("odnoklassniki_app_key", "string", s_activity.getPackageName());
        return s_activity.getResources().getString(appKey);
    }

    public static void getMyProfile(String fields, final long userData, String pictureField, final boolean debug)
    {
        final Map<String, String> fieldsMap = new HashMap<>();
        fieldsMap.put("fields", fields);
        final String localPictureField = pictureField;
        final OkListener okListener = new OkListener()
        {
            @Override
            public void onSuccess(JSONObject json)
            {
                ArrayList<String> keys = new ArrayList<>();
                ArrayList<String> values = new ArrayList<>();

                for (int i = 0; i < json.length(); ++i)
                {
                    try
                    {
                        String name = json.names().getString(i);
                        keys.add(name);

                        String value = json.getString(name);
                        values.add(value);
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace();
                    }
                }

                String[] stringKeys = keys.toArray(new String[0]);
                String[] stringValues = values.toArray(new String[0]);
                threadDelegateOnMyProfile(ErrorType.SUCCESS, userData, localPictureField, stringKeys, stringValues,"");
            }

            @Override
            public void onError(String error)
            {
                String[] empty = {};
                threadDelegateOnMyProfile(ErrorType.ERROR, userData, localPictureField, empty, empty, error);
            }
        };

        s_activity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                Odnoklassniki.getInstance().requestAsync("users.getCurrentUser", fieldsMap, OkRequestMode.getDEFAULT(), okListener);
            }
        });
    }
}
