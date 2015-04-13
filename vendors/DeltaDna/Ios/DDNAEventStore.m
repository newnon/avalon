#import "DDNAEventStore.h"
#import "DDNAPlayerPrefs.h"
#import "DDNALog.h"

static NSString *const PP_KEY_IN_FILE = @"DDSDK_EVENT_IN_FILE";
static NSString *const PP_KEY_OUT_FILE = @"DDSDK_EVENT_OUT_FILE";
static NSString *const FILE_A = @"A";
static NSString *const FILE_B = @"B";
static long long MAX_FILE_SIZE_BYTES = 40 * 1024 * 1024;

@interface DDNAEventStore ()
{
    NSFileHandle * inFileHandle;
    NSString * inFilePath;
    NSFileHandle * outFileHandle;
    NSString * outFilePath;
    BOOL initialised;
}

@end

@implementation DDNAEventStore

- (instancetype) initWithStorePath: (NSString *) path
{
    return [self initWithStorePath:path clearStore:NO];
}

- (instancetype) initWithStorePath: (NSString *) path clearStore: (BOOL) clearStore
{
    if ((self = [super init]))
    {
        [self initialiseFileStreamsInDir:path clearStore:clearStore];
        initialised = true;
    }
    return self;
}

- (BOOL) pushEvent: (NSDictionary *) eventDictionary
{
    @synchronized(self)
    {
        if (initialised && [inFileHandle offsetInFile] < MAX_FILE_SIZE_BYTES)
        {
            @try
            {
                // Serialise to a JSON byte array
                NSError * error = nil;
                NSData * eventData = [NSJSONSerialization dataWithJSONObject:eventDictionary
                                                                     options:0
                                                                       error:&error];
                if (error != nil && error.code != 0)
                {
                    DDNALogDebug(@"Event Store failed to serialize event %@ to JSON, got %li", eventDictionary, (long)error.code);
                    return false;
                }
                
                // Storage format is simply length:record in utf-8.
                NSUInteger eventLength = eventData.length;
                NSMutableData *bytes = [NSMutableData data];
                [bytes appendBytes:&eventLength length:sizeof(eventLength)];
                [bytes appendData:eventData];
                
                [inFileHandle writeData:bytes];
                return true;
            }
            @catch (NSException *exception)
            {
                DDNALogDebug(@"Problem pushing event to Event Store: %@", exception.reason);
            }
        }
        return false;
    }
}

- (BOOL) swap
{
    @synchronized(self)
    {
        // Only swap if the out buffer is empty.
        unsigned long long fileOffset = outFileHandle.offsetInFile;
        if ([outFileHandle seekToEndOfFile] == 0)
        {
            // Close off the write stream.
            [inFileHandle synchronizeFile];
            // Swap the file handles and paths.
            NSFileHandle *temp = inFileHandle;
            inFileHandle = outFileHandle;
            outFileHandle = temp;
            NSString *tempPath = inFilePath;
            inFilePath = outFilePath;
            outFilePath = tempPath;
            
            // Reset the write stream.
            [inFileHandle truncateFileAtOffset:0];
            // Reset the read stream.
            [outFileHandle seekToFileOffset:0];
            
            [DDNAPlayerPrefs setObject:[inFilePath lastPathComponent] forKey:PP_KEY_IN_FILE];
            [DDNAPlayerPrefs setObject:[outFilePath lastPathComponent] forKey:PP_KEY_OUT_FILE];
            [DDNAPlayerPrefs save];
            
            return true;
        }
        else
        {
            [outFileHandle seekToFileOffset:fileOffset];
            return false;
        }
    }
}

- (NSArray *) read
{
    @synchronized(self)
    {
        // Return an array of Strings, the JSON representation of each event.
        NSMutableArray *results = [NSMutableArray array];
        @try
        {
            NSData *lengthField;
            NSData *eventField;
            NSUInteger eventLength = 0;
            
            while ([lengthField = [outFileHandle readDataOfLength:sizeof(eventLength)] length] > 0)
            {
                [lengthField getBytes:&eventLength length:sizeof(eventLength)];
                eventField = [outFileHandle readDataOfLength:eventLength];
                [results addObject:[[NSString alloc] initWithData:eventField encoding:NSUTF8StringEncoding]];
            }
            // reset so we can read again
            [outFileHandle seekToFileOffset:0];
        }
        @catch (NSException *exception)
        {
            DDNALogDebug(@"Problem reading events from the Event Store: %@", exception.reason);
        }
        
        return results;
    }
}

- (void) clear
{
    @synchronized(self)
    {
        [outFileHandle truncateFileAtOffset:0];
    }
}

- (void) dealloc
{
    [inFileHandle synchronizeFile];
    [inFileHandle closeFile];
    [outFileHandle synchronizeFile];
    [outFileHandle closeFile];
}

// Private Methods //

- (void) initialiseFileStreamsInDir: (NSString *) storeDir clearStore: (BOOL) clearStore
{
    // Create directory if doesn't exist.
    NSError *error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:storeDir withIntermediateDirectories:YES attributes:nil error:&error];

    NSString *inFile = [DDNAPlayerPrefs getObjectForKey:PP_KEY_IN_FILE withDefault:FILE_A];
    NSString *outFile = [DDNAPlayerPrefs getObjectForKey:PP_KEY_OUT_FILE withDefault:FILE_B];
    inFile = [inFile lastPathComponent];    // support for old event stores that may have full path
    outFile = [outFile lastPathComponent];
    
    inFilePath = [storeDir stringByAppendingPathComponent:inFile];
    outFilePath = [storeDir stringByAppendingPathComponent:outFile];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    inFileHandle = [NSFileHandle fileHandleForUpdatingAtPath:inFilePath];
    if (inFileHandle == nil)
    {
        DDNALogDebug(@"In file %@ not found", inFilePath);
        [fileManager createFileAtPath:inFilePath contents:nil attributes:nil];
        inFileHandle = [NSFileHandle fileHandleForUpdatingAtPath:inFilePath];
        
        if (inFileHandle == nil)
        {
            DDNALogDebug(@"Failed to create in file");
        }
        else
        {
            DDNALogDebug(@"In file created");
        }
    }
    else
    {
        DDNALogDebug(@"In file %@ exists", inFilePath);
        [inFileHandle seekToEndOfFile];
    }
    
    outFileHandle = [NSFileHandle fileHandleForUpdatingAtPath:outFilePath];
    if (outFileHandle == nil)
    {
        DDNALogDebug(@"Out file %@ not found", outFilePath);
        [fileManager createFileAtPath:outFilePath contents:nil attributes:nil];
        outFileHandle = [NSFileHandle fileHandleForUpdatingAtPath:outFilePath];
        
        if (outFileHandle == nil)
        {
            DDNALogDebug(@"Failed to create out file");
        }
        else
        {
            DDNALogDebug(@"Out file created");
        }
    }
    else
    {
        DDNALogDebug(@"Out file %@ exists", outFilePath);
        [outFileHandle seekToFileOffset:0];
    }
    
    if (clearStore == YES)
    {
        [inFileHandle truncateFileAtOffset:0];
        [outFileHandle truncateFileAtOffset:0];
    }
    
    [DDNAPlayerPrefs setObject:[inFilePath lastPathComponent] forKey:PP_KEY_IN_FILE];
    [DDNAPlayerPrefs setObject:[outFilePath lastPathComponent] forKey:PP_KEY_OUT_FILE];
    [DDNAPlayerPrefs save];
}

@end