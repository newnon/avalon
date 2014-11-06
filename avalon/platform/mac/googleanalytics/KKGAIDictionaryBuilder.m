#import "KKGAIDictionaryBuilder.h"
#import "KKGAIFields.h"
#import "KKGAISystemInfo.h"

@interface GAIDictionaryBuilder()
@property (strong, nonatomic) NSMutableDictionary *dictionary;
@end

@implementation GAIDictionaryBuilder

- (instancetype)init
{
	self = [super init];
	if (self) {
		self.dictionary = [NSMutableDictionary dictionary];
	}
	return self;
}

- (void)set:(NSString *)value forKey:(NSString *)key
{
	self.dictionary[key] = value;
}

- (void)setAll:(NSDictionary *)params
{
	[self.dictionary setDictionary:params];
}

- (NSString *)get:(NSString *)paramName
{
	return self.dictionary[paramName];
}

- (id)objectForKeyedSubscript:(id <NSCopying>)key
{
	return self.dictionary[key];
}
- (void)setObject:(id)obj forKeyedSubscript:(id <NSCopying>)key
{
	self.dictionary[key] = obj;
}

- (NSMutableDictionary *)build
{
	return [NSMutableDictionary dictionaryWithDictionary:self.dictionary];
}

@end


@implementation GAIDictionaryBuilder (ClassMethods)

+ (GAIDictionaryBuilder *)createScreenView
{
	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAIAppView;
	payload[kGAIAppName] = [GAISystemInfo appName];
	payload[kGAIAppVersion] = [GAISystemInfo appVersion];
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createPageViewWithName:(NSString *)name hostname:(NSString *)hostname page:(NSString *)page
{
	NSParameterAssert(name);
	NSParameterAssert(hostname);
	NSParameterAssert(page);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAIPageView;
	payload[kGAIHostname] = name;
	payload[kGAIPage] = page;
	payload[kGAITitle] = name;
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createEventWithCategory:(NSString *)category action:(NSString *)action label:(NSString *)label value:(NSNumber *)value
{
	NSParameterAssert(category);
	NSParameterAssert(action);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAIEvent;
	payload[kGAIEventCategory] = category;
	payload[kGAIEventAction] = action;
	if (label) {
		payload[kGAIEventLabel] = label;
	}
	if (value) {
		payload[kGAIEventValue] = value;
	}
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createTransaction:(NSString *)transactionID affliation:(NSString *)affliation revenue:(NSNumber *)revenue shipping:(NSNumber *)shipping tax:(NSNumber *)tax currencyCode:(NSString *)currencyCode
{
	NSParameterAssert(transactionID);
	NSParameterAssert(affliation);
	NSParameterAssert(revenue);
	NSParameterAssert(shipping);
	NSParameterAssert(tax);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAITransaction;
	payload[kGAITransactionId] = transactionID;
	payload[kGAITransactionAffiliation] = affliation;
	payload[kGAITransactionRevenue] = revenue;
	payload[kGAITransactionShipping] = shipping;
	payload[kGAITransactionTax] = tax;
	if (currencyCode) {
		payload[kGAICurrencyCode] = currencyCode;
	}
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createItem:(NSString *)transactionID name:(NSString *)name SKU:(NSString *)SKU cateogory:(NSString *)category price:(NSNumber *)price quantity:(NSNumber *)quantity currencyCode:(NSString *)currencyCode
{
	NSParameterAssert(transactionID);
	NSParameterAssert(name);
	NSParameterAssert(SKU);
	NSParameterAssert(category);
	NSParameterAssert(price);
	NSParameterAssert(quantity);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAIItem;
	payload[kGAITransactionId] = transactionID;
	payload[kGAIItemName] = name;
	payload[kGAIItemPrice] = price;
	payload[kGAIItemQuantity] = quantity;
	payload[kGAIItemSku] = SKU;
	payload[kGAIItemCategory] = category;
	if (currencyCode) {
		payload[kGAICurrencyCode] = currencyCode;
	}
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createSocialWithNetwork:(NSString *)network action:(NSString *)action target:(NSString *)target
{
	NSParameterAssert(network);
	NSParameterAssert(action);
	NSParameterAssert(target);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAISocial;
	payload[kGAISocialNetwork] = network;
	payload[kGAISocialAction] = action;
	payload[kGAISocialTarget] = target;
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createTimingWithCategory:(NSString *)category interval:(NSNumber *)intervalMillis name:(NSString *)name label:(NSString *)label
{
	NSParameterAssert(category);
	NSParameterAssert(intervalMillis);
	NSParameterAssert(name);
	NSParameterAssert(label);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAITiming;
	payload[kGAITimingCategory] = category;
	payload[kGAITimingVar] = name;
	payload[kGAITimingValue] = intervalMillis;
	payload[kGAITimingLabel] = label;
	[builder.dictionary setDictionary:payload];
	return builder;
}
+ (GAIDictionaryBuilder *)createExceptionWithDescription:(NSString *)description withFatal:(NSNumber *)fatal
{
	NSParameterAssert(description);
	NSParameterAssert(fatal);

	GAIDictionaryBuilder *builder = [[GAIDictionaryBuilder alloc] init];
	NSMutableDictionary *payload = [NSMutableDictionary dictionary];
	payload[kGAIHitType] = kGAIException;
	payload[kGAIExDescription] = description;
	payload[kGAIExFatal] = fatal;
	[builder.dictionary setDictionary:payload];
	return builder;
}


@end
