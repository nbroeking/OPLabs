//
//  TestResults.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestResults.h"

@implementation TestResults
@synthesize dns, latency, packetloss, throughputUpload, throughputDownload, packetlossUnderLoad, latencyUnderLoad, mobileIdentifier, valid, routerIdentifier;

-(instancetype)init :(NSDictionary*)json{
    if( self = [super init]){
        
        valid = false;
        dns = 1.0;
        latency = 1.0;
        packetloss = 0.0;
        throughputDownload = 0.0;
        throughputUpload = 0.0;
        
    }
    return self;
}
-(instancetype)init{
    if (self = [super init]) {
        
        valid = false;
        dns = 1.0;
        latency = 1.0;
        packetloss = 0.0;
        throughputUpload = 1.0;
    }
    return self;
}
-(NSString *)getPost{
    
    if( !valid){
        return @"state=error";
    }
    return [[NSString alloc] initWithFormat:@"state=finished&&dns_response_avg=%f&packet_loss=%f&latency_avg=%f",dns,packetloss,latency];
    
}
@end
