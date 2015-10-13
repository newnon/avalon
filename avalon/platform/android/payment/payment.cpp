#include "avalon/payment.h"

#include <jni.h>
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <avalon/utils/assert.hpp>

namespace avalon {
namespace payment {

/**
 * C++ -->> Java
 */

namespace backend {
namespace helper {

const char* const CLASS_NAME = "com/avalon/payment/Backend";

void callStaticVoidMethod(const char* name)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

bool callStaticBoolMethod(const char* name)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()Z")) {
        bool result = (t.env->CallStaticBooleanMethod(t.classID, t.methodID) == JNI_TRUE);
        t.env->DeleteLocalRef(t.classID);
        return result;
    } else {
        return false;
    }
}

void callStaticVoidMethodWithString(const char* name, const char* argument)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Ljava/lang/String;)V")) {
        jstring jProductId = t.env->NewStringUTF(argument);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jProductId);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(jProductId);
    }
}

void callStaticVoidMethodWithStringAndBool(const char* name, const char* argument, const bool flag)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Ljava/lang/String;Z)V")) {
        jstring jProductId = t.env->NewStringUTF(argument);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jProductId, flag);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(jProductId);
    }
}

} // namespace helper
} // namespace backend

class AndroidManager:public Manager
{
public:
    AndroidManager():_delegate(nullptr)
    {
    }
    ~AndroidManager()
    {
    }
    virtual void addProduct(const std::string &id, const std::string &productIdentifier, bool consumable) override
    {
        Product product;
        product.id = id;
        product.productIdentifier = productIdentifier;
        product.consumable = consumable;
        addProduct(product);
    }
    virtual void addProduct(const Product &product) override
    {
        _products.push_back(product);
    }

    virtual void clearProducts() override
    {
    	_products.clear();
    	backend::helper::callStaticVoidMethod("clearItemDataRequest");
    }

    virtual void requestProductsData() override
    {
    	for (const auto& row : getProducts()) {
            backend::helper::callStaticVoidMethodWithStringAndBool(
                "addItemDataRequest",
                row.productIdentifier.c_str(),
                row.consumable
            );
        }
        backend::helper::callStaticVoidMethod("startItemDataRequest");
    }

    virtual void setDelegate(ManagerDelegate *delegate) override
    {
        _delegate = delegate;
    }
    virtual ManagerDelegate *getDelegate() const override
    {
        return _delegate;
    }

    virtual const std::vector<Product>& getProducts() const override
    {
        return _products;
    }

    virtual const Product* getProduct(const std::string &id) const override
    {
        for(const auto &product:_products)
        {
            if(product.id == id)
                return &product;
        }
        return nullptr;
    }

    Product* getProductByProductIdentifier(const std::string &productIdentifier)
    {
        for(auto &product:_products)
        {
            if(product.productIdentifier == productIdentifier)
                return &product;
        }
        return nullptr;
    }

    const Product* getProductByProductIdentifier(const std::string &productIdentifier) const
    {
        for(const auto &product:_products)
        {
            if(product.productIdentifier == productIdentifier)
                return &product;
        }
        return nullptr;
    }

    virtual void purchase(const std::string &id) override
    {
    	const avalon::payment::Product* productToPurchase = getProduct(id);
    	if(!productToPurchase)
    	{
    		Transaction managerTransaction;
    		managerTransaction.transactionState = TransactionState::Failed;
    		managerTransaction.productId = id;
    		_delegate->onPurchaseFail(managerTransaction, ManagerDelegateErrors::PRODUCT_UNKNOWN);
    	}
    	backend::helper::callStaticVoidMethodWithString("purchase", productToPurchase->productIdentifier.c_str());
    }

    virtual bool isPurchaseReady() const override
    {
    	return backend::helper::callStaticBoolMethod("isPurchaseReady");
    }

    virtual void restorePurchases() const override
    {
    }

    virtual void startService(const std::string &data)
    {
    	backend::helper::callStaticVoidMethodWithString("initialize",data.c_str());
    }

    virtual void stopService() override
    {
    	backend::helper::callStaticVoidMethod("shutdown");
    }

    virtual bool isStarted() const override
    {
    	return backend::helper::callStaticBoolMethod("isInitialized");
    }

private:
    std::vector<Product> _products;
    ManagerDelegate *_delegate;
};

Manager *Manager::getInstance()
{
    static AndroidManager *manager = nullptr;
    if(!manager)
    {
        manager = new AndroidManager();
    }
    return manager;
}

/**
 * Java -->> C++
 */

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_payment_Backend_delegateOnServiceStarted(JNIEnv* env, jclass clazz)
{
	Manager *globalManager = Manager::getInstance();
    AVALON_ASSERT_MSG(globalManager, "globalManager should be already set");

    if (globalManager && globalManager->getDelegate()) {
    	globalManager->getDelegate()->onServiceStarted();
    }
}

JNIEXPORT void JNICALL Java_com_avalon_payment_Backend_delegateOnPurchaseSucceed(JNIEnv* env, jclass clazz, jstring jProductId, jstring jOrderId, jboolean jRestored)
{
	AndroidManager *globalManager = static_cast<AndroidManager*>(Manager::getInstance());
    AVALON_ASSERT_MSG(globalManager, "globalManager should be already set");

    if (!globalManager) {
        return;
    }

    std::string productId = cocos2d::JniHelper::jstring2string(jProductId);
    const Product* product = globalManager->getProductByProductIdentifier(productId.c_str());

    if (globalManager->getDelegate()) {
    	Transaction managerTransaction;
    	if(product)
    	{
    		managerTransaction.productId = product->productIdentifier;
    		managerTransaction.transactionState = jRestored?TransactionState::Purchased:TransactionState::Restored;
    		globalManager->getDelegate()->onPurchaseSucceed(managerTransaction);
    	}
    	else
    	{
    		managerTransaction.productId = productId;
    		managerTransaction.transactionState = TransactionState::Failed;
    		globalManager->getDelegate()->onPurchaseFail(managerTransaction, ManagerDelegateErrors::PRODUCT_UNKNOWN);
    	}
    }
}

JNIEXPORT void JNICALL Java_com_avalon_payment_Backend_delegateOnPurchaseFail(JNIEnv* env, jclass clazz, jstring jProductId, jint code)
{
	Manager *globalManager = Manager::getInstance();
    AVALON_ASSERT_MSG(globalManager, "globalManager should be already set");

    if (globalManager && globalManager->getDelegate()) {
    	Transaction managerTransaction;
    	managerTransaction.productId = cocos2d::JniHelper::jstring2string(jProductId);
    	managerTransaction.transactionState = TransactionState::Failed;
    	globalManager->getDelegate()->onPurchaseFail(managerTransaction, static_cast<avalon::payment::ManagerDelegateErrors>(code));
    }
}

JNIEXPORT void JNICALL Java_com_avalon_payment_Backend_onInitialized(JNIEnv* env, jclass clazz)
{
	Manager *globalManager = Manager::getInstance();
    AVALON_ASSERT_MSG(globalManager, "globalManager should be already set");

    if (globalManager && globalManager->getDelegate()) {
    	globalManager->getDelegate()->onServiceStarted();
    }
}

JNIEXPORT void JNICALL Java_com_avalon_payment_Backend_onItemData(JNIEnv* env, jclass clazz, jstring jProductId, jstring jName, jstring jDesc, jstring jPriceStr, jstring jCurrencyCode, jfloat jprice)
{
	AndroidManager *globalManager = static_cast<AndroidManager*>(Manager::getInstance());
    AVALON_ASSERT_MSG(globalManager, "globalManager should be already set");
    
    if (!globalManager) {
        return;
    }

    std::string productId = cocos2d::JniHelper::jstring2string(jProductId);
    Product* product = globalManager->getProductByProductIdentifier(productId.c_str());

    if (!product) {
        return;
    }

    std::string localizedName = cocos2d::JniHelper::jstring2string(jName);
    std::string localizedDescription = cocos2d::JniHelper::jstring2string(jDesc);
    std::string localizedPrice = cocos2d::JniHelper::jstring2string(jPriceStr);
    std::string currencyCode = cocos2d::JniHelper::jstring2string(jCurrencyCode);

    product->localizedName = localizedName;
    product->localizedDescription = localizedDescription;
    product->localizedPrice = localizedPrice;
    product->currencyCode = currencyCode;
    product->price = (float)jprice;
}


} // extern "C"

} // namespace payment
} // namespace avalon
