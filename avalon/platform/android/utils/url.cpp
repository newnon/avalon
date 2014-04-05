#include <avalon/url.h>

#include <jni.h>
#include "platform/android/jni/JniHelper.h"

namespace avalon {
namespace web {

void openUrl( const std::string &url)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/avalon/utils/Url", "open", "(Ljava/lang/String;)V")) {
        jstring jUrl = t.env->NewStringUTF(url.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, jUrl);
        t.env->DeleteLocalRef(t.classID);

        t.env->DeleteLocalRef(jUrl);
    }
}

} // namespace web
} // namespace avalon

