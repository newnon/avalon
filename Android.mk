LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# inject the used flavor into the c++ preprocessor. please keep this block
# up to date with <avalon/utils/platform.h> and <avalon/utils/platform.cpp>!
ifeq (amazon, $(AVALON_PLATFORM_FLAVOR))
    LOCAL_CFLAGS += -DAVALON_PLATFORM_FLAVOR=1
    LOCAL_EXPORT_CFLAGS += -DAVALON_PLATFORM_FLAVOR=1
else ifeq (google, $(AVALON_PLATFORM_FLAVOR))
    LOCAL_CFLAGS += -DAVALON_PLATFORM_FLAVOR=2
    LOCAL_EXPORT_CFLAGS += -DAVALON_PLATFORM_FLAVOR=2
else ifeq (samsung, $(AVALON_PLATFORM_FLAVOR))
    LOCAL_CFLAGS += -DAVALON_PLATFORM_FLAVOR=3
    LOCAL_EXPORT_CFLAGS += -DAVALON_PLATFORM_FLAVOR=3
else
    $(error ERROR: variable AVALON_PLATFORM_FLAVOR not set)
endif

LOCAL_MODULE := avalon_static
LOCAL_MODULE_FILENAME := libavalon

LOCAL_SRC_FILES := avalon/utils/platform.cpp

ifneq (,$(findstring adsmanager,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_ADS_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_ADS_ENABLED
    LOCAL_SRC_FILES += avalon/ads/Manager.cpp
    LOCAL_SRC_FILES += avalon/platform/android/ads/provider/Chartboost.cpp
    LOCAL_SRC_FILES += avalon/platform/android/ads/provider/Revmob.cpp
    LOCAL_SRC_FILES += avalon/platform/android/ads/provider/SamsungAdHub.cpp
endif

ifneq (,$(findstring payment,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_PAIMENT_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_PAIMENT_ENABLED
    LOCAL_SRC_FILES += avalon/payment/Loader.cpp
    LOCAL_SRC_FILES += avalon/payment/Manager.cpp
    LOCAL_SRC_FILES += avalon/payment/Product.cpp
    LOCAL_SRC_FILES += avalon/payment/ProductConsumable.cpp
    LOCAL_SRC_FILES += avalon/platform/android/payment/Backend.cpp
    LOCAL_SRC_FILES += avalon/io/IniReader.cpp
endif

ifneq (,$(findstring gamecenter,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_GAMECENTER_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_GAMECENTER_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/gamecenter/GameCenter.cpp
endif

ifneq (,$(findstring appirater,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_APPIRATER_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_APPIRATER_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/appirater/Appirater.cpp
endif

ifneq (,$(findstring web,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_WEB_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_WEB_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/utils/url.cpp
    LOCAL_SRC_FILES += avalon/platform/android/webview/WebView.cpp
endif

ifneq (,$(findstring flurry,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_FLURRY_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_FLURRY_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/flurry/flurry.cpp
endif

ifneq (,$(findstring flurryads,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_FLURRYADS_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_FLURRYADS_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/flurryads/flurryads.cpp
endif

ifneq (,$(findstring notifications,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_NOTIFICATIONS_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_NOTIFICATIONS_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/notifications/notifications.cpp
endif

ifneq (,$(findstring admob,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_ADMOB_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_ADMOB_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/admob/admob.cpp
endif

ifneq (,$(findstring googleanalytics,$(AVALON_FEATURES)))
    LOCAL_CFLAGS += -DAVALON_CONFIG_GOOGLEANALYTICS_ENABLED
    LOCAL_EXPORT_CFLAGS += -DAVALON_CONFIG_GOOGLEANALYTICS_ENABLED
    LOCAL_SRC_FILES += avalon/platform/android/googleanalytics/GoogleAnalytics.cpp
endif

ifeq (google, $(AVALON_PLATFORM_FLAVOR))
    LOCAL_SRC_FILES += avalon/platform/android-google/ads/provider/TapForTap.cpp
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

# enable c++11 support but "remove" the override specifier with a simple
# preprocessor define - it's not supported yet :(
LOCAL_CFLAGS += -std=c++11 -Doverride= -fexceptions

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,2d)
