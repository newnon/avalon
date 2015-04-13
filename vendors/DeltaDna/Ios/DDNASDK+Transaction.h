//
//  DDNASDK+Transaction.h
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNASDK.h"

@interface DDNASDK (Transaction)

- (void) buyVirtualCurrency: (NSString *) virtualCurrencyType
            receivingAmount: (NSInteger) receviedAmount
                   withName: (NSString *) virtualCurrencyName
          usingRealCurrency: (NSString *) realCurrencyType
             spendingAmount: (NSInteger) spendingAmount
        withTransactionName: (NSString *) transactionName
      andTransactionReceipt: (NSString *) transactionReceipt;

@end
