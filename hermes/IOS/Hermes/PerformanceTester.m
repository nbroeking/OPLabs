//
//  PerformanceTester.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "PerformanceTester.h"
#import "TestSettings.h"
#import "TestState.h"
#import "UDPSocket.h"
#import "Tester.h"
#import "Throughput.h"

@interface PerformanceTester()

@property (strong, atomic) TestSettings* settings;
@property (assign, atomic) NSTimeInterval timeStart;
@property (assign, atomic) NSTimeInterval timeEnd;
@property (strong, atomic) UDPSocket * socket;
@property (strong, atomic) NSCondition *lock;
@property (assign, atomic) Boolean throughputComplete;
@property (weak, atomic) Tester *delegate;
@property (strong, atomic) TestResults *results;
@property (strong, nonatomic) Throughput *throughputHandler;

-(Boolean) sendDNSRequest :(NSString*)string withId:(NSInteger)identifier;
-(NSArray *)runDNSTest:(NSMutableArray *)domains needsDelay:(Boolean)delay;
-(Boolean) getDNSResponse:(NSInteger)identifier;

-(NSData*) getContent:(NSString*) name :(NSInteger) identifier;

@end

@implementation PerformanceTester
@synthesize settings, timeEnd, timeStart, socket;
@synthesize lock, throughputComplete;
@synthesize delegate;
@synthesize results;
@synthesize throughputHandler;

-(instancetype)init:(TestSettings *)settingst
{
    self = [super init];
    
    if(self)
    {
        self.settings = settingst;
    }
    return self;
}

//This method will run the test suit in order
// DNS TEST
// Latency TEST
// Packet Loss
// Then will spawn a thread and run
// Throughput and load tests at the same time
-(void)runTests:(Tester *)owner{
    delegate = owner;
    
    results = [[TestResults alloc] init];
    [results setMobileIdentifier:(int)[settings mobileResultID]];
    [results setSetID:(int)[settings setId]];
    [results setValid:YES];
    TestState *state = [TestState getStateMachine];
    
    [state setState:TESTINGDNS];
    [results setRouterIdentifier:[settings routerResultID]];
    
    //Run a DNS Response Test
    NSArray *times1 = [self runDNSTest:[settings invalidDomains] needsDelay:false];
    double dnsResult = 0;
    for (NSNumber *x in times1) {
        dnsResult += [x doubleValue];
    }

    if( [times1 count] ==0 ){
        
        [results setDns:0.0];
    }
    else{
        dnsResult /= [times1 count];
        [results setDns:dnsResult];
    }
    NSLog(@"DNS Result = %f", dnsResult);
    
    [state setState:TESTINGLATENCY];
    

    //Run a test packet latency test
    NSArray *times2 = [self runDNSTest:[settings validDomains] needsDelay:false];
    double latencyResult = 0;
    for (NSNumber *x in times2) {
        latencyResult += [x doubleValue];
    }
    
    if( [times2 count] ==0 ){
        [results setLatency:0.0];
    }
    else {
        latencyResult /= [times2 count];
        [results setLatency:latencyResult];
    }
    
    [results setLatencySD:[self standardDev:times2]];
    NSLog(@"Latency Result = %f", latencyResult);

        
    //Packet Loss is just 1 - times we have / times we should have
    if( ([[settings validDomains]count] == 0)&&([[settings invalidDomains] count] == 0))
    {
        NSLog(@"Error with packet loss");
        [results setValid:false];
    }
    [results setPacketloss:(1.0-(((double)[times2 count] + (double)[times1 count]) / ((double)[[settings validDomains] count] + (double)[[settings invalidDomains] count])))];
    
    NSLog(@"Packet loss = %f percent", [results packetloss]);

    [state setState:TESTINGTHROUGHPUT];
    //Run a throughput response
    
    throughputHandler = [[Throughput alloc] init:self withSettings:settings withResults:results];
    
    [throughputHandler runDownloadTest];
    
    throughputComplete = false;
    [self performSelectorInBackground:@selector(runUnderLoad) withObject:NULL];
}

//Should run on other thread
//This method will run a load test calculating latency and packet loss
//While under load
-(void) runUnderLoad{
    //Run a test packet latency test
    NSMutableArray *times = [[NSMutableArray alloc] init];
    
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    
    int tests = 0;
    
    while ( [[NSDate date] timeIntervalSince1970] - startTime < 10.0){
        NSArray *resTimes = [self runDNSTest:[settings validDomains] needsDelay:true];
        
        [times addObjectsFromArray:resTimes];
        tests += 1;
    }
    
    if( [times count] ==0 ){
        [results setLatencyUnderLoad:[[NSArray alloc] init]];
    }
    else {
        [results setLatencyUnderLoad:times.copy];
    }
    
    double top = [times count];
    double bottom = tests*[[settings validDomains] count];
    double packetLossUnderLoad = 1.0 -(top /bottom);
    
    [results setPacketlossUnderLoad:packetLossUnderLoad];
    
    //Signal the Tester thread that we are done
    [lock lock];
    throughputComplete = true;
    [lock signal];
    [lock unlock];

}
//Called by the throughput object when we have completed a upload test
-(void)completedUpload{
    NSLog(@"Performance test completed a upload");
    [delegate testComplete:results :settings];

}
//Called by the throughput object when we have completed a download test
-(void)completedDownload{
    NSLog(@"Performance test completed a download");
    
    [lock lock];
    
    while (throughputComplete != true) {
        [lock wait];
    }
    [lock unlock];
    
    [throughputHandler runUploadTest];
}

//Report the results to whover created us aka. the tester object
-(void)sendResults{
    [delegate testComplete:results :settings ];
}

//This will run a dns test by sending x number dns requests and
//Testing how long it takes for them to return
-(NSArray *)runDNSTest:(NSMutableArray *)domains needsDelay:(Boolean)delay{
    
    NSMutableArray *resultTimes = [[NSMutableArray alloc] init];
    
    //Run a dns test
    short id = 0;
    for( NSString *x in domains) {
        id += 1;
        if( [self sendDNSRequest:x withId:id]){
            if( [self getDNSResponse:id])
            {
                long elapsedTime = timeEnd*1000 - timeStart*1000;
                [resultTimes addObject:[[NSNumber alloc] initWithInteger: elapsedTime]];
                if( delay){
                    [NSThread sleepForTimeInterval:.2f];
                }
            }
        }
    }
    
    return resultTimes;
}

//Sends a UDP request
// Will set the start time for calculating the rtt
-(Boolean)sendDNSRequest:(NSString *)string withId:(NSInteger)identifier{
    Boolean returncode = true;

    socket = [[UDPSocket alloc] init:settings];
        
    //Construct the DNS packets
    NSData *sendData = [self getContent:string :identifier];

    timeStart = [[NSDate date] timeIntervalSince1970 ];
    returncode = [socket sendData:sendData to:[settings DNSServer]];

    return returncode;
    
}

//Receives the dns response
//This will set the return time for calculating the rtt
-(Boolean)getDNSResponse:(NSInteger)identifier{
    
    short recvId = -1;
    @try {
        Byte *data = NULL;
        while(recvId != identifier) {
            NSData *buffer = [socket recvData:[settings timeout]];
            
            if( [buffer length] < 4){
                return false;
            }
            data = (Byte*)[buffer bytes];
            recvId = ((short)(((int)data[0] << 8) | ((int)(data[1]))));
        }
        //Check if its valid
        short valid = (short)(data[3]);
        valid = (short)((int)valid & 0x000F);
            
        if(valid != 0 && valid != 3)
        {
            NSLog(@"DNS Error: %hd", valid);
            return false;
        }
      
        timeEnd = [[NSDate date] timeIntervalSince1970 ];
    }
    @catch (NSException *exception) {
        NSLog(@"Error getting DNS");
    }
    @finally {
        [socket shutdown];
    }

   
    return true;
}
//We hand craft dns requests using this method.
//We conform to the DNS standard
-(NSData *)getContent:(NSString *)name :(NSInteger)identifier{
    NSArray *strings = [name componentsSeparatedByString:@"."];
    
    int size =16 + (int)[name length] + 2;
    Byte* bytes = malloc(size);
    
    
    //Set the id
    bytes[0] = (Byte)((identifier>>8)&0xFF);
    bytes[1] = (Byte)(identifier&0xFF);
    
    //Set the flags
    bytes[2] = 0x01;
    bytes[3] = bytes[4]= 0x00;
    bytes[5] = 0x01;
    
    for( int i = 0; i < 6; i++)
    {
        bytes[6+i] = 0x00;
    }
    
    //Add the sections
    int index = 12;
    
    for( NSString *section in strings)
    {
        bytes[index] = (Byte)[section length];
        index++;
        NSData *tmp = [section dataUsingEncoding:NSASCIIStringEncoding];
        
        for(int i = 0; i < [tmp length]; i++)
        {
            bytes[index + i] = (Byte)(((Byte*)[tmp bytes])[i]);
        }
        index+= [section length];
    }
    //Add another 0 to mark the end of the section
    
    bytes[index] = 0x00;
    index++;
    
    //Set trailer
    
    bytes[index] = 0x00;
    bytes[index+1] = 0x01;
    bytes[index+2] = 0x00;
    bytes[index+3] = 0x01;
    
    NSData *data = [[NSData alloc] initWithBytes:bytes length: size];
    
    free(bytes);
    return data;
}
// This calculates the standard deviation for a NSArray
-(double) standardDev:(NSArray*) array{
    if( array == NULL){
        return 0.0;
    }
    
    //Calculate the mean
    double mean = 0.0;
    
    for( NSNumber *x in array){
        mean += [x doubleValue];
    }
    mean = mean / [array count];
    
    //Calculate the dev
    
    double dev = 0.0;
    
    for (NSNumber *x in array){
        dev += pow(([x doubleValue] -mean), 2);
    }
    dev = dev / ([array count] -1);
    
    dev = sqrt(dev);
    return dev;
}

@end
