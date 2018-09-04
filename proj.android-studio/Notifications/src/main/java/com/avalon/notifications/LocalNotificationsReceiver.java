package com.avalon.notifications;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

public class LocalNotificationsReceiver extends BroadcastReceiver {
	
	@Override
	public void onReceive(Context context, Intent intent) {
		// The application context needs to be set as first
    	Notifications.setContext(context);
		
		int notificationId = Integer.parseInt(intent.getAction());
		Bundle bundle = intent.getBundleExtra("notification");

		String message = bundle.getString("message");
		String title = bundle.getString("title");
		String sound = bundle.getString("sound");
		int badge = bundle.getInt("badge",0);
		
		Notifications.showNotification(context, notificationId, message, title, sound, badge, bundle);
		
		Notifications.unpersist(notificationId);
		Notifications.onLocalNotification(notificationId, message, title, sound, badge);
	}
}
