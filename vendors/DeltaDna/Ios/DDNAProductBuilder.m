//
//  DDNAProductBuilder.m
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNAProductBuilder.h"

@interface DDNAProductBuilder ()
{
    NSMutableDictionary * _realCurrency;
    NSMutableArray * _virtualCurrencies;
    NSMutableArray * _items;
}

@end

@implementation DDNAProductBuilder


- (void) setRealCurrency: (NSString *) type withAmount: (NSInteger) amount
{
    if (_realCurrency == nil)
    {
        _realCurrency = [NSMutableDictionary dictionary];
    }
    
    [_realCurrency setObject:type forKey:@"realCurrencyType"];
    [_realCurrency setObject:[NSNumber numberWithInteger:amount] forKey:@"realCurrencyAmount"];
}

- (void) addVirtualCurrency: (NSString *) type withAmount: (NSInteger) amount andName: (NSString *) name
{
    if (_virtualCurrencies == nil)
    {
        _virtualCurrencies = [NSMutableArray array];
    }
    
    NSDictionary * virtualCurrency = [NSDictionary dictionaryWithObjectsAndKeys:
                                      name, @"virtualCurrencyName",
                                      type, @"virtualCurrencyType",
                                      [NSNumber numberWithInteger:amount], @"virtualCurrencyAmount",
                                      nil];
    
    [_virtualCurrencies addObject:[NSDictionary dictionaryWithObjectsAndKeys:
                                   virtualCurrency, @"virtualCurrency",
                                   nil]];
}

- (void) addItem: (NSString *) type withAmount: (NSInteger) amount andName: (NSString *) name
{
    if (_items == nil)
    {
        _items = [NSMutableArray array];
    }
    
    NSDictionary * item = [NSDictionary dictionaryWithObjectsAndKeys:
                           name, @"itemName",
                           type, @"itemType",
                           [NSNumber numberWithInteger:amount], @"itemAmount",
                           nil];

    [_items addObject:[NSDictionary dictionaryWithObjectsAndKeys:
                       item, @"item",
                       nil]];
}

- (NSDictionary *) dictionary
{
    NSMutableDictionary * result = [NSMutableDictionary dictionary];
    
    if (_realCurrency != nil)
    {
        [result setObject:_realCurrency forKey:@"realCurrency"];
    }
    if (_virtualCurrencies != nil)
    {
        [result setObject:_virtualCurrencies forKey:@"virtualCurrencies"];
    }
    if (_items != nil)
    {
        [result setObject:_items forKey:@"items"];
    }
    
    return result;
}

@end
