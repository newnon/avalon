// Simple wrapper for persisting player game data

#import <Foundation/Foundation.h>

@interface DDNAPlayerPrefs : NSObject

+ (void) setObject: (NSObject *) object forKey: (NSString *) key;
+ (void) setInteger: (int) integer forKey: (NSString *) key;

+ (id) getObjectForKey: (NSString *) key withDefault: (NSObject *) defaultObject;
+ (int) getIntegerForKey: (NSString *) key withDefault: (int) defaultInteger;

+ (void) deleteKey: (NSString *) key;
+ (void) clear;
+ (void) save;

@end