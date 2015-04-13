//
//  DDNALog.m
//  DeltaDNASDK
//
//  Created by David White on 06/08/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNALog.h"

@implementation DDNALog

+ (void) log:(int)flag file:(const char *)file function:(const char *)function line:(int)line format:(NSString *)format, ...
{
    va_list args;
    if (format)
    {
        va_start(args, format);
        
        NSString *logMsg = [[NSString alloc] initWithFormat:format arguments:args];
        NSString *fileName = [[NSString stringWithUTF8String:file] lastPathComponent];
        NSString *level;
        
        switch (flag) {
            case DDNA_LOG_FLAG_DEBUG:
                level = @"DEBUG";
                break;
            case DDNA_LOG_FLAG_ERROR:
                level = @"ERROR";
                break;
            case DDNA_LOG_FLAG_INFO:
                level = @"INFO";
                break;
            case DDNA_LOG_FLAG_VERBOSE:
                level = @"VERBOSE";
                break;
            case DDNA_LOG_FLAG_WARN:
                level = @"WARNING";
                break;
            default:
                break;
        }
        
        NSLog(@"<DDNASDK %@:(%d)> [%@] %@", fileName, line, level, logMsg);
        
        va_end(args);
    }
}

@end
