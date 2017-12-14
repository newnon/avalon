/*
 * Copyright 2013 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.avalon.notifications;

import com.google.android.gms.gcm.GoogleCloudMessaging;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;

//public class GcmBroadcastReceiver extends WakefulBroadcastReceiver {
public class GcmBroadcastReceiver extends BroadcastReceiver {

	public static final String TAG = "GcmBroadcastReceiver";
	public static final int NOTIFICATION_ID = -1;
	
    @Override
    public void onReceive(Context context, Intent intent) {
    	// The application context needs to be set as first
    	Notifications.setContext(context);
        Bundle extras = intent.getExtras();
        GoogleCloudMessaging gcm = GoogleCloudMessaging.getInstance(context);
        // The getMessageType() intent parameter must be the intent you received
        // in your BroadcastReceiver.
        String messageType = gcm.getMessageType(intent);

        if (!extras.isEmpty()) {  // has effect of unparcelling Bundle
            /*
             * Filter messages based on message type. Since it is likely that GCM will be
             * extended in the future with new message types, just ignore any message types you're
             * not interested in, or that you don't recognize.
             */
            if (GoogleCloudMessaging.MESSAGE_TYPE_SEND_ERROR.equals(messageType)) {
            	Log.i(TAG, "Send error: " + extras.toString());
            } else if (GoogleCloudMessaging.MESSAGE_TYPE_DELETED.equals(messageType)) {
            	Log.i(TAG, "Deleted messages on server: " + extras.toString());
            // If it's a regular GCM message, do some work.
            } else if (GoogleCloudMessaging.MESSAGE_TYPE_MESSAGE.equals(messageType)) {
                // This loop represents the service doing some work.
                Log.i(TAG, "Completed work @ " + SystemClock.elapsedRealtime());
                // Post notification of received message.
                Log.i(TAG, "Received: " + extras.toString());
                sendNotification(context, extras);
            }
        }
        setResultCode(Activity.RESULT_OK);
    }

    // Put the message into a notification and post it.
    private void sendNotification(Context context, Bundle bundle) {
    	
    	String message = bundle.getString("message");
    	String sound = bundle.getString("sound");
    	int badgeNumber = bundle.getInt("badgeNumber");
    	
        Notifications.showNotification(context, NOTIFICATION_ID, message, sound, badgeNumber, bundle);
        Notifications.onRemoteNotification(message, sound, badgeNumber);
    }
}
