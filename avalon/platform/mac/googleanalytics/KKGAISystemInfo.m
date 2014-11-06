#import "KKGAISystemInfo.h"
#import <CoreServices/CoreServices.h>
#import <AppKit/NSScreen.h>
#include <sys/types.h>
#include <sys/sysctl.h>

NSString *UserAgentString() {
	static NSString *userAgent;
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
#ifdef MAC_OS_X_VERSION_10_10
        NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
        NSString *OSVersion = [NSString stringWithFormat:@"%d_%d_%d", (int)version.majorVersion, (int)version.minorVersion, (int)version.patchVersion];
#else
		SInt32 major, minor, bugfix;
		Gestalt(gestaltSystemVersionMajor, &major);
		Gestalt(gestaltSystemVersionMinor, &minor);
		Gestalt(gestaltSystemVersionBugFix, &bugfix);
		NSString *OSVersion = [NSString stringWithFormat:@"%d_%d_%d", major, minor, bugfix];
#endif

		userAgent = [NSString stringWithFormat:@"%@/%@ (Macintosh; %@ Mac OS X %@; %@)",
					 [GAISystemInfo appName],
					 [GAISystemInfo appVersion],
					 [GAISystemInfo CPUType],
					 OSVersion,
					 [GAISystemInfo machineModel]] ;
	});
	return userAgent;
}

@implementation GAISystemInfo

+ (NSString *)appName
{
	NSString *name = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleDisplayName"];
	if (!name) {
		name = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
	}
	return name;
}

+ (NSString *)appVersion
{
	return [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
}

+ (NSString *)CPUType
{
	int error = 0;
	int value = 0;
	unsigned long length = sizeof(value) ;

	NSString *visibleCPUType = @"an Unknown Processor";

	error = sysctlbyname("hw.cputype", &value, &length, NULL, 0);
	int cpuType = -1;
	if (error == 0) {
		cpuType = value;
		switch(value) {
			case 7:
				visibleCPUType = @"Intel";
				break;
			case 18:
				visibleCPUType = @"PowerPC";
				break;
			default:
				visibleCPUType = @"Unknown";
				break;
		}
	}

	return visibleCPUType;
}

+ (NSString *)machineModel
{
	size_t len = 0;
	sysctlbyname("hw.model", NULL, &len, NULL, 0);

	if (len) {
		char *model = malloc(len*sizeof(char));
		sysctlbyname("hw.model", model, &len, NULL, 0);
		NSString *model_ns = [NSString stringWithUTF8String:model];
		free(model);
		return model_ns;
	}

	return @"Just an Apple Computer"; //incase model name can't be read
}

+ (NSString *)primaryLanguage
{
	NSArray *languages = [[NSUserDefaults standardUserDefaults] objectForKey:@"AppleLanguages"];
	return languages[0];
}

+ (NSString *)screenResolutionString
{
	NSScreen *screen = [NSScreen mainScreen];
	return [NSString stringWithFormat:@"%ldx%ld", (long)screen.frame.size.width, (long)screen.frame.size.height];
}

+ (NSString *)screenDepthString
{
	NSScreen *screen = [NSScreen mainScreen];
	switch (screen.depth) {
		case NSWindowDepthTwentyfourBitRGB:
			return @"24-bits";
		case NSWindowDepthSixtyfourBitRGB:
			return @"64-bits";
		case NSWindowDepthOnehundredtwentyeightBitRGB:
			return @"128-bits";
		default:
			break;
	}
	return nil;
}

@end
