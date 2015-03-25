/****************************************************************************
 Copyright (c) 2014      Jesús Sánchez
 
 http://jesanchezgo.hostwp.es
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "ChartboostJni.h"
#include "avalon/Chartboost.h"

#define  CLASS_NAME "com/avalon/Chartboost/ChartboostBridge"

using namespace cocos2d;

extern "C"
{
    
    void startWithAppId(const char * appId, const char * appSignature)
    {
        JniMethodInfo methodInfo;
        
        if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "startWithAppId", "(Ljava/lang/String;Ljava/lang/String)V"))
        {
            return;
        }
        jstring stringArg1 = methodInfo.env->NewStringUTF(appId);
        jstring stringArg2 = methodInfo.env->NewStringUTF(appSignature);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg1, stringArg2);
        
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(stringArg1);
        methodInfo.env->DeleteLocalRef(stringArg2);
    }
    
    void cacheInterstitialJNI(const char* location)
    {
        JniMethodInfo methodInfo;
        
        if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "cacheInterstitial", "(Ljava/lang/String;)V"))
        {
            return;
        }
        jstring stringArg = methodInfo.env->NewStringUTF(location);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
            
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(stringArg);
    }
    
    void showInterstitialJNI(const char* location)
    {
        JniMethodInfo methodInfo;


		if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showInterstitial", "(Ljava/lang/String;)V"))
		{
			return;
		}
		jstring stringArg = methodInfo.env->NewStringUTF(location);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(stringArg);
    }
    
    bool hasInterstitialJNI(const char* location)
    {
        JniMethodInfo methodInfo;
        

		if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "hasInterstitial", "(Ljava/lang/String;)Z"))
		{
			return false;
		}
		jstring stringArg = methodInfo.env->NewStringUTF(location);
		jboolean retV = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(stringArg);

		return retV;
    }

    void cacheRewardedVideoJNI(const char* location)
	{
		JniMethodInfo methodInfo;

		if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "cacheRewardedVideo", "(Ljava/lang/String;)V"))
		{
			return;
		}
		jstring stringArg = methodInfo.env->NewStringUTF(location);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(stringArg);
	}

	void showRewardedVideoJNI(const char* location)
	{
		JniMethodInfo methodInfo;

		if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showRewardedVideo", "(Ljava/lang/String;)V"))
		{
			return;
		}
		jstring stringArg = methodInfo.env->NewStringUTF(location);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(stringArg);
	}

	bool hasRewardedVideoJNI(const char* location)
	{
		JniMethodInfo methodInfo;


		if (! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "hasRewardedVideo", "(Ljava/lang/String;)Z"))
		{
			return false;
		}
		jstring stringArg = methodInfo.env->NewStringUTF(location);
		jboolean retV = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
		methodInfo.env->DeleteLocalRef(stringArg);

		return retV;
	}

    jboolean Java_com_avalon_Chartboost_ChartboostBridge_shouldRequestInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            return delegate->shouldRequestInterstitial(JniHelper::jstring2string(location).c_str());
        }
        
        return true;
    }
    
    jboolean Java_com_avalon_Chartboost_ChartboostBridge_shouldDisplayInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            return delegate->shouldDisplayInterstitial(JniHelper::jstring2string(location).c_str());
        }
        
        return true;
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didDisplayInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didDisplayInterstitial(JniHelper::jstring2string(location).c_str());
        }
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didCacheInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didCacheInterstitial(JniHelper::jstring2string(location).c_str());
        }
    }
    
    void Java_com_avalon_Chartboost_ChartboostBridge_didFailToLoadInterstitial(JNIEnv*  env, jobject thiz, jstring location, jint error)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didFailToLoadInterstitial(JniHelper::jstring2string(location).c_str(), error);
        }
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didFailToRecordClick(JNIEnv*  env, jobject thiz, jstring uri, jint error)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didFailToRecordClick(JniHelper::jstring2string(uri).c_str(), error);
        }
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didDismissInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didDismissInterstitial(JniHelper::jstring2string(location).c_str());
        }
    }
    
    void Java_com_avalon_Chartboost_ChartboostBridge_didCloseInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didCloseInterstitial(JniHelper::jstring2string(location).c_str());
        }
    }
    
    void Java_com_avalon_Chartboost_ChartboostBridge_didClickInterstitial(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
            delegate->didClickInterstitial(JniHelper::jstring2string(location).c_str());
        }
    }

    jboolean Java_com_avalon_Chartboost_ChartboostBridge_shouldDisplayRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
    	if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
    		return delegate->shouldDisplayRewardedVideo(JniHelper::jstring2string(location).c_str());
    	}

    	return true;
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didDisplayRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
    	if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
    		return delegate->didDisplayRewardedVideo(JniHelper::jstring2string(location).c_str());
    	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didCacheRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didCacheRewardedVideo(JniHelper::jstring2string(location).c_str());
       	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didFailToLoadRewardedVideo(JNIEnv*  env, jobject thiz, jstring location, jint error)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didFailToLoadRewardedVideo(JniHelper::jstring2string(location).c_str(), error);
       	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didDismissRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didDismissRewardedVideo(JniHelper::jstring2string(location).c_str());
       	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didCloseRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didCloseRewardedVideo(JniHelper::jstring2string(location).c_str());
       	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didClickRewardedVideo(JNIEnv*  env, jobject thiz, jstring location)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didClickRewardedVideo(JniHelper::jstring2string(location).c_str());
       	}
    }

    void Java_com_avalon_Chartboost_ChartboostBridge_didCompleteRewardedVideo(JNIEnv*  env, jobject thiz, jstring location, jint reward)
    {
        if (avalon::ChartboostDelegate* delegate = avalon::Chartboost::getInstance()->getDelegate()) {
       		return delegate->didCompleteRewardedVideo(JniHelper::jstring2string(location).c_str(), reward);
       	}
    }
}
