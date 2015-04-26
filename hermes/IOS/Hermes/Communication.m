//
//  Communication.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/18/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Communication.h"
#import "HomeViewController.h"
#import "HermesHttpPost.h"
#import "TestSettings.h"
#import "ResponseHandler.h"
#import "TestResults.h"
#import "TestState.h"

NSString * const LoginURL = @"/api/auth/login";
NSString * const StartTestURL = @"/api/test_set/create";
NSString * const ReportResultURL = @"/api/test_result/%ld/edit";

@interface Communication ()

@property(strong, nonatomic) NSURLConnection* webConnection;
@property(strong, nonatomic) HermesHttpPost *post;

-(void) loginToServer: (id) sender;
-(void) sendRequest:(NSMutableURLRequest*) request :(NSString*)type needsResponse:(BOOL)needsResponse;
@end

@implementation Communication
@synthesize thread;
@synthesize webConnection;
@synthesize post;

//Get The singleton
+(Communication*) getComm
{
    static Communication *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[Communication alloc] init];
    });
    return sharedInstance;
}

//Initlizes the Communication subsystem to 0
-(instancetype)init{
    
    if( self = [super init])
    {
        started = false;
        shouldRun = false;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(reportTest:) name:@"TestComplete" object:nil];
    }
    return self;
}

/****************************************
 * These methods below are the methods that
 * the other subsystems should use to request services
 *
 *****************************************/
//Requests

-(void)login:(id)sender
{
    if( !sender)
    {
        NSLog(@"SUPER BIG ERROR: Nic made a huge mistake with his logic and should go fix it now");
    }
    [self performSelector:@selector(loginToServer:) onThread:thread withObject:sender waitUntilDone:NO];
}

-(void)startTest{
    
    [self performSelector:@selector(requestTestFromServer) onThread:thread withObject:NULL waitUntilDone:NO];
    
}
-(void)reportTest:(NSNotification*)notification{
    TestResults *results = (TestResults*)[notification object];
    [self performSelector:@selector(reportTestHelper:) onThread:thread withObject:results waitUntilDone:NO];
    
    NSLog(@"Comm: Preparing to Report Test Complete");
}

/*********************************************
 *These are the helper methods that get run on the
 *comm thread;
 ********************************************/
-(void) reportTestHelper:(TestResults*)results{
    
    NSLog(@"Reporting a test to the server");
    NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], [NSString stringWithFormat: ReportResultURL, (long)[results mobileIdentifier]]]]];
    
    request.HTTPMethod = @"POST";
    
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
    
    NSString *postString = [NSString stringWithFormat:@"user_token=%@&%@", [[SessionData getData] sessionIdEncoded], [results getPost]];
    
    NSData *data = [postString dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    
    [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:data];
    
    [self sendRequest:request :@"ReportResults" needsResponse:NO];
  
}
-(void) requestTestFromServer{
    
    NSLog(@"Request a test from the server");
    NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartTestURL]]];
    
    request.HTTPMethod = @"POST";
    
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
    
    NSString *postString = [NSString stringWithFormat:@"user_token=%@", [[SessionData getData] sessionIdEncoded]];
    
    NSData *data = [postString dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    
    [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:data];
    
    [self sendRequest:request :@"GetSetId" needsResponse:YES];
    
}

//The method that gets called on the comm thread
-(void)loginToServer:(id)sender
{
    NSLog(@"Login to server");
    // Create the request.
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString: [NSString stringWithFormat:@"%@%@", [[SessionData getData] hostname], LoginURL]]];
    
    // Specify that it will be a POST request
    request.HTTPMethod = @"POST";
    
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];

    NSString *postString = [NSString stringWithFormat:@"password=%@&email=%@", [[SessionData getData] password], [[SessionData getData] email]];
    NSData *data = [postString dataUsingEncoding:NSASCIIStringEncoding];
    [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:data];
    
    [self sendRequest:request :@"Login" needsResponse:false];
    
}

-(void) sendRequest:(NSMutableURLRequest*) request :(NSString*)type needsResponse:(BOOL)needsResponse{
    //We are creating a new Hermes HTtpPost and will receive the answer in report
    post = [[HermesHttpPost alloc] init];

    [post post:request :type :NULL];
}

/****************************************
* These methods below contain the controll
* methods for the threads
*
*****************************************/
//Main Thread Run Loop
-(void)threadMain
{
    //Give the runloop something to watch to prevent closing
    NSPort *port = [NSMachPort port];
    [[NSRunLoop currentRunLoop] addPort:port forMode:NSDefaultRunLoopMode];
    
    //Set up json parser
    while(shouldRun)
    {
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
    @synchronized(self){
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
-(void)forceStop{
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

//Kill the communication thread cleanly
-(void)stop{
    @synchronized(self){
        if(!started)
        {
            NSLog(@"Thread is already stopped");
            return;
        }
        TestState *stateMachine = [TestState getStateMachine];
        if( [stateMachine getState] != IDLE){
            NSLog(@"We cant shut down communication because we are running a test");
            return;
        }
        shouldRun = false;
        [self performSelector:@selector(tearDownRunLoop) onThread:thread withObject:nil waitUntilDone:false];
    }
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
