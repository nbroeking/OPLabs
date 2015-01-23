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

-(void) setUpSockets;
-(void) tearDownSockets;
-(void) tearDownRunLoop;

@end

@implementation Communication
@synthesize inputStream;
@synthesize outputStream;
@synthesize thread;

//Initlizes the Communication subsystem to 0
-(instancetype)init{
    
    if( self = [super init])
    {
        started = false;
        shouldRun = false;
        inputStream = nil;
        outputStream = nil;
        NSLog(@"Created Communication Object");
    }
    return self;
}
//Call back delegate that gets called when any stream has an event occure
- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)event {
    NSLog(@"Stream Event triggered.");
    
    switch(event) {
        case NSStreamEventHasSpaceAvailable: {
            if(stream == outputStream) {
                NSLog(@"outputStream is ready to write.");
            }
            break;
        }
        case NSStreamEventHasBytesAvailable: {
            if(stream == inputStream) {
                NSLog(@"inputStream is ready.");
                
                uint8_t buf[1024];
                unsigned int len = 0;
                
                //We need to account for tcp messages not all being sent at once
                //Protocol should have first 4 bytes send the number of bytes expected
                len = [inputStream read:buf maxLength:1024];
                
                if(len > 0) {
                    NSMutableData* data=[[NSMutableData alloc] initWithLength:0];
                    
                    [data appendBytes: (const void *)buf length:len];
                    
                    NSString *s = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
                    
                    NSLog(@"Bytes Received: %@", s);
                }
            } 
            break;
        }
        case NSStreamEventOpenCompleted: {
            NSLog(@"NSStreamEventOpenCompleted");
            break;
        }
        case NSStreamEventEndEncountered: {
            NSLog(@"NSStreamEventEndEncouterd");
            break;
        }
        case NSStreamEventErrorOccurred: {
            NSLog(@"Stream Error Occured");
            break;
        }
        case NSStreamEventNone: {
            NSLog(@"No Sream Event");
            break;
        }
        default: {
            NSLog(@"Stream is sending an Event: %i", event);
            
            break;
        }
    }
}
//Main Thread Run Loop
-(void)threadMain
{
    NSLog(@"Communication Start");
    //Set up json parser
    
    [self setUpSockets];
    
    while(shouldRun)
    {
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        NSLog(@"Communication Run Loop Reset");
    }
    @synchronized(self){
        [self tearDownSockets];
    
        //Reseting member variables
        thread = nil;
        started = false;
        shouldRun = false;
    }
    NSLog(@"Communication Thread Closed");
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
    }
}
//Set up the TCP Connections
-(void)setUpSockets
{
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocketToHost(NULL, (CFStringRef)@"10.0.1.10", 54321, &readStream, &writeStream);
    
    inputStream = (NSInputStream*) CFBridgingRelease(readStream);
    outputStream = (NSOutputStream*) CFBridgingRelease(writeStream);
    
    [inputStream setDelegate:self];
    [outputStream setDelegate:self];
    
    //Schedule in run loop
    [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
    
    [inputStream open];
    [outputStream open];
}
//Tear down the TCP connections
-(void)tearDownSockets
{
    [inputStream setDelegate:nil];
    [outputStream setDelegate:nil];
    
    [inputStream close];
    [inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream close];
    
    //Point to nil for ARC
    inputStream = nil;
    outputStream = nil;
}
//Tear down the run loop
-(void)tearDownRunLoop
{
    NSLog(@"Trying to kill communication loop");
    //When this method exits the run loop will close
}
//Returns if the sub system is running
-(BOOL)isRunning
{
    //Blocked with NSMutex
    @synchronized(self)
    {
        return started;
    }
}
@end
