//
//  Tester.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/22/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Tester : NSObject
{
    bool started;
    bool shouldRun;
}
@property(strong, atomic) NSThread *thread;
@property(strong, atomic) NSTimer *timer;
//Initilizer for the tester object
-(instancetype)init;

//The main thread for the tester sub system
-(void) threadMain;

//Start the sub system
-(void) start;

//Stop the subsystem
-(void) stop;

//Checks if the subsystem is running
-(BOOL) isRunning;
@end
