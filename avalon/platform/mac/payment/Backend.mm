#include "avalon/payment/Backend.h"

#include <StoreKit/StoreKit.h>

#include "avalon/payment/Manager.h"
#include "avalon/payment/ManagerDelegate.h"
#include "avalon/payment/Product.h"

#import "RMStore.h"
#import "RMStoreKeychainPersistence.h"

namespace avalon {
namespace payment {
    
static bool _initialized = false;
    
avalon::payment::ManagerDelegateErrors convertErrorToCode(NSError * error)
{
    avalon::payment::ManagerDelegateErrors ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
    switch (error.code) {
        case SKErrorPaymentCancelled:
            NSLog(@"[Payment] failedTransaction: SKErrorPaymentCancelled");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENTCANCELLED;
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
            ret = avalon::payment::ManagerDelegateErrors::PAYMENTINVALID;
            break;
            
        case SKErrorPaymentNotAllowed:
            NSLog(@"[Payment] failedTransaction: SKErrorPaymentNotAllowed");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENTNOTALLOWED;
            break;
            
        case 100:
            NSLog(@"[Payment] failedTransaction: RMStoreErrorCodeUnknownProductIdentifier");
            ret = avalon::payment::ManagerDelegateErrors::STOREPRODUCTNOTAVAILABLE;
            break;
        case 200:
            NSLog(@"[Payment] failedTransaction: RMStoreErrorCodeUnableToCompleteVerification");
            ret = avalon::payment::ManagerDelegateErrors::PAYMENTINVALID;
            break;
            
        default:
            NSLog(@"[Payment] failedTransaction: UNHANDELED: %ld", (long)error.code);
            ret = avalon::payment::ManagerDelegateErrors::UNKNOWN;
            break;
    }
    return ret;
}

RMStore* const __getMacBackend()
{
    static RMStoreKeychainPersistence* persistence = nullptr;
    if (!persistence) {
        persistence = [[RMStoreKeychainPersistence alloc] init];
        [RMStore defaultStore].transactionPersistor = persistence;
    }
    return [RMStore defaultStore];
}

Backend::Backend(Manager& manager)
: manager(manager)
{
    RMStoreKeychainPersistence *_persistence = [[RMStoreKeychainPersistence alloc] init];
    [RMStore defaultStore].transactionPersistor = _persistence;
}

Backend::~Backend()
{
    shutdown();
}

bool Backend::isInitialized() const
{
    return _initialized;
}

void Backend::initialize(const std::string &data)
{
    _initialized = true;
    // convert Avalon::Payment::ProductList into NSMutableSet
    NSMutableSet* productsIds = [[[NSMutableSet alloc] init] autorelease];
    for (const auto& pair : manager.getProducts()) {
        [productsIds addObject:[NSString stringWithUTF8String:pair.second->getProductId().c_str()]];
    }
    
    [__getMacBackend() requestProducts:productsIds success:^(NSArray *products, NSArray *invalidProductIdentifiers) {
        for (SKProduct* skProduct in products) {
            const char* productId = [[skProduct productIdentifier] cStringUsingEncoding:NSASCIIStringEncoding];
            avalon::payment::Product* avProduct = manager.getProduct(productId);
            if (avProduct == NULL) {
                NSLog(@"[Payment] productsRequest: Product not found on our side - productId: %s", productId);
                continue;
            }
            
            NSNumberFormatter* numberFormatter = [[[NSNumberFormatter alloc] init] autorelease];
            [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
            [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
            
            avProduct->price = [[skProduct price] floatValue];
            [numberFormatter setLocale:skProduct.priceLocale];
            avProduct->localizedPrice = [[numberFormatter stringFromNumber:skProduct.price] UTF8String];
            
            NSString* localizedName = [skProduct localizedTitle];
            if (localizedName != NULL) {
                avProduct->localizedName = [localizedName UTF8String];
            }
            NSString* localizedDescription = [skProduct localizedDescription];
            if (localizedDescription != NULL) {
                avProduct->localizedDescription = [localizedDescription UTF8String];
            }
        }
        
        if (manager.delegate) {
            manager.delegate->onServiceStarted(&manager);
        }

    } failure:^(NSError *error) {
    }];
}

void Backend::shutdown()
{
    if (!isInitialized()) {
        return;
    }
}

void Backend::purchase(Product* const product)
{
    [__getMacBackend() addPayment:[NSString stringWithCString:product->getProductId().c_str() encoding:NSUTF8StringEncoding] success:^(SKPaymentTransaction *transaction) {
        const char* productId = [transaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
        avalon::payment::Product* product = manager.getProduct(productId);
        if (product) {
            product->onHasBeenPurchased();
        } else {
            NSLog(@"[Payment] completeTransaction failed: invalid productId: %s", productId);
        }
        if (product) {
            if (manager.delegate) {
                manager.delegate->onPurchaseSucceed(&manager, product);
            }
        }
    } failure:^(SKPaymentTransaction *transaction, NSError *error) {
        if (manager.delegate) {
            manager.delegate->onPurchaseFail(&manager, convertErrorToCode(error));
        }
    }];
}

bool Backend::isPurchaseReady() const
{
    return [RMStore canMakePayments];
}

void Backend::restorePurchases() const
{
    [__getMacBackend() restoreTransactionsOnSuccess:^(NSArray *transactions) {
        for (SKPaymentTransaction* transaction in transactions) {
            const char* productId = [transaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
            avalon::payment::Product* product = manager.getProduct(productId);
            if (product) {
                product->onHasBeenPurchased();
            } else {
                NSLog(@"[Payment] completeTransaction failed: invalid productId: %s", productId);
            }
            if (product) {
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                
                if (manager.delegate) {
                    manager.delegate->onPurchaseSucceed(&manager, product);
                }
            }
        }
        if (manager.delegate) {
            manager.delegate->onRestoreSucceed(&manager);
        }
    } failure:^(NSError *error) {
        if (manager.delegate) {
            manager.delegate->onRestoreFail(&manager, convertErrorToCode(error));
        }
    }];
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

} // namespace payment
} // namespace avalon
