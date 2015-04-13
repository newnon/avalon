#import "DDNAPlayerPrefs.h"
#import "DDNALog.h"

static NSString *const PF_FILE_NAME = @"settings.plist";

static NSMutableDictionary *sPreferences;

@implementation DDNAPlayerPrefs

+ (void) initialize
{
    if (self == [DDNAPlayerPrefs self])
    {
        NSString *path = [[DDNAPlayerPrefs getPrivateDocsDir] stringByAppendingPathComponent:PF_FILE_NAME];
        sPreferences = [NSMutableDictionary dictionaryWithContentsOfFile:path];
        
        // If no file exists create a new empty dictionary
        if (sPreferences == nil)
        {
            sPreferences = [NSMutableDictionary dictionary];
        }
    }
}

+ (void) setObject:(NSObject *)object forKey:(NSString *)key
{
    @synchronized(sPreferences)
    {
        [sPreferences setObject:object forKey:key];
    }
}

+ (void) setInteger:(int)integer forKey:(NSString *)key
{
    @synchronized(sPreferences)
    {
        [sPreferences setObject:[NSNumber numberWithInt:integer] forKey:key];
    }
}

+ (id) getObjectForKey: (NSString *) key withDefault: (NSObject *) defaultObject
{
    @synchronized(sPreferences)
    {
        NSObject *object = [sPreferences objectForKey:key];
        return (object == nil) ? defaultObject : object;
    }
}

+ (int) getIntegerForKey:(NSString *)key withDefault:(int)defaultInteger
{
    @synchronized(sPreferences)
    {
        NSNumber *number = [sPreferences objectForKey:key];
        return (number == nil) ? defaultInteger : [number intValue];
    }
}

+ (void) deleteKey: (NSString *) key
{
    @synchronized(sPreferences)
    {
        [sPreferences removeObjectForKey:key];
    }
}

+ (void) clear
{
    @synchronized(sPreferences)
    {
        [sPreferences removeAllObjects];
        [DDNAPlayerPrefs save];
    }
}

+ (void) save
{
    NSString *path = [[DDNAPlayerPrefs getPrivateDocsDir] stringByAppendingPathComponent:PF_FILE_NAME];
    [sPreferences writeToFile:path atomically:YES];
}

+ (NSString *) getPrivateDocsDir
{
    // Finds the default location for saving user data on MacOS/iOS
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"DeltaDNA"];
    
    NSError *error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&error];
    return documentsDirectory;
}

@end