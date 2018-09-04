package com.avalon.notifications;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.gcm.GoogleCloudMessaging;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

public class Notifications{

	private static Context activity = Cocos2dxHelper.getActivity();
	private static final String TAG = "avalon_Notifications";
	private final static String LOCAL_NOTIFICAION_STORE = "LocalNotifications";
	private final static String REMOTE_NOTIFICAION_STORE = "RemoteNotifications";
	private static final String PROPERTY_REG_ID = "registration_id";
    private static final String PROPERTY_APP_VERSION = "appVersion";
	private static final int PLAY_SERVICES_RESOLUTION_REQUEST = 9000;
    private static HashMap<String, String> notificationParams = null;
	//private static String SENDER_ID = "Your-Sender-ID";
	private static GoogleCloudMessaging gcm;
	private static String regid;
	
	private static native void delegateOnLocalNotification(boolean active, int id, String message, String title, String sound, int badgeNumber);
	private static native void delegateOnRemoteNotification(boolean active, String message, String title, String sound, int badgeNumber);
	private static native void delegateOnRegisterForRemoteNotifications(String data, String error);

    public static void setContext (Context context) {
        if (Notifications.activity == null) {
            Notifications.activity = context;
        }
    }
	
	public static void onLocalNotification(int id, String message, String title, String sound, int badgeNumber)
	{
		if(Cocos2dxHelper.getActivity() != null)
		{
			final String lMessage = message;
            final String lTitle = title;
			final String lSound = sound;
			final int lNadgeNumber = badgeNumber;
            final int lId = id;
			final boolean lActive = getApplicationActive();
			Cocos2dxHelper.runOnGLThread(new Runnable() {
				@Override
				public void run() {
					Notifications.delegateOnLocalNotification(lActive, lId, lMessage, lTitle, lSound, lNadgeNumber);
				}
			});
		}
	}
	
	public static void onRemoteNotification(String message, String title, String sound, int badgeNumber)
	{
		if(Cocos2dxHelper.getActivity() != null)
		{
			final String lMessage = message;
            final String lTitle = title;
			final String lSound = sound;
			final int lNadgeNumber = badgeNumber;
			final boolean lActive = getApplicationActive();
			Cocos2dxHelper.runOnGLThread(new Runnable() {
				@Override
				public void run() {
					Notifications.delegateOnRemoteNotification(lActive, lMessage, lTitle, lSound, lNadgeNumber);
				}
			});
		}
	}
	
	public static void onRegisterForRemoteNotifications(String data, String error)
	{
		if(Cocos2dxHelper.getActivity() != null)
		{
			final String lData = data;
			final String lError = error;
			Cocos2dxHelper.runOnGLThread(new Runnable() {
				@Override
				public void run() {
					Notifications.delegateOnRegisterForRemoteNotifications(lData,lError);
				}
			});
		}
	}
	
	public static void showLocalNotification(String message, String title, String sound, long time, int id, int badgeNumber, HashMap<String, String> params) {
		Log.v(TAG, "showLocalNotification");
		add(message, title, sound, time, id, badgeNumber, params);
		JSONObject obj = new JSONObject();
		try {
			obj.put("message", message);
            obj.put("title", title);
			obj.put("sound", sound);
			obj.put("time", time);
			obj.put("badge", badgeNumber);
            if(params != null)
                obj.put("params", params);
		} catch (JSONException e) {
		}
		persist(id,obj);
	}
	
    /**
     * Cancel a specific notification that was previously registered.
     *
     * @param notificationId
     *            The original ID of the notification that was used when it was
     *            registered using add()
     */
	public static void cancelLocalNotification(int tag) {
		Log.v(TAG, "cancelLocalNotification");
		cancel(tag);
		unpersist(tag);
	}
	
    /**
     * Cancel all notifications that were created by this plugin.
     *
     * Android can only unregister a specific alarm. There is no such thing
     * as cancelAll. Therefore we rely on the Shared Preferences which holds
     * all our alarms to loop through these alarms and unregister them one
     * by one.
     */
	public static void cancelAllLocalNotifications() {
		Log.v(TAG, "cancelAllLocalNotifications");
		cancelAll();
		unpersistAll();
	}

    public static void processIntent(Intent intent)
    {
        Bundle extras = intent.getExtras();
        if(extras != null) {
            Bundle bundle = extras.getBundle("notification");
            if (bundle != null) {
                notificationParams = new HashMap<String, String>();
                for (String key : bundle.keySet()) {
                    Object value = bundle.get(key);
                    notificationParams.put(key, value!=null ? value.toString() : "");
                }
                extras.remove("notification");
            }
        }
    }

    public static void onPause()
    {
        notificationParams = null;
    }

    public static HashMap<String, String> getLaunchedNotification()
    {
        return notificationParams;
    }
	
	static void showNotification (Context context, int notificationId, String message, String title, String sound, int badge, Bundle bundle) {
		String packageName  = context.getPackageName();
        Intent launchIntent = context.getPackageManager().getLaunchIntentForPackage(packageName);

        launchIntent.addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_SINGLE_TOP);

        Bundle localBundle = new Bundle(bundle);
        localBundle.putInt("id", notificationId);
        launchIntent.putExtra("notification", localBundle);
        
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, launchIntent,
				PendingIntent.FLAG_UPDATE_CURRENT);

		Bitmap largeIcon = null;
        try {
            largeIcon = ((BitmapDrawable)context.getPackageManager().getApplicationIcon(context.getPackageName())).getBitmap();
        } catch (NameNotFoundException e) {
        }
        Notification.Builder builder = new Notification.Builder(context);
        if(title != null && !title.isEmpty())
            builder.setContentTitle(title);
        else
		    builder.setContentTitle(context.getString(context.getResources().getIdentifier("app_name", "string", context.getPackageName())));
		builder.setContentText(message);
		builder.setSmallIcon(context.getResources().getIdentifier("icon_small", "drawable", context.getPackageName()));
		builder.setLargeIcon(largeIcon);
		builder.setTicker(message);
		builder.setAutoCancel(true);
		builder.setNumber(badge);
		builder.setDefaults(Notification.DEFAULT_LIGHTS | Notification.DEFAULT_SOUND);
		if(sound != null && !sound.isEmpty())
			builder.setSound(Uri.parse(sound));
		builder.setContentIntent(pendingIntent);

		try
	    {
			getNotificationManager().notify(notificationId, builder.build());
	    }
		catch(Exception e)
	    {
			//Android 4.2 bug with vibrate permissions
	    }
    }
	
	/**
     * Checks wether a notification with an ID is scheduled.
     *
     * @param id
     *          The notification ID to be check.
     * @param callbackContext
     */
    public static boolean isScheduled (int id) {
        SharedPreferences settings = getLocalSharedPreferences();
        Map<String, ?> alarms      = settings.getAll();
        return alarms.containsKey(Integer.toString(id));
    }

    /**
     * Retrieves a list with all currently pending notifications.
     *
     * @param callbackContext
     */
    public static int[] getScheduledIds () {
        SharedPreferences settings = getLocalSharedPreferences();
        Map<String, ?> alarms      = settings.getAll();
        Set<String> keySet = alarms.keySet();
        int[] ret = new int[keySet.size()];
        int num = 0;
        for(String s: keySet){
        	ret[num++] = Integer.parseInt(s);
        }
        return ret;
    }
	
	static void add(String message, String title, String sound, long time, int notificationId, int badgeNumber, HashMap<String, String> params) {
        Bundle bundle = new Bundle();
        bundle.putString("message", message);
        bundle.putString("title", title);
        bundle.putInt("badge", badgeNumber);
        bundle.putString("sound", sound);

        if(params != null) {
            for (Map.Entry<String, String> entry : params.entrySet()) {
                bundle.putString(entry.getKey(), entry.getValue());
            }
        }

        Intent intent = new Intent(activity.getApplicationContext(), LocalNotificationsReceiver.class)
            .setAction("" + notificationId)
            .putExtra("notification", bundle);

        AlarmManager am  = getAlarmManager();
        PendingIntent sender = PendingIntent.getBroadcast(activity, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT);

        am.set(AlarmManager.RTC_WAKEUP, time * 1000, sender);
    }

	static void cancel(int notificationId) {
        /*
         * Create an intent that looks similar, to the one that was registered
         * using add. Making sure the notification id in the action is the same.
         * Now we can search for such an intent using the 'getService' method
         * and cancel it.
         */
        Intent intent = new Intent(activity.getApplicationContext(), LocalNotificationsReceiver.class)
            .setAction("" + notificationId);

        PendingIntent pi       = PendingIntent.getBroadcast(activity, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT);
        AlarmManager am        = getAlarmManager();
        NotificationManager nc = getNotificationManager();

        am.cancel(pi);

        try {
            nc.cancel(notificationId);
        } catch (Exception e) {}

    }

	static void cancelAll() {
        SharedPreferences settings = getLocalSharedPreferences();
        NotificationManager nc     = getNotificationManager();
        Map<String, ?> alarms      = settings.getAll();
        Set<String> alarmIds       = alarms.keySet();

        for (String alarmId : alarmIds) {
        	try {
        		cancel(Integer.parseInt(alarmId));
        	} catch (Exception e) {}
        }

        nc.cancelAll();
    }
	
	public static void registerForRemoteNotification() {

		Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            public void run() {
        		if (checkPlayServices()) {
        			gcm = GoogleCloudMessaging.getInstance(activity);
        			regid = getRegistrationId(activity);

        			if (regid.isEmpty()) {
        				registerInBackground();
        			}
        			else
        			{
        				onRegisterForRemoteNotifications(regid,"");
        			}
        		} else {
        			Log.i(TAG, "No valid Google Play Services APK found.");
        		}
            }
        });
	}
	
	public static void unregisterForRemoteNotification() {

		Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            public void run() {
        		if (checkPlayServices()) {
        			unregisterInBackground();
        		} else {
        			Log.i(TAG, "No valid Google Play Services APK found.");
        		}
            }
        });
	}
	
	/**
     * Check the device to make sure it has the Google Play Services APK. If
     * it doesn't, display a dialog that allows users to download the APK from
     * the Google Play Store or enable it in the device's system settings.
     */
    private static boolean checkPlayServices() {
        int resultCode = GooglePlayServicesUtil.isGooglePlayServicesAvailable(activity);
        if (resultCode != ConnectionResult.SUCCESS) {
            if (GooglePlayServicesUtil.isUserRecoverableError(resultCode)) {
                GooglePlayServicesUtil.getErrorDialog(resultCode, Cocos2dxHelper.getActivity(),
                        PLAY_SERVICES_RESOLUTION_REQUEST).show();
            } else {
                Log.i(TAG, "This device is not supported.");
                //finish();
            }
            return false;
        }
        return true;
    }

    /**
     * Stores the registration ID and the app versionCode in the application's
     * {@code SharedPreferences}.
     *
     * @param context application's context.
     * @param regId registration ID
     */
    private static void storeRegistrationId(Context context, String regId) {
        final SharedPreferences prefs = getRemoteSharedPreferences();
        int appVersion = getAppVersion(context);
        Log.i(TAG, "Saving regId on app version " + appVersion);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putString(PROPERTY_REG_ID, regId);
        editor.putInt(PROPERTY_APP_VERSION, appVersion);
        editor.commit();
    }

    /**
     * Gets the current registration ID for application on GCM service, if there is one.
     * <p>
     * If result is empty, the app needs to register.
     *
     * @return registration ID, or empty string if there is no existing
     *         registration ID.
     */
    private static String getRegistrationId(Context context) {
        final SharedPreferences prefs = getRemoteSharedPreferences();
        String registrationId = prefs.getString(PROPERTY_REG_ID, "");
        if (registrationId.isEmpty()) {
            Log.i(TAG, "Registration not found.");
            return "";
        }
        // Check if app was updated; if so, it must clear the registration ID
        // since the existing regID is not guaranteed to work with the new
        // app version.
        int registeredVersion = prefs.getInt(PROPERTY_APP_VERSION, Integer.MIN_VALUE);
        int currentVersion = getAppVersion(context);
        if (registeredVersion != currentVersion) {
            Log.i(TAG, "App version changed.");
            return "";
        }
        return registrationId;
    }

    /**
     * Registers the application with GCM servers asynchronously.
     * <p>
     * Stores the registration ID and the app versionCode in the application's
     * shared preferences.
     */
    private static void registerInBackground() {
    	Runnable r = new Runnable()
    	{
    	    @Override
    	    public void run()
    	    {
    	    	try {
                    if (gcm == null) {
                        gcm = GoogleCloudMessaging.getInstance(activity);
                    }
                    regid = gcm.register(activity.getString(activity.getResources().getIdentifier("app_id", "string", activity.getPackageName())));

                    onRegisterForRemoteNotifications(regid,"");

                    // Persist the regID - no need to register again.
                    storeRegistrationId(activity, regid);
                } catch (IOException ex) {
                    onRegisterForRemoteNotifications(null, ex.getMessage());
                    // If there is an error, don't just keep trying to register.
                    // Require the user to click a button again, or perform
                    // exponential back-off.
                }
    	    }
    	};

    	Thread t = new Thread(r);
    	t.start();
    }
    
    private static void unregisterInBackground() {
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                try {
                    if (gcm == null) {
                        gcm = GoogleCloudMessaging.getInstance(activity);
                    }
                    gcm.unregister();

                    // clear the regID
                    storeRegistrationId(activity, "");
                } catch (IOException ex) {
                }
				return null;
            }
        }.execute(null, null, null);
    }
    
    /**
     * @return Application's version code from the {@code PackageManager}.
     */
    private static int getAppVersion(Context context) {
        try {
            PackageInfo packageInfo = context.getPackageManager()
                    .getPackageInfo(context.getPackageName(), 0);
            return packageInfo.versionCode;
        } catch (NameNotFoundException e) {
            // should never happen
            throw new RuntimeException("Could not get package name: " + e);
        }
    }
    
    /**
     * Persist the information of this alarm to the Android Shared Preferences.
     * This will allow the application to restore the alarm upon device reboot.
     * Also this is used by the cancelAll method.
     *
     * @param alarmId
     *            The Id of the notification that must be persisted.
     * @param args
     *            The assumption is that parse has been called already.
     */
    static void persist (int alarmId, JSONObject args) {
        Editor editor = getLocalSharedPreferences().edit();

        editor.putString("" +alarmId, args.toString());
        editor.apply();
    }

    /**
     * Remove a specific alarm from the Android shared Preferences.
     *
     * @param alarmId
     *            The Id of the notification that must be removed.
     */
    static void unpersist (int alarmId) {
        Editor editor = getLocalSharedPreferences().edit();

        editor.remove("" + alarmId);
        editor.apply();
    }

    /**
     * Clear all alarms from the Android shared Preferences.
     */
    static void unpersistAll () {
        Editor editor = getLocalSharedPreferences().edit();

        editor.clear();
        editor.apply();
    }
	
	static boolean getApplicationActive () {
		return Cocos2dxHelper.isActivityVisible();
    }

    /**
     * The Local storage for local notifications.
     */
    protected static SharedPreferences getLocalSharedPreferences () {
        return activity.getSharedPreferences(LOCAL_NOTIFICAION_STORE, Context.MODE_PRIVATE);
    }
    
    /**
     * The Local storage for remote notifications.
     */
    protected static SharedPreferences getRemoteSharedPreferences () {
        return activity.getSharedPreferences(REMOTE_NOTIFICAION_STORE, Context.MODE_PRIVATE);
    }

    /**
     * The alarm manager for the application.
     */
    protected static AlarmManager getAlarmManager () {
        return (AlarmManager) activity.getSystemService(Context.ALARM_SERVICE);
    }

    /**
     * The notification manager for the application.
     */
    protected static NotificationManager getNotificationManager () {
        return (NotificationManager) activity.getSystemService(Context.NOTIFICATION_SERVICE);
    }
}
