//
//  PerformanceTester.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "PerformanceTester.h"
#import "TestSettings.h"
#import "TestState.h"

@interface PerformanceTester()

@property (strong, nonatomic) TestSettings* settings;
@property (assign, nonatomic) NSInteger timeStart;
@property (assign, nonatomic) NSInteger timeEnd;

-(Boolean) sendDNSRequest :(NSString*)string withId:(NSInteger)identifier;
-(NSArray*) runDNSTest: (NSMutableArray*)domains;
-(Boolean) getDNSResponse:(NSInteger)identifier;

-(NSMutableArray*) getContent:(NSString*) name :(NSInteger) identifier;

@end

@implementation PerformanceTester
@synthesize settings, timeEnd, timeStart;

-(instancetype)init:(TestSettings *)settingst
{
    self = [super init];
    
    if(self)
    {
        self.settings = settingst;
    }
    return self;
}

-(TestResults *)runTests{
    TestResults *results = [[TestResults alloc] init];
    [results setMobileIdentifier:(int)[settings mobileResultID]];
    
    TestState *state = [TestState getStateMachine];
    
    [state setState:TESTINGDNS];
    
    [results setRouterIdentifier:[settings routerTesultID]];
    
    
    //Run a DNS Response Test
    NSArray *times1 = [self runDNSTest:[settings invalidDomains]];
    double dnsResult = 0;
    for (NSNumber *x in times1) {
        dnsResult += [x doubleValue];
    }

    dnsResult /= [times1 count];
    [results setDns:dnsResult];
    NSLog(@"DNS Result = %f", dnsResult);
    
    [state setState:TESTINGLATENCY];
    

    //Run a test packet latency test
    NSArray *times2 = [self runDNSTest:[settings validDomains]];
    double latencyResult = 0;
    for (NSNumber *x in times2) {
        latencyResult += [x doubleValue];
    }
    
    latencyResult /= [times2 count];
    
    [results setLatency:latencyResult];
    NSLog(@"Latency Result = %f", latencyResult);

        
    //Packet Loss is just 1 - times we have / times we should have
    [results setPacketloss:(1-([times2 count] / [[settings validDomains] count]))];
    
    NSLog(@"Packet loss = %f percent", [results packetloss]);

    [state setState:TESTINGTHROUGHPUT];
    //TODO: Run a throughput response
    [NSThread sleepForTimeInterval:4000];
    
    return results;
}
-(NSArray *)runDNSTest:(NSMutableArray *)domains{
    return NULL;
}

-(Boolean)sendDNSRequest:(NSString *)string withId:(NSInteger)identifier{
    return false;
}

-(Boolean)getDNSResponse:(NSInteger)identifier{
    return false;
}
-(NSMutableArray *)getContent:(NSString *)name :(NSInteger)identifier{
    return false;
}

@end
