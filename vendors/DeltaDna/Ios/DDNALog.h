//
//  DDNALog.h
//  DeltaDNASDK
//
//  Created by David White on 06/08/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 Lightweight logging class for the SDK.
 
 Define Macros to make the interface easier to use.  Enable debug logging by defining
 DDNA_DEBUG=YES.
 */

#define DDNA_LOG_FLAG_ERROR    (1 << 0)  // 0...00001
#define DDNA_LOG_FLAG_WARN     (1 << 1)  // 0...00010
#define DDNA_LOG_FLAG_INFO     (1 << 2)  // 0...00100
#define DDNA_LOG_FLAG_DEBUG    (1 << 3)  // 0...01000
#define DDNA_LOG_FLAG_VERBOSE  (1 << 4)  // 0...10000

#ifndef DDNA_DEBUG
    #define DDNA_DEBUG NO
#endif

#define DDNALogDebug(frmt, ...) do{ if(DDNA_DEBUG) [DDNALog log:DDNA_LOG_FLAG_DEBUG \
                                                           file:__FILE__ \
                                                       function:sel_getName(_cmd) \
                                                           line:__LINE__ \
                                                         format:(frmt), ##__VA_ARGS__]; } while(0)

#define DDNALogWarn(frmt, ...) do{ [DDNALog log:DDNA_LOG_FLAG_WARN \
                                           file:__FILE__ \
                                       function:sel_getName(_cmd) \
                                           line:__LINE__ \
                                         format:(frmt), ##__VA_ARGS__]; } while(0)


@interface DDNALog : NSObject

+ (void)log:(int)flag
       file:(const char *)file
   function:(const char *)function
       line:(int)line
     format:(NSString *)format, ... __attribute__ ((format (__NSString__, 5, 6)));

@end
