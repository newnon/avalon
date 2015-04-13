// Holds the eventsv to be posted up to Collect

#include <Foundation/Foundation.h>

@interface DDNAEventStore : NSObject

- (instancetype) initWithStorePath: (NSString *) path;
- (instancetype) initWithStorePath: (NSString *) path clearStore: (BOOL) clearStore;
- (BOOL) pushEvent: (NSDictionary *) event;
- (BOOL) swap;
- (NSArray *) read;
- (void) clear;

@end