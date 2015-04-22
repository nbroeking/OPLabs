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
@property (assign, nonatomic) NSTimeInterval end;
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
@synthesize start, end;
@synthesize inputStream ,outputStream;
@synthesize timeoutTimer;
@synthesize uploadData;
@synthesize uploadMode;
@synthesize uploaded;

//Once the object is initlize we are prepared to run a download test
-(instancetype)init :(PerformanceTester*)parent withSettings:(TestSettings*)config withResults:(TestResults*)answer{
    self = [super init];
    if (self) {
        results = answer;
        settings = config;
        delegate = parent;
        start = 0.0;
        uploadMode = false;
        uploaded = 0;
        downloaded = 0;
        Byte* tmpBytes = malloc(1024*2024);
        memset(tmpBytes, 90, 1024*1024);
        
        uploadData = [[NSData alloc] initWithBytes:tmpBytes length:1024*1024];
        free(tmpBytes);
    }
    return self;
}
//This should be called after the init method above
//It prepares the object to run a download test
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

//Once the download test has been run we can run an upload test
//NOTE: This method will not work properly if the download test has not been ran first
-(void) runUploadTest{
    NSLog(@"Starting Upload test");
    
    //If this is true then there hasn't been an issue with the socket so we can run upload test
    if( inputStream != NULL &&outputStream != NULL){
        //Write some bytes
        
        uploadMode = true;
        uploaded = 0.0;
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
//If there was a timeout during the download test
//This can happen if there is no throughput server or if there was a
//socket error
-(void) timeoutDownload {
    NSLog(@"There was a timeout connecting to the throughput server");
    
    [self handleDownload:-1];
}
//This happens once we have downloaded data for 10 seconds
-(void) downloadCompleteTimeout {
    NSLog(@"Data stopped flowing");
    start = 0.0;
    [self handleDownload:1];
}
//Completed after we have uploaded data for 10 seconds
-(void) uploadCompleteTimeout {
    NSLog(@"Finished uploading data");
    
    [self handleUpload:1];
}
//If there was an error with the upload this will get called
-(void) timeoutUpload {
    NSLog(@"There was a timeout uploading data");
}

//THroughput is a delgate of the NSStream class. These are all events that a stream can have
//We use this to know when it is ok to upload more data or to download more data
//We are also reported stream errors
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

                uint8_t buffer[1024];
                int len;
                
                len = (int)[inputStream read:buffer maxLength:sizeof(buffer)];
                downloaded += len;
                
                if( start != 0.0 && [[NSDate date] timeIntervalSince1970] - start >= 10.0){
                    end = [[NSDate date] timeIntervalSince1970];
                    [timeoutTimer invalidate];
                    uploadMode = true;
                    [self handleDownload:1];
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
               NSInteger x = [outputStream write:[uploadData bytes] maxLength:[uploadData length]];
                if( x < 0){
                    NSLog(@"Error writting bytes but continuing");
                }
                if( x == 0){
                    NSLog(@"Fixed length stream but continuing");
                }
                self.uploaded += x;
                end = [[NSDate date] timeIntervalSince1970];
            }
            break;
            
        default:
            NSLog(@"Unknown stream event occured");
            break;
    }
}

//Once the download has completed we handle it
//If it is called with a negative status we need to
//handle the download for an error
//If status is positive then we report our results
-(void) handleDownload :(int)status{
    
    if( status < 0)
    {
        [self shutdown];
        [results setThroughputDownload:0.0];
        [results setValid:false];
    }
    else{
        double downloadThroughPut = (double)downloaded / (end - start);
        start = 0.0;
        [results setThroughputDownload:downloadThroughPut];
    }
    [delegate completedDownload];
}

//Once the upload has completed we handle it
//If it is called with a negative status we need to
//handle the download for an error
//If status is positive then we report our results
-(void) handleUpload :(int)status{
    
    if( status < 0)
    {
        [results setThroughputUpload:0.0];
        [results setValid:false];
    }
    else{
        double throughPut = (double)uploaded / (end - start);
        start = 0.0;
        [results setThroughputUpload:throughPut];
    }
    [self shutdown];
    

    [delegate completedUpload];
}

//This shutsdown the stream because we need to clean up our resources
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
    NSLog(@"We shutdown the Throughput connection");
}
@end
