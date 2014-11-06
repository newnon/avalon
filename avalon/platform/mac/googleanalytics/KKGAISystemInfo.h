#import <Foundation/Foundation.h>

NSString *UserAgentString();

/*! KKGASystemInfo helps to obtain required system information. */
@interface GAISystemInfo : NSObject
/*! Get the name of current application. */
+ (NSString *)appName;
/*! Get the version of current application. */
+ (NSString *)appVersion;
/*! Get CPU type. It could be "Intel", "PowerPC" or "Unknown". */
+ (NSString *)CPUType;
/*! Get the model of current device. */
+ (NSString *)machineModel;
/*! Get the language code of current user's primary language. */
+ (NSString *)primaryLanguage;
/*! Get screen resolution, such as "1024x768". */
+ (NSString *)screenResolutionString;
/*! Get screen depth, such as "24-bits", "64-bits" or "128-bits". */
+ (NSString *)screenDepthString;
@end
