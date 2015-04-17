//
//  ResponseHandler.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "ResponseHandler.h"
#import "SessionData.h"
#import "HermesHttpPost.h"
#import "TestSettings.h"

NSString * const StartMobileURL = @"/api/start_test/mobile";
NSString * const StartRouterURL = @"/api/start_test/router";

@interface ResponseHandler()
@property TestSettings *settings;
-(void) handleLogin: (NSMutableDictionary*)json;

@end

@implementation ResponseHandler
@synthesize settings;

- (instancetype)init
{
    self = [super init];
    if (self) {
        //Generic init
    }
    return self;
}

-(void)handle:(NSMutableDictionary *)json from:(HermesHttpPost*)owner{
    //Handle
    if( json)
    {
        if ( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"Login"]) {
            
            [self handleLogin:json];
        }
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"GetSetId"]) {

            if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
                settings = [[TestSettings alloc] init];
                [self handleSetId:json :owner];
                return;
            }
            NSLog(@"We got an error reporting data in communication");
            [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:nil];
            
        }
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"StartMobileTest"]) {
            if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
                
                [self handleStartMobileTest:json :owner];
                return;
            }
            else {
                NSLog(@"We got an error reporting data in communication");
            [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:nil];
            }
        }
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"StartRouterTest"]) {
            [self handleStartRouterTest:json];
        }
        else{
            NSLog(@"ERROR: Unknown request type");
        }
    }
    else{
        NSLog(@"Error: We should have never had a null json dictionary in the handler");
    }
}

-(void) handleLogin: (NSMutableDictionary*)json{
    if([(NSString*)[json objectForKey:@"status"] isEqualToString:@"success"]){
        [[SessionData getData] setSessionId: [[NSString alloc] initWithString:[json objectForKey:@"user_token"]]];
    }
    else{
        
        [[SessionData getData] setSessionId:@"DOMAIN"];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyLogin" object:(NSString*)[json objectForKey:@"status"]];
}

-(void) handleSetId: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    
    [settings setSetId:((NSInteger)[[json valueForKey:@"set_id"] intValue])];
    
    NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartMobileURL]]];
    
    request.HTTPMethod = @"POST";
    
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
    
    NSString *postString = [NSString stringWithFormat:@"user_token=%@&set_id=%d", [[SessionData getData] sessionIdEncoded], (int)[settings setId]];
    
    NSLog(@"Sending Post = %@", postString);
    NSData *data = [postString dataUsingEncoding:NSUTF8StringEncoding];
    
    [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:data];
    
    NSLog(@"Requesting a start Mobile test");
    [ owner post:request :@"StartMobileTest" :self];
}
    
-(void) handleStartMobileTest: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    NSLog(@"Handle Start Mobile Test");
    
    NSDictionary *config = (NSDictionary*)[json objectForKey:@"config"];
    NSDictionary *dns_config = (NSDictionary*)[config objectForKey:@"dns_config"];
    
    //Parse the json to get the settings
    NSArray *jArray = [dns_config objectForKey:@"invalid_name"];
    
    if (jArray != NULL) {
        [[settings invalidDomains] addObjectsFromArray:jArray];
    }
    
    //Get Valid Names
    jArray = [dns_config objectForKey:@"valid_name"];
    if (jArray != NULL) {
        [[settings validDomains] addObjectsFromArray:jArray];
    }
    
    //Set Resolver
    [settings setDNSServer:[dns_config valueForKey:@"dns_server"]];
    
    //Set the timeout
    [settings setTimeout: [[dns_config valueForKey:@"timeout"] intValue]];
    
    //Set the result ID
     [settings setMobileResultID:[[dns_config valueForKey:@"result_id"] intValue]];
     
    //Request a router start test
     
     NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartRouterURL]]];
     
     request.HTTPMethod = @"POST";
     
     [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
     
     NSString *postString = [NSString stringWithFormat:@"user_token=%@&set_id=%d", [[SessionData getData] sessionIdEncoded], (int)[settings setId]];
     
     NSLog(@"Sending Post = %@", postString);
     NSData *data = [postString dataUsingEncoding:NSUTF8StringEncoding];
     
     [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
     [request setHTTPBody:data];
     
     [ owner post:request :@"StartRouterTest" :self];
}
-(void) handleStartRouterTest: (NSMutableDictionary*)json{
    NSLog(@"Handle Start Router Test");
    if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
        
        [settings setRouterTesultID:[[json valueForKey:@"result_id"]intValue]];
    }
    else {
        [settings setRouterTesultID:-1];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:settings];

}
@end
