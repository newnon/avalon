//
//  Amplitude.cpp
//  Amplitude
//
//  Created by Vladimir Shlobin on 24.04.2018.
//

#include <platform/android/jni/JniHelper.h>
#include "avalon/Amplitude.h"

namespace avalon { namespace amplitude {

const auto HELPER_CLASS_NAME = "com/avalon/amplitude/AmplitudeHelper";

static jobject jobjectFromDictionary(const std::map<std::string,std::string> &dictionary)
{
    JNIEnv* env = cocos2d::JniHelper::getEnv();
    jclass mapClass = env->FindClass("java/util/HashMap");
    if(mapClass == NULL)
    {
        return NULL;
    }

    jsize map_len = 1;

    jmethodID init = env->GetMethodID(mapClass, "<init>", "(I)V");
    jobject hashMap = env->NewObject(mapClass, init, map_len);

    jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for(auto &it : dictionary)
    {
        jstring key = env->NewStringUTF(it.first.c_str());
        jstring value = env->NewStringUTF(it.second.c_str());

        env->CallObjectMethod(hashMap, put, key, value);

        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(mapClass);
    return hashMap;
}

void init()
{
	cocos2d::JniMethodInfo methodInfo;
	if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "init", "()V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void logEvent(const std::string &name, const std::map<std::string, std::string> &params)
{
	cocos2d::JniMethodInfo methodInfo;
	if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "logEvent", "(Ljava/lang/String;Ljava/util/Map;)V"))
	{
		jstring jName = methodInfo.env->NewStringUTF(name.c_str());
		jobject jParams = jobjectFromDictionary(params);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jName, jParams);
		methodInfo.env->DeleteLocalRef(jName);
		methodInfo.env->DeleteLocalRef(jParams);
	}
}

void logRevenue(const std::string &productId, int quantity, double price, const std::string &revenueType, const std::string &receipt, const std::string &receiptSig, const std::map<std::string, std::string> &params)
{
	cocos2d::JniMethodInfo methodInfo;
	if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "logRevenue", "(Ljava/lang/String;IDLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V"))
	{
		jstring jProductId = productId.empty() ? nullptr : methodInfo.env->NewStringUTF(productId.c_str());
		jint jQuantity = quantity;
		jdouble jPrice = price;
		jstring jRevenueType = revenueType.empty() ? nullptr : methodInfo.env->NewStringUTF(revenueType.c_str());
		jstring jReceipt = receipt.empty() ? nullptr : methodInfo.env->NewStringUTF(receipt.c_str());
		jstring jReceiptSig = receiptSig.empty() ? nullptr : methodInfo.env->NewStringUTF(receiptSig.c_str());
		jobject jParams = jobjectFromDictionary(params);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jProductId, jQuantity, jPrice, jRevenueType, jReceipt, jReceiptSig, jParams);
		methodInfo.env->DeleteLocalRef(jParams);
		methodInfo.env->DeleteLocalRef(jReceiptSig);
		methodInfo.env->DeleteLocalRef(jReceipt);
		methodInfo.env->DeleteLocalRef(jRevenueType);
		methodInfo.env->DeleteLocalRef(jProductId);
	}
}

} } // namespace avalon::amplitude
