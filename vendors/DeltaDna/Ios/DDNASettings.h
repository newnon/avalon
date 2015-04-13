//
// DDNASettings.h
//
// Defines constants for the DeltaDNA SDK.
//

#import <Foundation/Foundation.h>

extern NSString *const DDNA_SDK_VERSION;
extern NSString *const DDNA_ENGAGE_API_VERSION;

extern NSString *const DDNA_EVENT_STORAGE_PATH;
extern NSString *const DDNA_ENGAGE_STORAGE_PATH;
extern NSString *const DDNA_COLLECT_URL_PATTERN;
extern NSString *const DDNA_COLLECT_HASH_URL_PATTERN;
extern NSString *const DDNA_ENGAGE_URL_PATTERN;
extern NSString *const DDNA_ENGAGE_HASH_URL_PATTERN;

@interface DDNASettings : NSObject

/**
 Instructs the SDK to send a newPlayer event when the game
 is run the first time.
 */
@property (nonatomic, assign) BOOL onFirstRunSendNewPlayerEvent;

/**
 Instructs the SDK to send a clientDevice event when the
 game is started.
 */
@property (nonatomic, assign) BOOL onStartSendClientDeviceEvent;

/**
 Instructs the SDK to send a gameStarted event when the 
 game is started.
 */
@property (nonatomic, assign) BOOL onStartSendGameStartedEvent;

/**
 Controls the delay in seconds between retrying failed
 HTTP requests.
 */
@property (nonatomic, assign) int httpRequestRetryDelaySeconds;

/**
 Controls the number of times the SDK retries a failed
 HTTP request before giving up.
 */
@property (nonatomic, assign) int httpRequestMaxTries;

/**
 Controls the timeout in seconds before the SDK decides
 a HTTP request is unresponsive.
 */
@property (nonatomic, assign) int httpRequestTimeoutSeconds;

/**
 Controls if the SDK should automatically upload events
 in the background.
 */
@property (nonatomic, assign) BOOL backgroundEventUpload;

/**
 Controls how many seconds the SDK waits before starting
 to upload events in the backgroound.
 */
@property (nonatomic, assign) int backgroundEventUploadStartDelaySeconds;

/**
 Controls how frequently the event upload method is called.
 */
@property (nonatomic, assign) int backgroundEventUploadRepeatRateSeconds;

/**
 Returns the path to the privates settings directory on 
 this device.
 */
+ (NSString *)getPrivateSettingsDirectoryPath;

@end