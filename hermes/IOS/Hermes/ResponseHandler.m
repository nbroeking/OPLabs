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
    NSLog(@"Handling");
    //Handle
    if( json)
    {
        if ( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"Login"]) {
            
            [self handleLogin:json];
        }
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"GetSetId"]) {
            NSLog(@"Received a handle for a Set Id");

            if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
                
                settings = [[TestSettings alloc] init];
                [self handleSetId:json :owner];
                return;
            }
            NSLog(@"We got an error reporting data in communication");
            [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:nil];
            
        }
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"StartMobileTest"]) {
            NSLog(@"Handling Start Mobile Test");
            if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
                
                [self handleStartMobileTest:json :owner];
                return;
            }
            NSLog(@"We got an error reporting data in communication");
            [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:nil];
        }
        else{
            NSLog(@"ERROR: Unknown request type");
        }
    }
    else{
        NSLog(@"Error: We should have never had a null json dictionary in the handler");
    }
}

-(void) reportData: (NSMutableDictionary*)json withType:(NSString*)type{
    NSLog(@"Received Data in Communication");
    

}

-(void) handleLogin: (NSMutableDictionary*)json{
    if([(NSString*)[json objectForKey:@"status"] isEqualToString:@"success"]){
        [[SessionData getData] setSessionId: [[NSString alloc] initWithString:[json objectForKey:@"user_token"]]];
    }
    else{
        
        [[SessionData getData] setSessionId:NULL];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyLogin" object:(NSString*)[json objectForKey:@"status"]];
}

-(void) handleSetId: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    
    NSLog(@"Set id = %d",(int)((NSInteger)[[json valueForKey:@"set_id"]intValue]));
    [settings setSetId:((NSInteger)[[json valueForKey:@"set_id"] intValue])];
    
    NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartMobileURL]]];
    
    request.HTTPMethod = @"POST";
    
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
    
    NSString *postString = [NSString stringWithFormat:@"user_token=%@&set_id=%d", [[SessionData getData] sessionId], (int)[settings setId]];
    
    NSLog(@"Sending Post = %@", postString);
    NSData *data = [postString dataUsingEncoding:NSUTF8StringEncoding];
    
    [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
    [request setHTTPBody:data];
    
    [ owner post:request :@"StartMobileTest" :self];
}
-(void) handleStartMobileTest: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    NSLog(@"Handle Start Mobile Test");
}
@end
