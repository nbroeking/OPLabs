//
//  TestState.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 3/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

//THis is a state machine that keeps track of all information regarding a performance test

#import <Foundation/Foundation.h>
#import "TestResults.h"

@interface TestState : NSObject

enum State {IDLE, COMPLETED, PREPARING, TESTINGDNS, TESTINGLATENCY, TESTINGTHROUGHPUT};

@property(strong, atomic) TestResults *mobileResults;
@property(strong, atomic) TestResults *routerResults;
@property(assign, atomic) NSTimeInterval startTime;
@property(assign, atomic) double totalEstimatedRunTime;

+(TestState*) getStateMachine;

-(enum State)getState;
-(void)setState:(enum State)arg;
-(void)timeout:(enum State)arg;
-(NSString*)getStateAsString;
@end
