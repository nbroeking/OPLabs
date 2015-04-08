//
//  TestState.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 3/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TestState : NSObject

enum State {IDLE, COMPLETED, PREPARING, TESTINGPING};

-(enum State)getState;
-(void)setState:(enum State)arg;
-(void)timeout:(enum State)arg;
@end
