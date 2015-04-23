//
//  HermesHttpPost.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "HermesHttpPost.h"
#import "ResponseHandler.h"
@interface HermesHttpPost()

@property (strong, nonatomic) NSMutableData *responseData;
@property (weak, nonatomic) Communication *delegate;
@property (strong, nonatomic) NSString *type;
@property (strong, nonatomic) NSMutableArray *requests;
@property (strong, nonatomic) NSMutableDictionary *answer;
@property (strong, nonatomic) ResponseHandler *handler;
@end

@implementation HermesHttpPost
@synthesize responseData;
@synthesize delegate;
@synthesize type;
@synthesize requests;
@synthesize answer;
@synthesize handler;

//Prepare to collect data
-(instancetype)init {
    if( self = [super init])
    {
        delegate = NULL;
        answer = [[NSMutableDictionary alloc] init];
        handler = NULL;
    }
    return self;
}
//Initlizes the object with the communicatin delegate so
//we know where to report to
-(instancetype) initWithComm: (Communication*) comm{
    if( self = [super init])
    {
        delegate = comm;
        answer = [[NSMutableDictionary alloc] init];
        handler = NULL;
    }
    return self;
}

//Sends a post to the server and set our handler to handlert if there is one
-(void) post:(NSMutableURLRequest*)request :(NSString*)postType :(ResponseHandler*)handlert
{
    if( handlert != NULL){
        self.handler  = handlert;
    }
    [request setTimeoutInterval:6];
    type = [[NSString alloc] initWithString:postType];
    [NSURLConnection connectionWithRequest:request delegate:self];
}
//Sends a post to the server and will create a new handler when the return
-(void) posts:(NSMutableArray*)requestst : (NSString*)postType
{
    self.requests = requestst;
    type = postType;
    
    NSMutableURLRequest *request = [self.requests objectAtIndex:0];
    [self.requests removeObjectAtIndex:0];
    [NSURLConnection connectionWithRequest:request delegate:self];
}
#pragma mark NSURLConnection Delegate Methods
//When the connection is first established we need to create a place for the data to go
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    responseData = [[NSMutableData alloc] init];
}
//When we receive data we append it to the buffer we created above
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    // Append the new data to the instance variable you declared
    [responseData appendData:data];
}

//The request is complete so we handle the json object in the response handler
- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    // The request is complete and data has been received
    NSMutableDictionary *json = NULL;
    //Parse JSON
    @try {
        json = [NSJSONSerialization JSONObjectWithData:responseData options:kNilOptions error:nil];
        json = [[NSMutableDictionary alloc] initWithDictionary:json copyItems:true];
        
      }
    @catch (NSException* e)
    {
        NSLog(@"Caught Exception %@ This is normally caused by a bad domain.", e);
        json = [[NSMutableDictionary alloc] init];
        [json setValue:@"DOMAIN" forKey:@"status"];
    }
    
    @try {
        [json setValue:type forKey:@"POST_TYPE"];
    }
    @catch (NSException *e) {
        NSLog(@"Caught Exception %@", e);
    }
    
    [self reportData: json];
}

//If there was an error we report it
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    // The request has failed for some reason!
    // Check the error var
    NSLog(@"Comm: Connection failed with error %@\n\n", error);
    
    
    NSMutableDictionary *json = [[NSMutableDictionary alloc] init];
    [json setValue:@"ERROR" forKey:@"status"];
    [json setValue:type forKey:@"POST_TYPE"];
    [self reportData:json];
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

//Need to add a type to our dictionary
-(void) reportData: (NSMutableDictionary*)json
{
    if( self.handler == NULL){
        handler = [[ResponseHandler alloc]init];
    }
    [handler handle:json from: self];
}

@end
