package com.avalon.notifications;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class LocalNotificationsReceiver extends BroadcastReceiver {
	
	@Override
	public void onReceive(Context context, Intent intent) {
		// The application context needs to be set as first
    	Notifications.setContext(context);
		
		int notificationId = Integer.parseInt(intent.getAction());
		String message = intent.getStringExtra("message");
		String sound = intent.getStringExtra("sound");
		int badge = intent.getIntExtra("badge",0); 
		
		Notifications.showNotification(context, notificationId, message, sound, badge);
		
		Notifications.unpersist(notificationId);
		Notifications.onLocalNotification(message,sound,badge);
	}
}
