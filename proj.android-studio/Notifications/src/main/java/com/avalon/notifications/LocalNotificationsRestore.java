package com.avalon.notifications;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;

/**
 * This class is triggered upon reboot of the device. It needs to re-register
 * the alarms with the AlarmManager since these alarms are lost in case of
 * reboot.
 */
public class LocalNotificationsRestore extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
    	// The application context needs to be set as first
    	Notifications.setContext(context);
    	
        // Obtain alarm details form Shared Preferences
        SharedPreferences alarms = Notifications.getLocalSharedPreferences();
        Set<String> alarmIds     = alarms.getAll().keySet();

        /*
         * For each alarm, parse its alarm options and register is again with
         * the Alarm Manager
         */
        for (String alarmId : alarmIds) {
            try {
            	JSONObject args  = new JSONObject(alarms.getString(alarmId, ""));
            	
            	String message = args.getString("message");
            	String sound = args.getString("sound");
                String title = args.getString("title");
            	int time = args.getInt("time");
            	int badgeNumber = args.getInt("badge");

                HashMap<String, String> params = new HashMap<String, String>();

                try {
                    JSONObject jsonParams = args.getJSONObject("params");
                    Iterator<?> keys = jsonParams.keys();

                    while (keys.hasNext()) {
                        String key = (String) keys.next();
                        String value = args.getString(key);
                        params.put(key, value);
                    }
                } catch (JSONException e) {}

                /*
                 * If the trigger date was in the past, the notification will be displayed immediately.
                 */
                Notifications.add(message, title, sound, time, Integer.parseInt(alarmId), badgeNumber, params);

            } catch (Exception e) {}
        }
    }
}