#import <Foundation/Foundation.h>

extern NSString *const kGAIHitType;
extern NSString *const kGAITrackingId;
extern NSString *const kGAIClientId;
extern NSString *const kGAIAnonymizeIp;
extern NSString *const kGAISessionControl;
extern NSString *const kGAIScreenResolution;
extern NSString *const kGAIViewportSize;
extern NSString *const kGAIEncoding;
extern NSString *const kGAIScreenColors;
extern NSString *const kGAILanguage;
extern NSString *const kGAIJavaEnabled;
extern NSString *const kGAIFlashVersion;
extern NSString *const kGAINonInteraction;
extern NSString *const kGAIReferrer;
extern NSString *const kGAILocation;
extern NSString *const kGAIHostname;
extern NSString *const kGAIPage;
extern NSString *const kGAIDescription;  // synonym for kKKGAIScreenName
extern NSString *const kGAIScreenName;   // synonym for kKKGAIDescription
extern NSString *const kGAITitle;
extern NSString *const kGAIAppName;
extern NSString *const kGAIAppVersion;
extern NSString *const kGAIAppId;
extern NSString *const kGAIAppInstallerId;
extern NSString *const kGAIUserId;

extern NSString *const kGAIEventCategory;
extern NSString *const kGAIEventAction;
extern NSString *const kGAIEventLabel;
extern NSString *const kGAIEventValue;

extern NSString *const kGAISocialNetwork;
extern NSString *const kGAISocialAction;
extern NSString *const kGAISocialTarget;

extern NSString *const kGAITransactionId;
extern NSString *const kGAITransactionAffiliation;
extern NSString *const kGAITransactionRevenue;
extern NSString *const kGAITransactionShipping;
extern NSString *const kGAITransactionTax;
extern NSString *const kGAICurrencyCode;

extern NSString *const kGAIItemPrice;
extern NSString *const kGAIItemQuantity;
extern NSString *const kGAIItemSku;
extern NSString *const kGAIItemName;
extern NSString *const kGAIItemCategory;

extern NSString *const kGAICampaignSource;
extern NSString *const kGAICampaignMedium;
extern NSString *const kGAICampaignName;
extern NSString *const kGAICampaignKeyword;
extern NSString *const kGAICampaignContent;
extern NSString *const kGAICampaignId;

extern NSString *const kGAITimingCategory;
extern NSString *const kGAITimingVar;
extern NSString *const kGAITimingValue;
extern NSString *const kGAITimingLabel;

extern NSString *const kGAIExDescription;
extern NSString *const kGAIExFatal;

// hit types
extern NSString *const kGAIAppView;
extern NSString *const kGAIPageView;
extern NSString *const kGAIEvent;
extern NSString *const kGAISocial;
extern NSString *const kGAITransaction;
extern NSString *const kGAIItem;
extern NSString *const kGAIException;
extern NSString *const kGAITiming;

extern NSString *const kGAIExperimentID;
extern NSString *const kGAIExperimentVariant;

@interface GAIFields : NSObject
/*!
 Generates the correct parameter name for a custon dimension with an index.
 @param index the index of the custom dimension.
 @return an NSString representing the custom dimension parameter for the index.
 */
+ (NSString *)customDimensionForIndex:(NSUInteger)index;
/*!
 Generates the correct parameter name for a custom metric with an index.
 @param index the index of the custom metric.
 @return an NSString representing the custom metric parameter for the index.
 */
+ (NSString *)customMetricForIndex:(NSUInteger)index;
@end
