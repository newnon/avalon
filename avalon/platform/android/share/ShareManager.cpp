//
//  ShareManager.cpp
//
//

#include "avalon/ShareManager.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccUtils.h"

const char* const CLASS_NAME = "com/avalon/share/ShareHelper";

#define  LOG_TAG    "ShareManager Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

namespace avalon {
    
class AndroidShareManager : public ShareManager
{
public:
    virtual void setAppId(const std::string& appId)
    {
        _appId = appId;
    }
    
    virtual void setHashTag(const std::string& hashTag)
    {
        _hashTag = hashTag;
    }

    virtual void share(const std::string& text, const std::string& image)
    {
        share(text, text, image);
    }
    
    virtual void shareScreenshot(const std::string& text)
    {
        shareScreenshot(text, text);
    }

    void afterCaptured(const std::string& text, const std::string& longText, bool succeed, const std::string& outputFile)
    {
    	if (succeed)
    	{
    		cocos2d::JniMethodInfo methodInfo;

    		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo,CLASS_NAME,"shareScreenshot", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    		{
    			jstring jtext = methodInfo.env->NewStringUTF(text.c_str());
    			jstring jlongText = methodInfo.env->NewStringUTF(longText.c_str());
    			jstring jimage = methodInfo.env->NewStringUTF(outputFile.c_str());
    			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jtext, jlongText, jimage);
    			methodInfo.env->DeleteLocalRef(jtext);
    			methodInfo.env->DeleteLocalRef(jimage);
    			methodInfo.env->DeleteLocalRef(methodInfo.classID);
    		}
    		else
    		{
    			LOGD("jni:shareFile not found");
    		}
    	}
    	else
    	{
    		LOGD("screen capture failed");
    	}
    }

    virtual void shareScreenshot(const std::string& text, const std::string& longText)
    {
    	cocos2d::utils::captureScreen(std::bind(&AndroidShareManager::afterCaptured, this, text, longText, std::placeholders::_1, std::placeholders::_2 ),"temp.png");
    }
    
    void share(const std::string& text, const std::string& longText, const std::string& image)
    {
    	cocos2d::JniMethodInfo methodInfo;

    	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo,CLASS_NAME,"shareFile", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    	{
    		jstring jtext = methodInfo.env->NewStringUTF(text.c_str());
    		jstring jlongText = methodInfo.env->NewStringUTF(longText.c_str());
    	    jstring jimage = methodInfo.env->NewStringUTF(image.c_str());
    	    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jtext, jlongText, jimage);
    	    methodInfo.env->DeleteLocalRef(jtext);
    	    methodInfo.env->DeleteLocalRef(jimage);
    	    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    	}
    	else
    	{
    	    LOGD("jni:shareFile not found");
    	}
    }
    
    AndroidShareManager()
    {
        
    }

    virtual ~AndroidShareManager()
    {
    }
    
private:
    std::string _appId;
    std::string _hashTag;
};

ShareManager* ShareManager::getInstance()
{
    static AndroidShareManager* instance = new AndroidShareManager();
    return instance;
}

} // namespace avalon
