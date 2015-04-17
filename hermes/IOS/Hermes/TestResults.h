//
//  TestResults.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TestResults : NSObject

@property(assign, nonatomic) double dns;
@property(assign, nonatomic) double packetloss;
@property(assign, nonatomic) double latency;
@property(assign, nonatomic) double load;
@property(assign, nonatomic) double throughput;


@end
