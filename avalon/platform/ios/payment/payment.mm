#include "avalon/payment.h"

#import "RMStore.h"
#import "RMStoreAppReceiptVerificator.h"
#import "RMStoreKeychainPersistence.h"

namespace avalon {
namespace payment {
    
    
avalon::payment::ManagerDelegateErrors convertErrorToCode(NSError * error)
{
    avalon::payment::ManagerDelegateErrors ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
    switch (error.code) {
        case SKErrorPaymentCancelled:
            NSLog(@"[Payment] failedTransaction: SKErrorPaymentCancelled");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENT_CANCELLED;
            break;
            
        case SKErrorUnknown:
            NSLog(@"[Payment] failedTransaction: SKErrorUnknown: %@ | %@", error.localizedDescription, error.localizedFailureReason );
            ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
            break;
            
        case SKErrorClientInvalid:
            NSLog(@"[Payment] failedTransaction: SKErrorClientInvalid");
            ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
            break;
            
        case SKErrorPaymentInvalid:
            NSLog(@"[Payment] failedTransaction: SKErrorPaymentInvalid");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENT_INVALID;
            break;
            
        case SKErrorPaymentNotAllowed:
            NSLog(@"[Payment] failedTransaction: SKErrorPaymentNotAllowed");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENT_NOTALLOWED;
            break;
            
        case 100:
            NSLog(@"[Payment] failedTransaction: RMStoreErrorCodeUnknownProductIdentifier");
            ret = avalon::payment::ManagerDelegateErrors::STORE_PRODUCT_NOT_AVAILABLE;
            break;
        case 200:
            NSLog(@"[Payment] failedTransaction: RMStoreErrorCodeUnableToCompleteVerification");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENT_INVALID;
            break;
            
        default:
            NSLog(@"[Payment] failedTransaction: UNHANDELED: %ld", (long)error.code);
            ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
            break;
    }
    return ret;
}
    
class IOSManager:public Manager
{
public:
    IOSManager():_started(false),_delegate(nullptr)
    {
        _transactionPersistor = [[RMStoreKeychainPersistence alloc] init];
        _receiptVerificator = [[RMStoreAppReceiptVerificator alloc] init];
        [RMStore defaultStore].transactionPersistor = _transactionPersistor;
        [RMStore defaultStore].receiptVerificator = _receiptVerificator;
    }
    ~IOSManager()
    {
        [RMStore defaultStore].transactionPersistor = nil;
        _transactionPersistor = nil;
        [RMStore defaultStore].receiptVerificator = nil;
        _receiptVerificator = nil;
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
    }
    
    virtual void requestProductsData() override
    {
        NSMutableSet* productsIds = [[NSMutableSet alloc] init];
        for (const auto& product : _products)
        {
            [productsIds addObject:[NSString stringWithUTF8String:product.productIdentifier.c_str()]];
        }
        
        [[RMStore defaultStore] requestProducts:productsIds success:^(NSArray *products, NSArray *invalidProductIdentifiers) {
            for (SKProduct* skProduct in products) {
                const char* productId = [[skProduct productIdentifier] cStringUsingEncoding:NSASCIIStringEncoding];
                Product* avProduct = getProductByProductIdentifier(productId);
                if (avProduct == NULL) {
                    NSLog(@"[Payment] productsRequest: Product not found on our side - productId: %s", productId);
                    continue;
                }
                
                NSNumberFormatter* numberFormatter = [[NSNumberFormatter alloc] init];
                [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
                [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                
                avProduct->price = [[skProduct price] floatValue];
                [numberFormatter setLocale:skProduct.priceLocale];
                avProduct->localizedPrice = [[numberFormatter stringFromNumber:skProduct.price] UTF8String];
                
                NSString *currencyCode = [skProduct.priceLocale objectForKey:NSLocaleCurrencyCode];
                if(currencyCode != NULL) {
                    avProduct->currencyCode = [currencyCode UTF8String];
                }
                NSString* localizedName = [skProduct localizedTitle];
                if (localizedName != NULL) {
                    avProduct->localizedName = [localizedName UTF8String];
                }
                NSString* localizedDescription = [skProduct localizedDescription];
                if (localizedDescription != NULL) {
                    avProduct->localizedDescription = [localizedDescription UTF8String];
                }
            }
            
            if (_delegate) {
                _delegate->onRequestProductsSucceed();
            }
        } failure:^(NSError *error) {
            if (_delegate) {
                _delegate->onRequestProductsFail();
            }
        }];

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
        
        NSString *externalId = [NSString stringWithUTF8String:id.c_str()];
            
        [[RMStore defaultStore] addPayment:[NSString stringWithCString:productToPurchase->productIdentifier.c_str() encoding:NSUTF8StringEncoding] success:^(SKPaymentTransaction *transaction) {
            const char* productId = [transaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
            if (_delegate)
            {
                Transaction managerTransaction;
                managerTransaction.transactionIdentifier = [transaction.transactionIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
                managerTransaction.transactionState = (transaction.transactionState == SKPaymentTransactionStateRestored)? TransactionState::Restored : TransactionState::Purchased;
                managerTransaction.productId = [externalId cStringUsingEncoding:NSASCIIStringEncoding];
                managerTransaction.receipt.resize(transaction.transactionReceipt.length);
                memcpy(&managerTransaction.receipt.front(), transaction.transactionReceipt.bytes, transaction.transactionReceipt.length);

                const avalon::payment::Product* product = getProductByProductIdentifier(productId);
                if(product)
                {
                    _delegate->onPurchaseSucceed(managerTransaction);
                }
                else
                {
                    _delegate->onPurchaseFail(managerTransaction, ManagerDelegateErrors::PRODUCT_UNKNOWN);
                }
            }
        } failure:^(SKPaymentTransaction *transaction, NSError *error) {
            if (_delegate) {
                Transaction managerTransaction;
                managerTransaction.transactionState = TransactionState::Failed;
                if(transaction)
                    managerTransaction.transactionIdentifier = [transaction.transactionIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
                managerTransaction.productId = [externalId cStringUsingEncoding:NSASCIIStringEncoding];
                _delegate->onPurchaseFail(managerTransaction, convertErrorToCode(error));
            }
        }];

    }
    
    virtual bool isPurchaseReady() const override
    {
        return [RMStore canMakePayments];
    }
    
    virtual void restorePurchases() const override
    {
        [[RMStore defaultStore] restoreTransactionsOnSuccess:^(NSArray *transactions) {
            std::vector<Transaction> managerTransactions;
            for (SKPaymentTransaction* transaction in transactions) {
                const char* productId = [transaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
                if (_delegate)
                {
                    Transaction managerTransaction;
                    managerTransaction.transactionIdentifier = [transaction.transactionIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
                    const Product* product = getProductByProductIdentifier(productId);
                    if(product)
                    {
                        managerTransaction.productId = product->id;
                        managerTransaction.transactionState = (transaction.transactionState == SKPaymentTransactionStateRestored)? TransactionState::Restored : TransactionState::Purchased;
                        managerTransaction.receipt.resize(transaction.transactionReceipt.length);
                        memcpy(&managerTransaction.receipt.front(), transaction.transactionReceipt.bytes, transaction.transactionReceipt.length);
                        managerTransactions.push_back(managerTransaction);
                        _delegate->onPurchaseSucceed(managerTransaction);
                    }
                }
            }
            if (_delegate) {
                _delegate->onRestoreSucceed(managerTransactions);
            }
        } failure:^(NSError *error) {
            if (_delegate) {
                _delegate->onRestoreFail(convertErrorToCode(error));
            }
        }];
    }
    
    virtual void startService() override
    {
        if(_started)
        {
            NSLog(@"[Payment] service already started");
            return;
        }
        
        if (_delegate) {
            _delegate->onServiceStarted();
        }
        
        _started = true;
    }
    
    virtual void stopService() override
    {
        _started = false;
    }
    
    virtual bool isStarted() const override
    {
        return _started;
    }
    
private:
    bool _started;
    ManagerDelegate *_delegate;
    RMStoreKeychainPersistence *_transactionPersistor;
    RMStoreAppReceiptVerificator *_receiptVerificator;
    std::vector<Product> _products;
};
    
Manager *Manager::getInstance()
{
    static IOSManager *manager = nullptr;
    if(!manager)
    {
        manager = new IOSManager();
    }
    return manager;
}
    
} // namespace payment
} // namespace avalon
