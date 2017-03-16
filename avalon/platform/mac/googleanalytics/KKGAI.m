#import "KKGAI.h"
#import "KKGAIFields.h"
#import "KKGAISystemInfo.h"

@interface MacGAILogger : NSObject <GAILogger>

@property (nonatomic, assign) GAILogLevel logLevel;

- (void)verbose:(NSString *)message;
- (void)info:(NSString *)message;
- (void)warning:(NSString *)message;
- (void)error:(NSString *)message;
@end

@implementation MacGAILogger

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.logLevel = kGAILogLevelError;
    }
    return self;
}

- (void)verbose:(NSString *)message
{
    if(self.logLevel>=kGAILogLevelVerbose)
        NSLog(@"%@", message);
}

- (void)info:(NSString *)message
{
    if(self.logLevel>=kGAILogLevelInfo)
        NSLog(@"%@", message);
}

- (void)warning:(NSString *)message
{
    if(self.logLevel>=kGAILogLevelWarning)
        NSLog(@"%@", message);
}

- (void)error:(NSString *)message
{
    if(self.logLevel>=kGAILogLevelError)
        NSLog(@"%@", message);
}
@end



@interface NSString (HTTPFormExtensions)
+ (instancetype)stringAsWWWURLEncodedFormFromDictionary:(NSDictionary *)formDictionary;
@end

NS_INLINE NSString *KKEscape(NSString *inValue)
{
	CFStringRef escaped = CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)inValue, NULL, CFSTR("&+"), CFStringConvertNSStringEncodingToEncoding(NSUTF8StringEncoding));
	return CFBridgingRelease(escaped);
}

@implementation NSString (HTTPFormExtensions)
+ (instancetype)stringAsWWWURLEncodedFormFromDictionary:(NSDictionary *)formDictionary
{
	NSMutableString *combinedDataString = [NSMutableString string];
	NSArray *allKeys = [formDictionary allKeys];
	for (id key in allKeys) {
		id value = formDictionary[key];
		if ([value respondsToSelector:@selector(stringValue)]) {
			value = [value stringValue];
		}
		[combinedDataString appendFormat:@"%@=%@", KKEscape(key), KKEscape(value)];
		if (key != [allKeys lastObject]) {
			[combinedDataString appendString:@"&"];
		}
	}
	return combinedDataString;
}
@end

static NSString *GenerateUUIDString()
{
    CFUUIDRef uuid = CFUUIDCreate(NULL);
    CFStringRef uuidStr = CFUUIDCreateString(NULL, uuid);
    CFRelease(uuid);
    return CFBridgingRelease(uuidStr);
}

static NSString *const KKGoogleAnalyticsErrorDomain = @"KKGoogleAnalyticsErrorDomain";

@interface GAI()
@property (strong, nonatomic) NSTimer *timer;
@property (strong, nonatomic) NSString *clientID;
@property (strong, nonatomic) NSString *screenResolution;
@property (strong, nonatomic) NSString *screenDepth;
@property (strong, nonatomic) NSString *language;
@property (strong, nonatomic) NSOperationQueue *operationQueue;
@property (strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;
@property (strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (strong, nonatomic) NSManagedObjectContext *managedObjectContext;
@property (strong, nonatomic) NSMutableDictionary *trackers;
@end

@interface MacGAITracker : NSObject<GAITracker>
@property (strong, nonatomic) NSString *trackingID;
@property (strong, nonatomic) NSString *name;
@property (strong, nonatomic) NSMutableDictionary *properties;
@end

@implementation MacGAITracker

- (instancetype)initWithTrackingID:(NSString*)trackingID andName:(NSString*)name
{
    self = [super init];
    if (self) {
        self.trackingID = trackingID;
        self.name = name;
        self.properties = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)dealloc
{
}

- (void)set:(NSString *)parameterName
      value:(NSString *)value
{
    self.properties[parameterName] = value;
}

- (NSString *)get:(NSString *)parameterName
{
    return [self.properties objectForKey:parameterName];
}

- (void)send:(NSDictionary *)params
{
    NSAssert(self.trackingID != nil, @"Must have self.trackingID");
    NSAssert([self.trackingID length] > 0, @"Must have self.trackingID");
    NSAssert([GAI sharedInstance].clientID != nil, @"Must have self.clientID");
    NSAssert([[GAI sharedInstance].clientID length] > 0, @"Must have self.clientID");
    
    NSMutableDictionary *payload = [NSMutableDictionary dictionaryWithDictionary:params];
    NSParameterAssert(payload);
    payload[@"v"] = @1;
    payload[kGAITrackingId] = self.trackingID;
    payload[kGAIClientId] = [GAI sharedInstance].clientID;
    
#define SET(k, v) if(v) { payload[k] = v; }
    SET(kGAIScreenResolution, [GAI sharedInstance].screenResolution);
    SET(kGAIScreenColors, [GAI sharedInstance].screenDepth);
    SET(kGAILanguage, [GAI sharedInstance].language);
#undef SET
    
    [self.properties enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
        payload[key] = obj;
    }];
    
    NSString *text = [NSString stringAsWWWURLEncodedFormFromDictionary:payload];
    void (^insertAndSaveBlock)(void) = ^{
        NSManagedObject *object = [NSEntityDescription insertNewObjectForEntityForName:@"Record" inManagedObjectContext:[GAI sharedInstance].managedObjectContext];
        [object setValue:text forKey:@"text"];
        [[GAI sharedInstance].managedObjectContext insertObject:object];
        [[GAI sharedInstance].managedObjectContext save:nil];
    };
    if (![NSThread isMainThread]) {
        dispatch_async(dispatch_get_main_queue(), insertAndSaveBlock);
    }
    else {
        insertAndSaveBlock();
    }
}

@end

@implementation GAI

- (id<GAITracker>)trackerWithName:(NSString *)name
                         trackingId:(NSString *)trackingId
{
    id<GAITracker> ret =[self.trackers objectForKey:name];
    if(!ret)
    {
        ret = [[MacGAITracker alloc] initWithTrackingID:trackingId andName:name];
        self.trackers[name] = ret;
    }
    if(!self.defaultTracker)
        self.defaultTracker = ret;
    return ret;
}

- (id<GAITracker>)trackerWithTrackingId:(NSString *)trackingId
{
    return [self trackerWithName:trackingId trackingId:trackingId];
}

- (void)removeTrackerByName:(NSString *)name
{
    [self.trackers removeObjectForKey:name];
    if (self.defaultTracker && self.defaultTracker.name) {
        NSArray *values = [self.trackers allValues];
        if ([values count] != 0)
            self.defaultTracker = [values objectAtIndex:0];
        else
            self.defaultTracker = nil;
    }
}

+ (instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    static GAI *sharedInstance;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[GAI alloc] init];
    });
    return sharedInstance;
}

- (void)dealloc
{
	[self.timer invalidate];
	self.timer = nil;
	[self.operationQueue cancelAllOperations];
}

- (instancetype)init
{
	self = [super init];
	if (self) {
        self.logger = [[MacGAILogger alloc] init];
        self.trackers = [NSMutableDictionary dictionary];
        self.dispatchInterval = 120;
		self.operationQueue = [[NSOperationQueue alloc] init];
		self.operationQueue.maxConcurrentOperationCount = 1;
		NSString *existingClientID = [[NSUserDefaults standardUserDefaults] stringForKey:@"GoogleAnalyticsClientID"];
		if (!existingClientID) {
			existingClientID = GenerateUUIDString();
			[[NSUserDefaults standardUserDefaults] setObject:existingClientID forKey:@"GoogleAnalyticsClientID"];
			[[NSUserDefaults standardUserDefaults] synchronize];
		}
		NSArray *languages = [[NSUserDefaults standardUserDefaults] objectForKey:@"AppleLanguages"];
		self.language = languages[0];
		self.clientID = existingClientID;
		self.screenResolution = [GAISystemInfo screenResolutionString];
		self.screenDepth = [GAISystemInfo screenDepthString];
        [NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(timer:) userInfo:nil repeats:NO];
		[self _scheduleTimer];
	}
	return self;
}

- (void)timer:(NSTimer *)timer
{
	[self dispatch];
}

- (void)_scheduleTimer
{
	if (self.timer) {
		[self.timer invalidate];
		self.timer = nil;
	}
	self.timer = [NSTimer scheduledTimerWithTimeInterval:self.dispatchInterval target:self selector:@selector(timer:) userInfo:nil repeats:YES];
}

- (void) setDispatchInterval:(NSTimeInterval) interval
{
    _dispatchInterval = interval;
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
        self.timer = [NSTimer scheduledTimerWithTimeInterval:self.dispatchInterval target:self selector:@selector(timer:) userInfo:nil repeats:YES];
    }
}

- (void)dispatch
{
    [self dispatchWithCompletionHandler:nil];
}

- (void)dispatchWithCompletionHandler:(void (^)(GAIDispatchResult))completionHandler
{
    if ([self.operationQueue operationCount]) {
        if(completionHandler)
            dispatch_async(dispatch_get_main_queue(), ^{ completionHandler(kGAIDispatchError); });
        return;
    }
    
    __block NSError *error = nil;
    __block NSArray *array = nil;
    void (^getArrayBlock)(void) = ^{
        NSEntityDescription *entityDescription = [NSEntityDescription entityForName:@"Record" inManagedObjectContext:self.managedObjectContext];
        NSFetchRequest *request = [[NSFetchRequest alloc] init];
        [request setEntity:entityDescription];
        array = [self.managedObjectContext executeFetchRequest:request error:&error];
    };
    if (![NSThread isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), getArrayBlock);
    }
    else {
        getArrayBlock();
    }
    if (!array || ![array count]) {
        if(completionHandler)
        {
            dispatch_async(dispatch_get_main_queue(), ^{ completionHandler(kGAIDispatchNoData); });
        }
        return;
    }
    NSMutableString *payload = [NSMutableString string];
    for(NSUInteger idx = 0; idx<array.count;++idx) {
        [payload appendFormat:idx<(array.count-1)?@"%@\n":@"%@", [[array objectAtIndex:idx] valueForKey:@"text"]];
    }
    
    NSURL *URL = [NSURL URLWithString:@"https://www.google-analytics.com/collect"];
    NSMutableURLRequest *HTTPRequest = [NSMutableURLRequest requestWithURL:URL cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:60.0];
    [HTTPRequest setHTTPMethod:@"POST"];
    [HTTPRequest setHTTPBody:[payload dataUsingEncoding:NSUTF8StringEncoding]];
    [HTTPRequest addValue:UserAgentString() forHTTPHeaderField:@"User-Agent"];
    
    [NSURLConnection sendAsynchronousRequest:HTTPRequest queue:self.operationQueue completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
        if (!connectionError) {
            dispatch_async(dispatch_get_main_queue(), ^{
                for (id object in array) {
                    [self.managedObjectContext deleteObject:object];
                }
                [self.managedObjectContext save:nil];
                
                if(completionHandler)
                    completionHandler(kGAIDispatchGood);
            });
        }
        else
        {
            if(completionHandler)
                dispatch_async(dispatch_get_main_queue(), ^{ completionHandler(kGAIDispatchError); });
        }
    }];
    return;
}

- (void)startDispatching
{
	[self _scheduleTimer];
}

- (void)pauseDispatching
{
	if (self.timer) {
		[self.timer invalidate];
		self.timer = nil;
	}
}

@end

@implementation GAI (CoreData)

- (NSURL *)applicationFilesDirectory
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSURL *appSupportURL = [[fileManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] lastObject];
	NSString *bundleID = [NSBundle mainBundle].bundleIdentifier;
	return [appSupportURL URLByAppendingPathComponent:bundleID];
}

- (NSManagedObjectModel *)managedObjectModel
{
	if (_managedObjectModel) {
		return _managedObjectModel;
	}
    _managedObjectModel = [[NSManagedObjectModel alloc] init];
    
    NSEntityDescription *entity = [[NSEntityDescription alloc] init];
    [entity setName:@"Record"];
    [entity setManagedObjectClassName:@"Record"];
    
    NSMutableArray *properties = [NSMutableArray array];
    NSAttributeDescription *textAttribute = [[NSAttributeDescription alloc] init];
    [textAttribute setName:@"text"];
    [textAttribute setAttributeType:NSStringAttributeType];
    [textAttribute setOptional:YES];
    [properties addObject:textAttribute];
    
    // add attributes to entity
    [entity setProperties:properties];
    
    // add entity to model
    [_managedObjectModel setEntities:[NSArray arrayWithObject:entity]];
    
	return _managedObjectModel;
}

- (NSPersistentStoreCoordinator *)persistentStoreCoordinator
{
	if (_persistentStoreCoordinator) {
		return _persistentStoreCoordinator;
	}

	NSManagedObjectModel *mom = [self managedObjectModel];
	if (!mom) {
		[self.logger error: [NSString stringWithFormat:@"%@:%@ No model to generate a store from", [self class], NSStringFromSelector(_cmd)]];
		return nil;
	}

	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSURL *applicationFilesDirectory = [self applicationFilesDirectory];
	NSError *error = nil;

	NSDictionary *properties = [applicationFilesDirectory resourceValuesForKeys:@[NSURLIsDirectoryKey] error:&error];

	if (!properties) {
		BOOL ok = NO;
		if ([error code] == NSFileReadNoSuchFileError) {
			ok = [fileManager createDirectoryAtPath:[applicationFilesDirectory path] withIntermediateDirectories:YES attributes:nil error:&error];
		}
		if (!ok) {
			[self.logger error:[error localizedDescription]];
			return nil;
		}
	}
	else {
		if (![properties[NSURLIsDirectoryKey] boolValue]) {
			// Customize and localize this error.
			NSString *failureDescription = [NSString stringWithFormat:@"Expected a folder to store application data, found a file (%@).", [applicationFilesDirectory path]];

			NSMutableDictionary *dict = [NSMutableDictionary dictionary];
			[dict setValue:failureDescription forKey:NSLocalizedDescriptionKey];
			error = [NSError errorWithDomain:KKGoogleAnalyticsErrorDomain code:101 userInfo:dict];

			[self.logger error:[error localizedDescription]];
			return nil;
		}
	}

	NSURL *storedDataURL = [applicationFilesDirectory URLByAppendingPathComponent:@"GoogleAnalytics.storedata"];
	NSPersistentStoreCoordinator *coordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:mom];
	NSString *type = NSSQLiteStoreType; // NSXMLStoreType
	if (![coordinator addPersistentStoreWithType:type configuration:nil URL:storedDataURL options:nil error:&error]) {
		[self.logger error:[error localizedDescription]];
		return nil;
	}
	_persistentStoreCoordinator = coordinator;

	return _persistentStoreCoordinator;
}

- (NSManagedObjectContext *)managedObjectContext
{
	if (_managedObjectContext) {
		return _managedObjectContext;
	}

	NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
	if (!coordinator) {
		[self.logger error:@"Failed to initialize the store"];
		return nil;
	}
	_managedObjectContext = [[NSManagedObjectContext alloc] init];
	[_managedObjectContext setPersistentStoreCoordinator:coordinator];

	return _managedObjectContext;
}

@end


