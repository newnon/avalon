#ifndef AVALON_PAYMENT_H
#define AVALON_PAYMENT_H

#include <string>
#include <vector>
#include <cstdint>

namespace avalon {
namespace payment {

struct Product
{
    Product():data(0), consumable(false), price(0.0f) {}
    std::string id;
    std::string productIdentifier;
    std::string localizedName;
    std::string localizedDescription;
    std::string localizedPrice;
    std::string currencyCode;
    std::intptr_t data;
    bool consumable;
    float price;
};

enum class TransactionState
{
    Purchased,
    Failed,
    Restored
};

struct Transaction
{
    std::string transactionIdentifier;
    TransactionState transactionState;
    std::string productId;
    std::vector<unsigned char> receipt;
};

enum class ManagerDelegateErrors
{
    UNKNOWN,
    PRODUCT_UNKNOWN,              // product not found in list
    CLIENT_INVALID,               // client is not allowed to issue the request, etc.
    PAYMENT_CANCELLED,            // user cancelled the request, etc.
    PAYMENT_INVALID,              // purchase identifier was invalid, etc.
    PAYMENT_NOTALLOWED,           // this device is not allowed to make the payment
    STORE_PRODUCT_NOT_AVAILABLE,  // Product is not available in the current storefront
};

class ManagerDelegate
{
public:
    virtual void onServiceStarted() {};
    
    virtual void onRequestProductsSucceed() = 0;
    virtual void onRequestProductsFail() = 0;
    
    virtual void onPurchaseSucceed(const Transaction &transaction) = 0;
    virtual void onPurchaseFail(const Transaction &transaction, ManagerDelegateErrors error) = 0;
    
    virtual void onRestoreSucceed(const std::vector<Transaction> &transactions) {};
    virtual void onRestoreFail(ManagerDelegateErrors error) {};
};

class Manager
{
public:
    static Manager *getInstance();
    
    virtual void addProduct(const std::string &id, const std::string &productIdentifier, bool consumable) = 0;
    virtual void addProduct(const Product &product) = 0;
    virtual void clearProducts() = 0;
    virtual void requestProductsData() = 0;
    
    virtual void setDelegate(ManagerDelegate *delegate) = 0;
    virtual ManagerDelegate *getDelegate() const = 0;
    
    virtual const std::vector<Product>& getProducts() const = 0;
    virtual const Product* getProduct(const std::string &id) const = 0;
    
    virtual void purchase(const std::string &id) = 0;
    virtual bool isPurchaseReady() const = 0;
    virtual void restorePurchases() const = 0;
    
    virtual void startService(const std::string &data) = 0;
    virtual void stopService() = 0;
    virtual bool isStarted() const = 0;
    
    virtual void setCurrentLocale(const std::string& locale) { };
};

} //payment
} //avalon

#endif /* AVALON_PAYMENT_H */
