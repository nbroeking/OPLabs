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

-(instancetype)init {
    if( self = [super init])
    {
        delegate = NULL;
        answer = [[NSMutableDictionary alloc] init];
        handler = NULL;
    }
    return self;
}
-(instancetype) initWithComm: (Communication*) comm{
    if( self = [super init])
    {
        delegate = comm;
        answer = [[NSMutableDictionary alloc] init];
        handler = NULL;
    }
    return self;
}
-(instancetype) init: (NSObject*)sender
{
    if( self = [super init])
    {
        handler = NULL;
        delegate = (Communication*)sender;
        answer = [[NSMutableDictionary alloc] init];
    }
    return self;
}
-(void) post:(NSMutableURLRequest*)request :(NSString*)postType :(ResponseHandler*)handlert
{
    if( handlert != NULL){
        NSLog(@"Posted with a response");
        self.handler  = handlert;
    }
    [request setTimeoutInterval:6];
    type = [[NSString alloc] initWithString:postType];
    [NSURLConnection connectionWithRequest:request delegate:self];
}

-(void) posts:(NSMutableArray*)requestst : (NSString*)postType
{
    self.requests = requestst;
    type = postType;
    
    NSMutableURLRequest *request = [self.requests objectAtIndex:0];
    [self.requests removeObjectAtIndex:0];
    [NSURLConnection connectionWithRequest:request delegate:self];
}
#pragma mark NSURLConnection Delegate Methods

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    responseData = [[NSMutableData alloc] init];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    // Append the new data to the instance variable you declared
    NSLog(@"Did Receive Data");
    
    [responseData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    // The request is complete and data has been received
    // You can parse the stuff in your instance variable now

    NSLog(@"Comm: Connection did complete");
    NSLog(@"Data: %@", [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding]);
    
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
    //We need to intercept that request Results tag so if we need to request again we can
    if( [[json objectForKey:@"POST_TYPE"]  isEqual:@"RouterResults"]){
        if( ([[json objectForKey:@"status"] isEqual:@"success"])&& !([[json objectForKey:@"state"] isEqual:@"finished"])){
            //Request again in 15 seconds
            NSLog(@"We need to request again in 15 seconds");

            //TODO:
            return;
        }
    }
    
    //For normal control flow
    if( self.handler == NULL){
        handler = [[ResponseHandler alloc]init];
    }
    [handler handle:json from: self];
}

@end
