//
//  DDNAProductBuilder.h
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DDNAProductBuilder : NSObject

- (void) setRealCurrency: (NSString *) type
              withAmount: (NSInteger) amount;

- (void) addVirtualCurrency: (NSString *) type
                  withAmount: (NSInteger) amount
                    andName: (NSString *) name;

- (void) addItem: (NSString *) type
      withAmount: (NSInteger) amount
         andName: (NSString *) name;

- (NSDictionary *) dictionary;

@end
