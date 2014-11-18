#include "avalon/payment.h"

namespace avalon {
namespace payment {

class WindowsManager:public Manager
{
public:
    WindowsManager():_delegate(nullptr)
    {
    }
    ~WindowsManager()
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
        if (_delegate)
        {
            Transaction managerTransaction;
            managerTransaction.transactionState = TransactionState::Purchased;
            managerTransaction.productId = id;
            _delegate->onPurchaseSucceed(managerTransaction);
        }
    }
    
    virtual bool isPurchaseReady() const override
    {
        return true;
    }
    
    virtual void restorePurchases() const override
    {
        std::vector<Transaction> managerTransactions;
        if (_delegate) {
            _delegate->onRestoreSucceed(managerTransactions);
        }
    }
    
    virtual void startService(const std::string &data)
    {
        if(!_started)
        {
            _started = true;
            if (_delegate) {
                _delegate->onServiceStarted();
        }
    }
    
    virtual void stopService() override
    {
        if(_started)
            _started = false;
    }
    
    virtual bool isStarted() const override
    {
        return _started;
    }
    
private:
    std::vector<Product> _products;
    ManagerDelegate *_delegate;
    bool _started;
};

Manager *Manager::getInstance()
{
    static WindowsManager *manager;
    if(!manager)
    {
        manager = new WindowsManager();
    }
    return manager;
}

} // namespace payment
} // namespace avalon
