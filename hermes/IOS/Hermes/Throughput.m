//
//  Throughput.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/16/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Throughput.h"
#import "TestResults.h"
#import "TestSettings.h"
#import "PerformanceTester.h"

@interface Throughput()
@property (weak, nonatomic) TestResults *results;
@property (weak, nonatomic) TestSettings *settings;
@property (weak, nonatomic) PerformanceTester* delegate;
@property (strong, nonatomic) NSMutableData *data;
@property (assign, nonatomic) NSInteger downloaded;
@property (assign, nonatomic) NSTimeInterval start;
@property (strong, nonatomic) NSInputStream *inputStream;
@property (strong, nonatomic) NSOutputStream *outputStream;
@property (strong, nonatomic) NSTimer *timeoutTimer;
@end

@implementation Throughput
@synthesize settings;
@synthesize results;
@synthesize delegate;
@synthesize data;
@synthesize downloaded;
@synthesize start;
@synthesize inputStream ,outputStream;
@synthesize timeoutTimer;


-(instancetype)init :(PerformanceTester*)parent withSettings:(TestSettings*)config withResults:(TestResults*)answer{
    self = [super init];
    if (self) {
        results = answer;
        settings = config;
        delegate = parent;
        start = 0.0;
    }
    return self;
}
-(void) runDownloadTest{
    NSString *urlStr = [settings throughputServer];
    if (![urlStr isEqualToString:@""]) {
        NSURL *website = [NSURL URLWithString:urlStr];
        if (!website) {
            NSLog(@"%@ is not a valid URL", website);
            return;
        }
        
        CFReadStreamRef readStream;
        CFWriteStreamRef writeStream;
        //CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)[website host], (unsigned int)[settings port] , &readStream, &writeStream);
        CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)@"10.0.1.2", 4321 , &readStream, &writeStream);
        
        inputStream = (__bridge_transfer NSInputStream *)readStream;
        outputStream = (__bridge_transfer NSOutputStream*)writeStream;
        
        
        [inputStream setDelegate:self];
        [outputStream setDelegate:self];
        
        [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        [inputStream open];
        [outputStream open];
        
        timeoutTimer = [NSTimer scheduledTimerWithTimeInterval:5.0
                                         target:self
                                       selector:@selector(timeout)
                                       userInfo:nil
                                        repeats:NO];
        
        //The connection is set up
    }
}

-(void) timeout {
    NSLog(@"There was a timeout");
    
    [self handle:-1];
}
-(void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
    switch (eventCode)
    {
        case NSStreamEventOpenCompleted:
            NSLog(@"Stream Opened");
            start = [[NSDate date] timeIntervalSince1970 ];
            downloaded = 0;
            [timeoutTimer invalidate];
            break;
            
        case NSStreamEventHasBytesAvailable:
            if( aStream == inputStream)
            {
                uint8_t buffer[1024];
                int len;
                
                while([inputStream hasBytesAvailable])
                {
                    len = (int)[inputStream read:buffer maxLength:sizeof(buffer)];
                    downloaded += len;
                }
                
                if( start != 0.0 && [[NSDate date] timeIntervalSince1970] - start >= 10){
                    start = 0.0;
                    [self handle:1];
                }
            }
            break;
            
        case NSStreamEventErrorOccurred:
            NSLog(@"Can not connect to the host");
            [self handle:-1];
            break;
            
        case NSStreamEventEndEncountered:
            NSLog(@"End of stream encountered");
            [self handle:-1];
            break;
            
        default:
            NSLog(@"Unknown stream event");
            break;
    }
}
-(void) handle :(int)status{
    NSLog(@"Completed with stuff");
    
    if( status < 0)
    {
        [self shutdown];
        [results setThroughputDownload:0.0];
        [results setValid:false];
    }
    else{
        double downloadThroughPut = (double)downloaded / ([[NSDate date] timeIntervalSince1970] - start);
        [results setThroughputDownload:downloadThroughPut];
    }

    [delegate completedDownload];
}

-(void) shutdown{
    
    if( inputStream){
        [inputStream close];
        [inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [inputStream setDelegate:NULL];
        inputStream = nil;
    }
    if(outputStream){
        [outputStream close];
        [outputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [outputStream setDelegate:NULL];
        outputStream = nil;
    }
}
@end
