package com.avalon.notifications;

import java.util.Set;

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
            	int time = args.getInt("time");
            	int badgeNumber = args.getInt("badge");

                /*
                 * If the trigger date was in the past, the notification will be displayed immediately.
                 */
                Notifications.add(message, sound, time, Integer.parseInt(alarmId), badgeNumber);

            } catch (Exception e) {}
        }
    }
}