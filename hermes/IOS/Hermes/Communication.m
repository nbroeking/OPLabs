//
//  Communication.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/18/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Communication.h"

@interface Communication ()

@property(strong, atomic) NSInputStream* inputStream;
@property(strong, atomic) NSOutputStream* outputStream;

@end

@implementation Communication
@synthesize inputStream;
@synthesize outputStream;
@synthesize thread;

-(instancetype)init{
    
    if( self = [super init])
    {
        started = false;
        NSLog(@"Created Communication Object");
    }
    return self;
}

-(void)threadMain
{
    //Init Everything
    NSLog(@"Comm main");
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocketToHost(NULL, (CFStringRef)@"10.0.1.10", 54321, &readStream, &writeStream);
    
    inputStream = (NSInputStream*) CFBridgingRelease(readStream);
    outputStream = (NSOutputStream*) CFBridgingRelease(writeStream);
    
    [inputStream setDelegate:self];
    [outputStream setDelegate:self];
    
    //Schedule in run loop
    //[inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode: [[NSRunLoop currentRunLoop] currentMode]];
    
    [inputStream open];
    [outputStream open];
    
    started = true;
    NSLog(@"Communication Start Completed");
    
    NSLog(@"Starting Run Loop");
    
    [[NSRunLoop currentRunLoop] run];
    
    NSLog(@"Run Loop Stopped");
    
    [inputStream setDelegate:nil];
    [outputStream setDelegate:nil];

    [inputStream close];
    [inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream close];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    NSLog(@"Communication Thread Closed");
}
-(void)start{
    
    NSLog(@"Starting Threads");
    //Start the run loop
    thread = [[NSThread alloc] initWithTarget:self selector:@selector(threadMain) object:nil];
   
    [thread start];
}
-(void)stop{
    
    //Kill the sockets
    
    //Kill the run loop
    NSLog(@"Communication Stop Completed");
}
@end
