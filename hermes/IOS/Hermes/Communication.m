//
//  Communication.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/18/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "Communication.h"
#import "CommunicationDelegate.h"
#import "HomeViewController.h"

@interface Communication ()

@property(strong, atomic) NSThread *thread;
@property(strong, nonatomic) NSURL *loginURL;
@property(strong, nonatomic) NSMutableData *responseData;
@property(strong, nonatomic) NSURLConnection* webConnection;
@property(weak, nonatomic)  HomeViewController* sender;

-(void) loginToServer: (id) sender;

@end

@implementation Communication
@synthesize thread;
@synthesize loginURL;
@synthesize responseData;
@synthesize webConnection;
@synthesize sender;

//Initlizes the Communication subsystem to 0
-(instancetype)init{
    
    if( self = [super init])
    {
        started = false;
        shouldRun = false;
        
        //Constants
        loginURL = [NSURL URLWithString: [NSString stringWithFormat:@"%@%@", [[SessionData getData] hostname], @"/api/auth/login"]];
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

-(void)login:(id)sendert
{
    if( self.sender)
    {
        NSLog(@"SUPER BIG ERROR: Nic made a huge mistake with his logic and should go fix it now");
    }
    self.sender = sendert;
    loginURL = [NSURL URLWithString: [NSString stringWithFormat:@"%@%@", [[SessionData getData] hostname], @"/api/auth/login"]];
    [self performSelector:@selector(loginToServer:) onThread:thread withObject:sender waitUntilDone:NO];
}
-(void)loginToServer:(id)sender
{
    // Create the request.
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:loginURL];
    
    // Specify that it will be a POST request
    request.HTTPMethod = @"POST";
    
    // Set Header Values
    [request setValue: [[SessionData getData] email] forHTTPHeaderField:@"email"];
    [request setValue:[[SessionData getData] password] forHTTPHeaderField:@"password"];
    
    // Convert your data and set your request's HTTPBody property
    NSString *stringData = @"";
    NSData *requestBodyData = [stringData dataUsingEncoding:NSUTF8StringEncoding];
    request.HTTPBody = requestBodyData;
    
    webConnection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
}

#pragma mark NSURLConnection Delegate Methods
//TODO: Not sure if what would happen if we tried to login btwice before we got a response

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

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    // The request is complete and data has been received
    // You can parse the stuff in your instance variable now
    
    NSLog(@"Comm: Connection did complete");
    NSLog(@"Data: %@", [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding]);
    
    //Parse JSON
    NSDictionary *json = [NSJSONSerialization JSONObjectWithData:responseData options:kNilOptions error:nil];
    
    NSString *token = [[NSString alloc] initWithString:[json objectForKey:@"status"]];
    
    if( ![token  isEqual: @"failure"])
    {
        [[SessionData getData] setSessionId: token];
        NSLog(@"Token: %@", [[SessionData getData] sessionId]);
    }
    else
    {
        [[SessionData getData] setSessionId:nil];
    }
    [sender performSelectorOnMainThread:@selector(notifyLogin) withObject: nil waitUntilDone:NO];
    sender = nil;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    // The request has failed for some reason!
    // Check the error var
    NSLog(@"Comm: Connection failed with error %@", error);
}

//Overloading the Authentication challenge to accept self signed certs
- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge {
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust])
    {
        [challenge.sender useCredential:[NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust] forAuthenticationChallenge:challenge];
    }
    [challenge.sender continueWithoutCredentialForAuthenticationChallenge:challenge];
}

//Used to see if we can AuthenticateAgainst Protection Space
- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace {
    return [protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust];
}
//Used to tell the connection that we dont want to cache the response
- (NSCachedURLResponse *)connection:(NSURLConnection *)connection
                  willCacheResponse:(NSCachedURLResponse*)cachedResponse {
    // Return nil to indicate not necessary to store a cached response for this connection
    return nil;
}
@end
