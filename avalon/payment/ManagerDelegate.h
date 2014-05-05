#ifndef AVALON_PAYMENT_MANAGERDELEGATE_H
#define AVALON_PAYMENT_MANAGERDELEGATE_H

namespace avalon {
namespace payment {

class Manager;
class Product;
    
enum class ManagerDelegateErrors
{
    UNKNOWN,
    PAYMENTCANCELLED,            // user cancelled the request, etc.
    PAYMENTINVALID,              // purchase identifier was invalid, etc.
    PAYMENTNOTALLOWED,           // this device is not allowed to make the payment
    STOREPRODUCTNOTAVAILABLE,    // Product is not available in the current storefront
};

class ManagerDelegate
{
public:
    virtual void onServiceStarted(Manager* const manager) {};

    virtual void onPurchaseSucceed(Manager* const manager, Product* const product) = 0;
    virtual void onPurchaseFail(Manager* const manager, ManagerDelegateErrors error) = 0;

    virtual void onTransactionStart(Manager* const manager) {};
    virtual void onTransactionEnd(Manager* const manager) {};

    virtual void onRestoreSucceed(Manager* const manager) {};
    virtual void onRestoreFail(Manager* const manager, ManagerDelegateErrors error) {};
};

} // namespace payment
} // namespace avalon

#endif /* AVALON_PAYMENT_MANAGERDELEGATE_H */
