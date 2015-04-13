//
//  DDNASDK+Transaction.m
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNASDK+Transaction.h"
#import "DDNAEventBuilder.h"
#import "DDNAProductBuilder.h"

@implementation DDNASDK (Transaction)

- (void) buyVirtualCurrency: (NSString *) virtualCurrencyType
            receivingAmount: (NSInteger) receviedAmount
                   withName: (NSString *) virtualCurrencyName
          usingRealCurrency: (NSString *) realCurrencyType
             spendingAmount: (NSInteger) spendingAmount
        withTransactionName: (NSString *) transactionName
      andTransactionReceipt: (NSString *) transactionReceipt
{
    DDNAEventBuilder * transactionParams = [[DDNAEventBuilder alloc] init];
    [transactionParams setString:@"PURCHASE" forKey:@"transactionType"];
    [transactionParams setString:transactionName forKey:@"transactionName"];
    
    DDNAProductBuilder * productsSpentParams = [[DDNAProductBuilder alloc] init];
    [productsSpentParams setRealCurrency:realCurrencyType withAmount:spendingAmount];
    [transactionParams setProductBuilder:productsSpentParams forKey:@"productsSpent"];
    
    DDNAProductBuilder * productsReceivedParams = [[DDNAProductBuilder alloc] init];
    [productsReceivedParams addVirtualCurrency:virtualCurrencyType withAmount:receviedAmount andName:virtualCurrencyName];
    [transactionParams setProductBuilder:productsReceivedParams forKey:@"productsReceived"];
    
    [transactionParams setString:transactionReceipt forKey:@"transactionReceipt"];
    
    [self recordEvent:@"transaction" withEventBuilder:transactionParams];
}

@end
