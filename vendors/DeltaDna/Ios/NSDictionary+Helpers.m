#import "NSDictionary+Helpers.h"
#import "NSString+Helpers.h"

@implementation NSDictionary (Helpers)

+ (NSDictionary *) dictionaryWithJSONString: (NSString *) jsonString
{    
    if ([NSString stringIsNilOrEmpty:jsonString])
    {
        return [NSDictionary dictionary];
    }
    
    NSData * data = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError * error = nil;
    NSDictionary * result = [NSJSONSerialization JSONObjectWithData:data
                                                            options:kNilOptions
                                                              error:&error];
    if (error != 0)
    {
        return [NSDictionary dictionary];
    }
    
    return result;
}

@end