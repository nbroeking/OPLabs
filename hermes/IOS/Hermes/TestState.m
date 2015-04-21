//
//  TestState.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 3/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestState.h"
@interface TestState()
{
    enum State state;
}

@end

@implementation TestState
@synthesize routerResults, mobileResults, startTime;

+(TestState*) getStateMachine
{
    @synchronized(self){
    static TestState *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[TestState alloc] init];
    });
    return sharedInstance;
    }
}

-(instancetype)init
{
    if (self = [super init]) {
        state = IDLE;
        mobileResults = NULL;
        routerResults = NULL;
    }
    return self;
}

-(enum State)getState
{
    @synchronized(self)
    {
        return state;
    }
}
-(NSString*)getStateAsString{
    switch (state) {
        case IDLE:
            return @"Idle";
            break;
        case COMPLETED:
            return @"Completed";
            break;
        case PREPARING:
            return @"Preparing";
            break;
        case TESTINGDNS:
            return @"Testing DNS";
            break;
        case TESTINGLATENCY:
            return @"Testing Latency";
            break;
        case TESTINGTHROUGHPUT:
            return @"Testing Throughput";
            break;
        default:
            return @"";
            break;
    }
}
-(void)setState:(enum State)arg
{
    @synchronized(self)
    {
        state = arg;
        NSLog(@"Setting state to %d", arg);
    }
}
-(void)timeout:(enum State)arg
{
    @synchronized(self)
    {
        //TODO: We should be telling things that there was a timeout
        state = arg;
        NSLog(@"Timeout occured switich to IDLE");
    }
}
@end
