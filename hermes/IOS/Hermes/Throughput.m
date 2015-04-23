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
@property (strong, nonatomic) TestResults *results;
@property (strong, nonatomic) TestSettings *settings;
@property (strong, nonatomic) PerformanceTester* delegate;
@property (strong, nonatomic) NSMutableData *data;
@property (assign, nonatomic) NSInteger downloaded;
@property (assign, nonatomic) NSTimeInterval start;
@property (strong, nonatomic) NSInputStream *inputStream;
@property (strong, nonatomic) NSOutputStream *outputStream;
@property (strong, nonatomic) NSTimer *timeoutTimer;
@property (assign, nonatomic) NSInteger uploaded;
@property (assign, nonatomic) Boolean uploadMode;
@property (strong, nonatomic) NSData* uploadData;

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
@synthesize uploadData;
@synthesize uploadMode;
@synthesize uploaded;

-(instancetype)init :(PerformanceTester*)parent withSettings:(TestSettings*)config withResults:(TestResults*)answer{
    self = [super init];
    if (self) {
        results = answer;
        settings = config;
        delegate = parent;
        start = 0.0;
        uploadMode = false;
        uploaded = 0;
        Byte* tmpBytes = malloc(1024*2024);
        memset(tmpBytes, 90, 1024*1024);
        
        uploadData = [[NSData alloc] initWithBytes:tmpBytes length:1024*1024];
        free(tmpBytes);
    }
    return self;
}
-(void) runDownloadTest{
    NSLog(@"Starting a download test");
    
    NSString *urlStr = [settings throughputServer];
    if (![urlStr isEqualToString:@""]) {
        NSURL *website = [NSURL URLWithString:urlStr];
        if (!website) {
            NSLog(@"%@ is not a valid URL", website);
            return;
        }
        
        CFReadStreamRef readStream;
        CFWriteStreamRef writeStream;
        //CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)@"10.0.1.2", (unsigned int)5432 , &readStream, &writeStream);
        CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)[settings throughputServer], (short)[settings port] , &readStream, &writeStream);
        
        inputStream = (__bridge_transfer NSInputStream *)readStream;
        outputStream = (__bridge_transfer NSOutputStream*)writeStream;
        
        
        [inputStream setDelegate:self];
        [outputStream setDelegate:self];
        
        [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        [inputStream open];
        [outputStream open];
        
        timeoutTimer = [NSTimer scheduledTimerWithTimeInterval:12.0
                                         target:self
                                       selector:@selector(timeoutDownload)
                                       userInfo:nil
                                        repeats:NO];
        
        //The connection is set up
    }
}

-(void) runUploadTest{
    NSLog(@"Starting Upload test");
    
    //If this is true then there hasn't been an issue with the socket so we can run upload test
    if( inputStream != NULL &&outputStream != NULL){
        //Write some bytes
        
        uploadMode = true;
        timeoutTimer = [NSTimer scheduledTimerWithTimeInterval:10.0
                                                        target:self
                                                      selector:@selector(uploadCompleteTimeout)
                                                      userInfo:nil
                                                       repeats:NO];
        start = [[NSDate date] timeIntervalSince1970];
        
        [outputStream write:[uploadData bytes] maxLength:[uploadData length]];
        self.uploaded += [uploadData length];
    }
    else{
        [self handleUpload:-1];
    }
}

-(void) timeoutDownload {
    NSLog(@"There was a timeout connecting to the throughput server");
    
    [self handleDownload:-1];
}
-(void) downloadCompleteTimeout {
    NSLog(@"Data stopped flowing");
    start = 0.0;
    [self handleDownload:1];
}
-(void) uploadCompleteTimeout {
    NSLog(@"Finished uploading data");
    
    [self handleUpload:1];
}
-(void) timeoutUpload {
    NSLog(@"There was a timeout uploading data");
}

-(void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
    switch (eventCode)
    {
        case NSStreamEventOpenCompleted:
            start = [[NSDate date] timeIntervalSince1970 ];
            downloaded = 0;
            [timeoutTimer invalidate];
            
            timeoutTimer = [NSTimer scheduledTimerWithTimeInterval:12.0
                                                            target:self
                                                          selector:@selector(downloadCompleteTimeout)
                                                          userInfo:nil
                                                           repeats:NO];
            break;
            
        case NSStreamEventHasBytesAvailable:
            if( uploadMode == false){
                if( aStream == inputStream)
                {
                    uint8_t buffer[1024];
                    int len;
                
                    len = (int)[inputStream read:buffer maxLength:sizeof(buffer)];
                    downloaded += len;
                
                    if( start != 0.0 && [[NSDate date] timeIntervalSince1970] - start >= 10.0){
                        [timeoutTimer invalidate];
                        uploadMode = true;
                        start = 0.0;
                        [self handleDownload:1];
                    }
                }
            }
            else{
                //Throw away excess data
                uint8_t buffer[1024];
                [inputStream read:buffer maxLength:sizeof(buffer)];
            }
            break;
            
        case NSStreamEventErrorOccurred:
            NSLog(@"Can not connect to the host");
            [self handleUpload:-1];
            break;
            
        case NSStreamEventEndEncountered:
            NSLog(@"End of stream encountered");
            [self handleUpload:-1];
            break;
            
        case NSStreamEventHasSpaceAvailable:
            
            if( uploadMode == true){
                [outputStream write:[uploadData bytes] maxLength:[uploadData length]];
                self.uploaded += [uploadData length];
            }
            break;
            
        default:
            NSLog(@"Unknown stream event occured");
            break;
    }
}

-(void) handleDownload :(int)status{
    
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

-(void) handleUpload :(int)status{
    
    if( status < 0)
    {
        [results setThroughputUpload:0.0];
        [results setValid:false];
    }
    else{
        double throughPut = (double)uploaded / ([[NSDate date] timeIntervalSince1970] - start);
        [results setThroughputUpload:throughPut];
    }
    [self shutdown];
    

    [delegate completedUpload];
}
-(void) shutdown{
    
    NSLog(@"We successfully shutdown the Throughput connection");
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
