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
-(NSArray*) runDNSTest: (NSMutableArray*)domains;
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
    NSArray *times1 = [self runDNSTest:[settings invalidDomains]];
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
    NSArray *times2 = [self runDNSTest:[settings validDomains]];
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
    
    NSLog(@"Latency Result = %f", latencyResult);

        
    //Packet Loss is just 1 - times we have / times we should have
    if( ([[settings validDomains]count] == 0)&&([[settings invalidDomains] count] == 0))
    {
        NSLog(@"Error with packet loss");
        [results setValid:false];
    }
    [results setPacketloss:(1-(([times2 count] + [times1 count]) / ([[settings validDomains] count] + [[settings invalidDomains] count])))];
    
    NSLog(@"Packet loss = %f percent", [results packetloss]);

    [state setState:TESTINGTHROUGHPUT];
    //Run a throughput response
    
    throughputHandler = [[Throughput alloc] init:self withSettings:settings withResults:results];
    
    [throughputHandler runDownloadTest];
    
    throughputComplete = false;
    [self performSelectorInBackground:@selector(runUnderLoad) withObject:NULL];
}

//Should run on other thread
-(void) runUnderLoad{
    //Run a test packet latency test
    NSMutableArray *times = [[NSMutableArray alloc] init];
    
    NSTimeInterval startTime = [[NSDate date] timeIntervalSince1970];
    
    int tests = 0;
    
    while ( [[NSDate date] timeIntervalSince1970] - startTime < 10.0){
        NSArray *resTimes = [self runDNSTest:[settings validDomains]];
        
        [times addObjectsFromArray:resTimes];
        tests += 1;
    }
    
    double latencyResult = 0;
    for (NSNumber *x in times) {
        latencyResult += [x doubleValue];
    }
    
    if( [times count] ==0 ){
        [results setLatencyUnderLoad:0.0];
    }
    else {
        latencyResult /= [times count];
        [results setLatencyUnderLoad:latencyResult];
    }
    
    [results setPacketlossUnderLoad:1- ([times count] / tests*[[settings validDomains]count])];
    
    //TODO: Signal the other thread to continue
    
    NSLog(@"Latency Under Load Result = %f", latencyResult);
    NSLog(@"Packet Loss under Load = %f", [results packetloss]);
    
    [lock lock];
    throughputComplete = true;
    [lock signal];
    [lock unlock];

}

-(void)completedUpload{
    NSLog(@"Performance test completed a upload");
    [delegate testComplete:results :settings];

}
-(void)completedDownload{
    NSLog(@"Performance test completed a download");
    
    [lock lock];
    
    while (throughputComplete != true) {
        [lock wait];
    }
    [lock unlock];
    //TODO: RUn an upload test
    
    [throughputHandler runUploadTest];
}
-(void)sendResults{
    NSLog(@"Performance tester is done with the settings");
    [delegate testComplete:results :settings ];
}

-(NSArray *)runDNSTest:(NSMutableArray *)domains{
    
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
            }
        }
    }
    
    return resultTimes;
}

-(Boolean)sendDNSRequest:(NSString *)string withId:(NSInteger)identifier{
    Boolean returncode = true;

    socket = [[UDPSocket alloc] init:settings];
        
    //Construct the DNS packets
    NSData *sendData = [self getContent:string :identifier];

    timeStart = [[NSDate date] timeIntervalSince1970 ];
    returncode = [socket sendData:sendData to:[settings DNSServer]];

    return returncode;
    
}

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

@end
