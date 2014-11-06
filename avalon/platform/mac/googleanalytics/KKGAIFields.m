#import "KKGAIFields.h"

NSString *const kGAIHitType = @"t";
NSString *const kGAITrackingId = @"tid";
NSString *const kGAIClientId = @"cid";
NSString *const kGAIAnonymizeIp = @"aip";
NSString *const kGAISessionControl = @"sc";
NSString *const kGAIScreenResolution = @"sr";
NSString *const kGAIViewportSize = @"vp";
NSString *const kGAIEncoding = @"de";
NSString *const kGAIScreenColors = @"sd";
NSString *const kGAILanguage = @"ul";
NSString *const kGAIJavaEnabled = @"fe";
NSString *const kGAIFlashVersion = @"fl";
NSString *const kGAINonInteraction = @"ni";
NSString *const kGAIReferrer = @"dr";
NSString *const kGAILocation = @"dl";
NSString *const kGAIHostname = @"dh";
NSString *const kGAIPage = @"dp";
NSString *const kGAIDescription = @"cd";
NSString *const kGAIScreenName = @"cd";
NSString *const kGAITitle = @"dt";
NSString *const kGAIAppName = @"an";
NSString *const kGAIAppVersion = @"av";
NSString *const kGAIAppId = @"aid";
NSString *const kGAIAppInstallerId = @"aiid";
NSString *const kGAIUserId = @"uid";

NSString *const kGAIEventCategory = @"ec";
NSString *const kGAIEventAction = @"ea";
NSString *const kGAIEventLabel = @"el";
NSString *const kGAIEventValue = @"ev";

NSString *const kGAISocialNetwork = @"sn";
NSString *const kGAISocialAction = @"sa";
NSString *const kGAISocialTarget = @"st";

NSString *const kGAITransactionId = @"ti";
NSString *const kGAITransactionAffiliation = @"ta";
NSString *const kGAITransactionRevenue = @"tr";
NSString *const kGAITransactionShipping = @"ts";
NSString *const kGAITransactionTax = @"tt";
NSString *const kGAICurrencyCode = @"cu";

NSString *const kGAIItemPrice = @"ip";
NSString *const kGAIItemQuantity = @"iq";
NSString *const kGAIItemSku = @"ic";
NSString *const kGAIItemName = @"in";
NSString *const kGAIItemCategory = @"iv";

NSString *const kGAICampaignSource = @"cs";
NSString *const kGAICampaignMedium = @"cm";
NSString *const kGAICampaignName = @"cn";
NSString *const kGAICampaignKeyword = @"ck";
NSString *const kGAICampaignContent = @"cc";
NSString *const kGAICampaignId = @"ci";

NSString *const kGAITimingCategory = @"utc";
NSString *const kGAITimingVar = @"utv";
NSString *const kGAITimingValue = @"utt";
NSString *const kGAITimingLabel = @"utl";

NSString *const kGAIExDescription = @"exd";
NSString *const kGAIExFatal = @"exf";

// hit types
NSString *const kGAIAppView = @"appview";
NSString *const kGAIPageView = @"pageview";
NSString *const kGAIEvent = @"event";
NSString *const kGAISocial = @"social";
NSString *const kGAITransaction = @"transaction";
NSString *const kGAIItem = @"item";
NSString *const kGAIException = @"exception";
NSString *const kGAITiming = @"timing";

NSString *const kGAIExperimentID = @"xid";
NSString *const kGAIExperimentVariant = @"xvar";

@implementation GAIFields

+ (NSString *)customDimensionForIndex:(NSUInteger)index
{
	return [NSString stringWithFormat:@"cd%lu", (unsigned long)index];
}
+ (NSString *)customMetricForIndex:(NSUInteger)index
{
	return [NSString stringWithFormat:@"cm%lu", (unsigned long)index];
}

@end
