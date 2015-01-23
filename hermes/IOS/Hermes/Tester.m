//
//  Tester.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/22/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Tester.h"
@interface Tester()

-(void) tearDownRunLoop;
@end

@implementation Tester
@synthesize thread;
//TODO
-(instancetype)init
{
    return self;
}
//Main Thread Run Loop
-(void)threadMain
{
    NSLog(@"Tester Start");

    while(shouldRun)
    {
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        NSLog(@"Run Loop Reset");
    }
    @synchronized(self){
        NSLog(@"Starting Tester Tear down");
        
        //Reseting member variables
        thread = nil;
        started = false;
        shouldRun = false;
    }
    NSLog(@"Tester Thread Closed");
}
//Start the Communication thread
-(void)start{
    
    @synchronized(self){
        
        if( started)
        {
            NSLog(@"Communication already started: try again");
            return;
        }
        shouldRun = true;
        started = true;
        
        NSLog(@"Starting Threads");
        //Start the run loop
        thread = [[NSThread alloc] initWithTarget:self selector:@selector(threadMain) object:nil];
        [thread setName:@"Communication"];
        [thread start];
    }
}
//Kill the communication thread cleanly
-(void)stop{
    @synchronized(self){
        if(!started)
        {
            NSLog(@"Thread is already stopped");
            return;
        }
        shouldRun = false;
        [self performSelector:@selector(tearDownRunLoop) onThread:thread withObject:nil waitUntilDone:false];
        NSLog(@"Tester Stop Requested");
    }
}
-(void)tearDownRunLoop
{
    NSLog(@"Trying to kill run loop");
    //When this method exits the run loop will close
}
-(BOOL)isRunning
{
    @synchronized(self)
    {
        return started;
    }
}
@end
