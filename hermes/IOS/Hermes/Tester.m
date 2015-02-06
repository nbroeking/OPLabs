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
@synthesize timer;

-(instancetype)init
{
    if( self = [super init])
    {
        started = false;
        shouldRun = false;
        thread = nil;
        timer = nil;
    }
    return self;
}
//Main Thread Run Loop
-(void)threadMain
{
    NSLog(@"Tester Start");
    //Every time an event loop is kicked we exit our run loop. So we want to make sure we reenter it every time.
    //RunMode is a blocking call until a system event happens
    
    
    //Add a mach port to prevent the run loop from closing if there is no event
    //I know this is ugly but the documentation says there is no other way
    NSPort *port = [NSMachPort port];
    [[NSRunLoop currentRunLoop] addPort:port forMode:NSDefaultRunLoopMode];
    
    while(shouldRun)
    {
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        NSLog(@"Tester Run Loop Reset");
    }
    @synchronized(self){
        
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
            NSLog(@"Tester already started: try again");
            return;
        }
        shouldRun = true;
        started = true;
        
        NSLog(@"Starting Threads");
        //Start the run loop
        thread = [[NSThread alloc] initWithTarget:self selector:@selector(threadMain) object:nil];
        [thread setName:@"Tester"];
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
    }
}

//This method is used to add something to the run loops queue.
//This method purposly does nothing except wake up the run loop
//To give it a chance to exit
-(void)tearDownRunLoop
{
    NSLog(@"Trying to kill tester");
    //When this method exits the run loop will close
}

//Returns is running under a NSMutex
-(BOOL)isRunning
{
    @synchronized(self)
    {
        return started;
    }
}
@end
