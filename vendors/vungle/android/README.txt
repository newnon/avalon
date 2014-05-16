 __      __                _      _____       _     
 \ \    / /               | |    |  __ \     | |    
  \ \  / /   _ _ __   __ _| | ___| |__) |   _| |__  
   \ \/ / | | | '_ \ / _` | |/ _ \  ___/ | | | '_ \ 
    \  /| |_| | | | | (_| | |  __/ |   | |_| | |_) |
     \/  \__,_|_| |_|\__, |_|\___|_|    \__,_|_.__/ 
                      __/ |                         
                     |___/                          

Version 1.3.11


== QUICK INTRODUCTION FOR ECLIPSE DEVELOPMENT =====

First things first, we need to copy the vungle-publisher-1.3.11.jar file into your project. It's important that you put the library in a directory called 'libs'. This 'libs' folder needs to be at the root level of your project's directory. Then press F5 in Eclipse (or right-click your project in the Package Explorer and click 'refresh') to make sure the Eclipse is aware that the vungle-publisher-1.3.11.jar file exists.

Next, we'll make sure that Eclipse knows how to use the jar file. Open up your project's Properties Dialog by right clicking your project, and clicking "Properties". Then, click the "Java Build Path" item on the left of the window. Next, click into the "Libraries" tab. Once in there, click the "Add JARs..." button. Navigate to the 'libs' directory, and select the vungle-publisher-1.3.11.jar file, and click "OK". Then, click "OK" to leave the Project Properties.



== UPDATING THE ANDROID MANIFEST =====

VunglePub needs a few permissions to operate correctly. Open up your AndroidManifest.xml file, and make sure that your program has the following permission tags inside the outer manifest tag:

	<uses-permission android:name="android.permission.INTERNET" />
	<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
	<uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
	<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />

NOTE: The ACCESS_WIFI_STATE is optional, but recommended.

VunglePub opens advertisements in a separate activity. Be sure to add the following activity to the correct <application> tag:

	<activity
		android:name="com.vungle.sdk.VungleAdvert"
		android:configChanges="keyboardHidden|orientation|screenSize"
		android:theme="@android:style/Theme.NoTitleBar.Fullscreen"
	/>

NOTE: The 'configChanges' entry has the value 'screenSize' in it. This value was introduced Android 3.2 (API level 13). If your target API version is less than 13, then Eclipse will complain about this value. To fix this, you can either:

 * Raise your target version to 13 or higher
 * Omit the screenSize entry from the configChanges list

Raising your target API version is highly recommended, however, since it will make your app behave more naturally on modern Android devices, while still being compatible with older devices.

In addition, VunglePub sends network requests in the background using an IntentService, which you should also add to the <application> tag:

  <service android:name="com.vungle.sdk.VungleIntentService"/>


== INITIALIZING THE LIBRARY =====

Now it's time for some actual code. To initialize the VunglePub library, you need to do 3 things:
 
 1. Import the library
 2. Call one of the init(...) procedures
 3. Hook the VunglePub library into your activities

To import the library, you can either type in:

	import com.vungle.sdk.VunglePub;

Or, (if you are using Eclipse) you can press ctrl+shift+O (or command+shift+O on a mac) to automatically import the library after you start using it.

Next, we need to call one of the two init(...) procedures:
	
	VunglePub.init(Context context, String appId);
	VunglePub.init(Context context, String vungleAppId, int age, int gender);

Use the first procedure if don't have any demographic information on the user, and use the second procedure if you do.

Finally we need to hook VunglePub into the lifecycle of each of your app's activities. We do this by calling one of these procedures in the appropriate activity lifecycle events:

	VunglePub.onPause();
	VunglePub.onResume();

For example:
	
	@Override
	protected void onResume() {
		super.onResume();
		VunglePub.onResume();
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		VunglePub.onPause();
	}

Once all of these steps are complete, we can start displaying ads.



== DISPLAYING ADVERTISEMENTS =====

To spawn an advertisement, call one of these functions:

	boolean VunglePub.displayAdvert();
	boolean VunglePub.displayIncentivizedAdvert(boolean showCloseButton);
	boolean VunglePub.displayIncentivizedAdvert(String name, boolean showCloseButton);

The first function will spawn an interstitial ad. The second two functions spawn incentivized ads. The difference is that an incentivized ad can optionally hide the 'close' button from the user during the video, and the second incentivized function allows for a 'name' parameter, which can be used to identify the users on our server. All of these functions return a boolean, indicating whether the ad was started or not.

If you wish to check to see if an ad is available, without actually showing an ad, you can use this function:

	boolean VunglePub.isVideoAvailable();

For debugging purposes, it might be useful to know the reason why a video is or isn't available. To get that info, try:

	boolean VunglePub.isVideoAvailable(true);

When 'true' is passed into isVideoAvailable(), then the reason why a video is or isn't available is printed into LogCat.



== CHANGINE THE VIDEO ROTATION BEHAVIOR =====

By default, VunglePub will display video advertisements in the video's orientation. While this utilizes the screen most effectively, some people may prefer having the ads always play in the user's orientation. To achieve that, you can use this procedure:

	VunglePub.setAutoRotation(boolean shouldAutoRotate);

Calling that procedure with 'true' will make the video rotate to the user's orientation, so that the video is always facing up. Calling that procedure with 'false' will return VunglePub to its default behavior, where the video will rotate the video to get the best fit on the screen. 



== CHANGING THE SOUND BEHAVIOR =====

By default, Vungle advertisements will keep the same volume level and mute state as the hosting application. However, this isn't always appropriate, like in quiet games, or if the user has disabled sound in the app, but the sound isn't actually muted. For these cases, we have this procedure:

	VunglePub.setSoundEnabled(boolean isSoundEnabled);

When you call that function with 'false', all advertisements will be started in the 'muted' state. Note that users can still unmute the ads if they wish. Calling the function with 'true' will enable the default behavior, where the ads will have the same volume as the enclosing app.

If you wish to see VunglePub's current sound behavior, you can use the following function:

	boolean VunglePub.getSoundEnabled();



== CHANGING THE BACK BUTTON BEHAVIOR =====

By default, the Vungle SDK will suppress the 'back' button during the video part of the advertisement. Instead, it has an X button that will appear after a certain amount of time has passed, and will allow the user to dismiss the video. The reason for this is that if we allow the use of the back button to dismiss the advertisement, users can get in the habit of immediately dismissing all adverts, without even giving them the chance to grab their attention.

If you disagree with this default, and you want to enable the back button for advertisements, you can use one or both the following setters:

	// Will enable the back button for interstitial ads:
	VunglePub.setBackButtonEnabled(boolean isBackButtonEnabled);

	// Will enable the back button for incentivized ads:
	VunglePub.setIncentivizedBackButtonEnabled(boolean isBackButtonEnabled);

Do note: changing either of these variables to 'true' may have an adverse reaction to your revenue.



== LISTENING FOR EVENTS =====

If you want to know when certain advertisement events happen, look into the VunglePub.EventListener interface. You can hand an implementation of that interface into the VunglePub library with:
	
	VunglePub.setEventListener(VunglePub.EventListener el);

For example:
	
	VunglePub.setEventListener(new VunglePub.EventListener() {
		
		// This event happens whenever VunglePub decides to play an ad:
		@Override
		public void onVungleAdStart() {
			Log.i("MyApp", "An advertisement just started.");
		}
		
		// This event happens whenever a user has exited an ad, and is not back in your application:
		@Override
		public void onVungleAdEnd() {
			Log.i("MyApp", "An advertisement just ended.");
		}
		
		// This event happens whenever a user has exited an ad, and has watched at least SOME portion of it.
		// This procedure is not called if the user opts out of an ad before it starts, and thus has watched no portion of the video:
		@Override
		public void onVungleView(double watched, double length) {
			Log.i("MyApp", "Someone just watched " + watched + " seconds of a " + length + " second ad.");
		}
	});

Do note: when each of those overridden procedures are called, they are called on a new thread. So, if you want this callback to tinker with the UI thread, you need to use some sort of synchronization procedure. Android "Handler" objects might come in handy. You can also try the `Activity.runOnUiThread(Runnable r)` procedure.

See:
	http://developer.android.com/reference/android/os/Handler.html
	and
	http://developer.android.com/reference/android/app/Activity.html#runOnUiThread(java.lang.Runnable)
