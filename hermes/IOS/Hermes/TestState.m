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

//This is a singleton so we need to keep track of only one instance
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
//Initilize to null
-(instancetype)init
{
    if (self = [super init]) {
        state = IDLE;
        mobileResults = NULL;
        routerResults = NULL;
    }
    return self;
}
//Gets the state as a enum
-(enum State)getState
{
    @synchronized(self)
    {
        return state;
    }
}
//Returns the state to be displayed
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
//Sets the state
-(void)setState:(enum State)arg
{
    @synchronized(self)
    {
        state = arg;
    }
}
//This is used if a state needs a timeout. The current implementation doesn't use this
-(void)timeout:(enum State)arg
{
    @synchronized(self)
    {
        state = arg;
        NSLog(@"Timeout occured switich to IDLE");
    }
}
@end
