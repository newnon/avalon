#import <Foundation/Foundation.h>

@interface GAIDictionaryBuilder : NSObject
- (void)set:(NSString *)value forKey:(NSString *)key;
- (void)setAll:(NSDictionary *)params;
- (NSString *)get:(NSString *)paramName;
- (NSMutableDictionary *)build;
@end

@interface GAIDictionaryBuilder (ClassMethods)
+ (GAIDictionaryBuilder *)createScreenView;
+ (GAIDictionaryBuilder *)createPageViewWithName:(NSString *)name hostname:(NSString *)hostname page:(NSString *)page;
+ (GAIDictionaryBuilder *)createEventWithCategory:(NSString *)category action:(NSString *)action label:(NSString *)label value:(NSNumber *)value;
+ (GAIDictionaryBuilder *)createTransaction:(NSString *)transactionID affliation:(NSString *)affliation revenue:(NSNumber *)revenue shipping:(NSNumber *)shipping tax:(NSNumber *)tax currencyCode:(NSString *)currencyCode;
+ (GAIDictionaryBuilder *)createItem:(NSString *)transactionID name:(NSString *)name SKU:(NSString *)SKU cateogory:(NSString *)category price:(NSNumber *)price quantity:(NSNumber *)quantity currencyCode:(NSString *)currencyCode;

+ (GAIDictionaryBuilder *)createSocialWithNetwork:(NSString *)network action:(NSString *)action target:(NSString *)target;
+ (GAIDictionaryBuilder *)createTimingWithCategory:(NSString *)category interval:(NSNumber *)intervalMillis name:(NSString *)name label:(NSString *)label;
+ (GAIDictionaryBuilder *)createExceptionWithDescription:(NSString *)description withFatal:(NSNumber *)fatal;
@end
