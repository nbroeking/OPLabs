//
//  TestResults.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestResults.h"

@implementation TestResults
@synthesize dns, latency, load, packetloss, throughput, mobileIdentifier, valid, routerIdentifier;

-(instancetype)init{
    if (self = [super init]) {
        
        valid = false;
        dns = 1.0;
        latency = 1.0;
        load = 1.0;
        packetloss = 0.0;
        throughput = 1.0;
    }
    return self;
}
-(NSString *)getPost{
    
    return [[NSString alloc] initWithFormat:@"state=finished&&dns_response_avg=%f&packet_loss=%f&latency_avg=%f",dns,packetloss,latency];
    
}
@end
