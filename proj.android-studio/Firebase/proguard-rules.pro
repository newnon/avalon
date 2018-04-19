# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in E:\developSoftware\Android\SDK/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

-dontobfuscate

# From firebase_cpp_sdk/libs/android/analytics.pro
-keep,includedescriptorclasses public class com.google.firebase.analytics.FirebaseAnalytics{ *; }

# From firebase_cpp_sdk/libs/android/app.pro
-keep,includedescriptorclasses public class com.google.android.gms.common.GoogleApiAvailability{ *; }
-keep,includedescriptorclasses public class com.google.android.gms.crash.internal.api.CrashApiImpl { *; }
-keep,includedescriptorclasses public class com.google.android.gms.tasks.OnFailureListener { *; }
-keep,includedescriptorclasses public class com.google.android.gms.tasks.OnSuccessListener { *; }
-keep,includedescriptorclasses public class com.google.android.gms.tasks.Task { *; }
-keep,includedescriptorclasses public class com.google.firebase.FirebaseApp{ *; }
-keep,includedescriptorclasses public class com.google.firebase.FirebaseOptions{ *; }
-keep,includedescriptorclasses public class com.google.firebase.FirebaseOptions$Builder{ *; }
-keep,includedescriptorclasses public class com.google.firebase.iid.FirebaseInstanceId{ *; }
-keep,includedescriptorclasses public class dalvik.system.DexClassLoader{ *; }
