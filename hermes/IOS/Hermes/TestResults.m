//
//  TestResults.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestResults.h"

@implementation TestResults
@synthesize dns, latency, load, packetloss, throughput;

-(instancetype)init{
    if (self = [super init]) {
        
        dns = 0.0;
        latency = 0.0;
        load = 0.0;
        packetloss = 0.0;
        throughput = 0.0;
    }
    return self;
}
@end
