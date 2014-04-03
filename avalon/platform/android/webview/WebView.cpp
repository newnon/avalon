//
//  ZYWebView.cpp
//  CCXWebview
//
//  Created by Vincent on 12-11-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "avalon/WebView.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>

const char* kJNIPakageName = "org/cocos2dx/cpp/AppActivity";

#define  LOG_TAG    "WebView Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace avalon {
namespace web {

WebView::WebView()
{

}

WebView::~WebView()
{
    removeWebView();
}

void WebView::showWebView(const std::string &url, float x, float y, float width, float height)
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(minfo,kJNIPakageName,"displayWebView", "(IIII)V"))
    {
        jint jX = (int)x;
        jint jY = (int)y;
        jint jWidth = (int)width;
        jint jHeight = (int)height;
         minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jX, jY, jWidth, jHeight);
         minfo.env->DeleteLocalRef(minfo.classID);
    }
    else
    {
        LOGD("jni:displayWebView not found");
    }
    
    if(cocos2d::JniHelper::getStaticMethodInfo(minfo,kJNIPakageName,"updateURL", "(Ljava/lang/String;)V"))
    {
        jstring jmsg = minfo.env->NewStringUTF(url.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jmsg);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    else
    {
        LOGD("jni:updateURL not found");
    }
}

void WebView::updateURL(const std::string &url)
{
    cocos2d::JniMethodInfo minfo;
    
    if(cocos2d::JniHelper::getStaticMethodInfo(minfo,kJNIPakageName,"updateURL", "(Ljava/lang/String;)V"))
    {
        jstring jmsg = minfo.env->NewStringUTF(url.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jmsg);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    else
    {
        LOGD("jni:updateURL not found");
    }
}

void WebView::removeWebView()
    {
    cocos2d::JniMethodInfo minfo;
    
    if(cocos2d::JniHelper::getStaticMethodInfo(minfo,kJNIPakageName,"removeWebView", "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    else
    {
        LOGD("jni:updateURL failed");
    }
}

} // namespace web
} // namespace avalon


