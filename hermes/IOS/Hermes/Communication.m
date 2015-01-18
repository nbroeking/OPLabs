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
    
}
-(void)start{
    
    //Start the run loop
    
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    
    CFStreamCreatePairWithSocketToHost(NULL, (CFStringRef)@"10.0.1.10", 54321, &readStream, &writeStream);
    
    inputStream = (NSInputStream*) CFBridgingRelease(readStream);
    outputStream = (NSOutputStream*) CFBridgingRelease(writeStream);
    
    //Schedule in run loop
    
    [inputStream open];
    [outputStream open];
    
    started = true;
    NSLog(@"Communication Start Completed");
}
-(void)stop{
    
    //Kill the sockets
    
    //Kill the run loop
    NSLog(@"Communication Stop Completed");
}
@end
