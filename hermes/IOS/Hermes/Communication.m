//
//  Communication.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/18/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Communication.h"

@interface Communication ()

@property(strong, atomic) NSThread *thread;
@property(strong, nonatomic) NSURL *loginURL;
@property(strong, nonatomic) NSMutableData *responseData;
@property(strong, nonatomic) NSURLConnection* webConnection;


-(void) loginToServer: (id) sender;

@end

@implementation Communication
@synthesize thread;
@synthesize loginURL;
@synthesize responseData;
@synthesize webConnection;

//Initlizes the Communication subsystem to 0
-(instancetype)init{
    
    if( self = [super init])
    {
        started = false;
        shouldRun = false;
        
        //Constants
        loginURL = [NSURL URLWithString:@"https://128.138.202.143/api/auth/login"];
    }
    return self;
}
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

-(void)login:(id)sender
{
    [self performSelector:@selector(loginToServer:) onThread:thread withObject:sender waitUntilDone:NO];
}
-(void)loginToServer:(id)sender
{
    // Create the request.
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:loginURL];
    
    // Specify that it will be a POST request
    request.HTTPMethod = @"POST";
    
    // Set Header Values
    [request setValue:@"nbroeking@me.com" forHTTPHeaderField:@"email"];
    [request setValue:@"password1" forHTTPHeaderField:@"password"];
    
    // Convert your data and set your request's HTTPBody property
    NSString *stringData = @"";
    NSData *requestBodyData = [stringData dataUsingEncoding:NSUTF8StringEncoding];
    request.HTTPBody = requestBodyData;
    
    webConnection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
}

#pragma mark NSURLConnection Delegate Methods
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    // A response has been received, this is where we initialize the instance var you created
    // so that we can append data to it in the didReceiveData method
    // Furthermore, this method is called each time there is a redirect so reinitializing it
    // also serves to clear it
    responseData = [[NSMutableData alloc] init];
    NSLog(@"Comm: Connection did Receive Response");
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    // Append the new data to the instance variable you declared
    NSLog(@"Comm: Did Receive Data");
    [responseData appendData:data];
}

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection
                  willCacheResponse:(NSCachedURLResponse*)cachedResponse {
    // Return nil to indicate not necessary to store a cached response for this connection
    return nil;
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    // The request is complete and data has been received
    // You can parse the stuff in your instance variable now
    
    NSLog(@"Comm: Connection did complete");
    NSLog(@"Data: %@", [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding]);
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    // The request has failed for some reason!
    // Check the error var
    NSLog(@"Comm: Connection failed with error %@", error);
}

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
    return [protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust];
}

- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge {
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust])
    {
        [challenge.sender useCredential:[NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust] forAuthenticationChallenge:challenge];
    }
    [challenge.sender continueWithoutCredentialForAuthenticationChallenge:challenge];
}

@end
