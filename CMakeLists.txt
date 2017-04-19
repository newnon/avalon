cmake_minimum_required(VERSION 2.6)

option(AVALON_PAYMENT "payment" OFF)
option(AVALON_GAMECENTER "gamecenter" OFF)
option(AVALON_APPIRATER "appirater" OFF)
option(AVALON_WEB "web" OFF)
option(AVALON_FLURRY "flurry" OFF)
option(AVALON_FLURRYADS "flurryads" OFF)
option(AVALON_NOTIFICATIONS "notifications" OFF)
option(AVALON_ADMOB "admob" OFF)
option(AVALON_GOOGLEANALYTICS "googleanalytics" OFF)
option(AVALON_MOPUB "mopub" OFF)
option(AVALON_UI "ui" OFF)
option(AVALON_ADS "ads" OFF)
option(AVALON_SYSTEMINFO "systeminfo" OFF)
option(AVALON_FACEBOOK "facebook" OFF)
option(AVALON_VKONTAKTE "vkontakte" OFF)

if(NOT MSVC)# all gcc
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(WIN32) # Win32
  set(PLATFORM_FOLDER windows)
elseif(APPLE)# osx or ios
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(PLATFORM_FOLDER mac)
else()
  set(PLATFORM_FOLDER ios)
endif()
elseif(ANDROID)# Android
  set(PLATFORM_FOLDER android)
elseif(EMSCRIPTEN)# Emscripten
  set(PLATFORM_FOLDER emcc)
endif()

set(AVALON_SRC avalon/empty.cpp)

if(AVALON_PAYMENT)
add_definitions(-DAVALON_CONFIG_PAYMENT_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/payment/payment.cpp
)
endif()

if(AVALON_GAMECENTER)
add_definitions(-DAVALON_CONFIG_GAMECENTER_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/gamecenter/GameCenter.cpp
)
endif()

if(AVALON_APPIRATER)
add_definitions(-DAVALON_CONFIG_APPIRATER_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/appirater/Appirater.cpp
)
endif()

if(AVALON_WEB)
add_definitions(-DAVALON_CONFIG_WEB_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/web/url.cpp
  avalon/platform/${PLATFORM_FOLDER}/web/WebView.cpp
)
endif()

if(AVALON_FLURRY)
add_definitions(-DAVALON_CONFIG_FLURRY_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/flurry/flurry.cpp
)
endif()

if(AVALON_FLURRYADS)
add_definitions(-DAVALON_CONFIG_FLURRYADS_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/flurryads/flurryads.cpp
)
endif()

if(AVALON_NOTIFICATIONS)
add_definitions(-DAVALON_CONFIG_NOTIFICATIONS_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/notifications/notifications.cpp
)
endif()

if(AVALON_ADS)
add_definitions(-DAVALON_CONFIG_ADS_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/ads/AdsManager.cpp
)
endif()

if(AVALON_ADMOB)
add_definitions(-DAVALON_CONFIG_ADMOB_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/admob/admob.cpp
)
endif()

if(AVALON_GOOGLEANALYTICS)
add_definitions(-DAVALON_CONFIG_GOOGLEANALYTICS_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/googleanalytics/GoogleAnalytics.cpp
)
endif()

if(AVALON_MOPUB)
add_definitions(-DAVALON_CONFIG_MOPUB_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/mopub/MoPub.cpp
)
endif()

if(AVALON_UI)
add_definitions(-DAVALON_CONFIG_UI_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/ui/AlertNative.cpp
)
endif()

if(AVALON_SYSTEMINFO)
add_definitions(-DAVALON_CONFIG_SYSTEMINFO_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/systeminfo/SystemInfo.cpp
)
endif()

if(AVALON_FACEBOOK)
add_definitions(-DAVALON_CONFIG_FACEBOOK_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/facebook/facebook.cpp
)
endif()

if(AVALON_VKONTAKTE)
add_definitions(-DAVALON_CONFIG_VKONTAKTE_ENABLED)
set(AVALON_SRC
  ${AVALON_SRC}
  avalon/platform/${PLATFORM_FOLDER}/vkontakte/VKSocialPlugin.cpp
)
endif()

add_library(avalon STATIC
  ${AVALON_SRC}
)

include_directories(
  .
  ../cocos2d/cocos
)

set_target_properties(avalon
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

