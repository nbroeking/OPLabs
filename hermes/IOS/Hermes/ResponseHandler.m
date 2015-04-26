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
#import "TestState.h"
#import "TestResults.h"

NSString * const StartMobileURL = @"/api/start_test/mobile";
NSString * const StartRouterURL = @"/api/start_test/router";

@interface ResponseHandler()
@property TestSettings *settings;
-(void) handleLogin: (NSMutableDictionary*)json;

@end

@implementation ResponseHandler
@synthesize settings;

//Handle a return from the json
//This decides what kind of post it is and sends it to the specific handler methods
//below
-(void)handle:(NSMutableDictionary *)json from:(HermesHttpPost*)owner{
    //Handle
    if( json)
    {
        //NSLog(@"Json: %@", json);
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
        else if( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"ReportResults"]) {
            [self handleStartReportResults:json];
        }
        else{
            NSLog(@"ERROR: Unknown request type");
        }
    }
    else{
        NSLog(@"Error: We should have never had a null json dictionary in the handler");
    }
}

//If it is a login we check for an error and then broadcast our response
-(void) handleLogin: (NSMutableDictionary*)json{
    if([(NSString*)[json objectForKey:@"status"] isEqualToString:@"success"]){
        [[SessionData getData] setSessionId: [[NSString alloc] initWithString:[json objectForKey:@"user_token"]]];
    }
    else{
        
        [[SessionData getData] setSessionId:@"DOMAIN"];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyLogin" object:(NSString*)[json objectForKey:@"status"]];
}

//If it was a set id post then we store the set id in a settings object ang give it to our owner
-(void) handleSetId: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    
    @try {
        [settings setSetId:((NSInteger)[[json valueForKey:@"set_id"] intValue])];
        
        NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartMobileURL]]];
        
        request.HTTPMethod = @"POST";
        
        [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
        
        NSString *postString = [NSString stringWithFormat:@"user_token=%@&set_id=%d", [[SessionData getData] sessionIdEncoded], (int)[settings setId]];
        
        NSData *data = [postString dataUsingEncoding:NSUTF8StringEncoding];
        
        [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
        [request setHTTPBody:data];
        
        NSLog(@"Requesting a start Mobile test");
        [ owner post:request :@"StartMobileTest" :self];
    }
    @catch (NSException *exception) {
        NSLog(@"Error Starting getting setId test");
        [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:NULL];
    }
}

//If it was a start mobile post then we get the test settings from the json and then request a start
//Router test
-(void) handleStartMobileTest: (NSMutableDictionary*)json :(HermesHttpPost*)owner {
    NSLog(@"Handle Start Mobile Test");
    
    @try {
        NSDictionary *config = (NSDictionary*)[json objectForKey:@"config"];
        NSDictionary *dns_config = (NSDictionary*)[config objectForKey:@"dns_config"];
        NSDictionary *throughput_config = (NSDictionary*)[config objectForKey:@"throughput_config"];
        
        //Parse the json to get the settings
        NSArray *jArray = [dns_config objectForKey:@"invalid_names"];
        
        if (jArray != NULL) {
            [[settings invalidDomains] addObjectsFromArray:jArray];
        }
        
        //Get Valid Names
        jArray = [dns_config objectForKey:@"valid_names"];
        if (jArray != NULL) {
            [[settings validDomains] addObjectsFromArray:jArray];
        }
        
        //Set Resolver
        [settings setDNSServer:[dns_config valueForKey:@"dns_server"]];
        
        //Set the timeout
        [settings setTimeout: [[dns_config valueForKey:@"timeout"] intValue]];
        
        //Set the result ID
        [settings setMobileResultID:[[json valueForKey:@"result_id"] intValue]];
        
        
        //Get the throughput config
        
        NSString* server = [throughput_config valueForKey:@"server_ip"] ;
        NSArray *serverParts = [server componentsSeparatedByString:@":"];
        
        [settings setThroughputServer:[serverParts objectAtIndex:0]];
        [settings setPort:[(NSString*)[serverParts objectAtIndex:1] intValue]];
        
        //Request a router start test
        
        NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@%@", [[SessionData getData]hostname], StartRouterURL]]];
        
        request.HTTPMethod = @"POST";
        
        [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
        
        NSString *postString = [NSString stringWithFormat:@"user_token=%@&set_id=%d", [[SessionData getData] sessionIdEncoded], (int)[settings setId]];
        
        
        NSData *data = [postString dataUsingEncoding:NSUTF8StringEncoding];
        
        [request setValue:[NSString stringWithFormat:@"%lu", (unsigned long)[data length]] forHTTPHeaderField:@"Content-Length"];
        [request setHTTPBody:data];
        
        [ owner post:request :@"StartRouterTest" :self];
    }
    @catch (NSException *exception) {
        NSLog(@"Error Starting a mobile test");
        [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:NULL];
    }
}

//We just check to make sure the start router test succeded or failed and then notify everyone that we are starting a test
-(void) handleStartRouterTest: (NSMutableDictionary*)json{
    NSLog(@"Handle Start Router Test");
    if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
        NSLog(@"There was a router");
        [settings setRouterResultID:[[json valueForKey:@"result_id"]intValue]];
    }
    else {
        NSLog(@"There was no router found");
        [settings setRouterResultID:-1];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyStartTest" object:settings];

}
//check to make sure that we correctly reported our results
//NOTE: If we dont we just forget about them. Ideally we would try again
//But for now we just try once
-(void) handleStartReportResults: (NSMutableDictionary*)json{
    NSLog(@"Handle Start Report Tests");
    if ([[json valueForKey:@"status"] isEqualToString:@"success"]) {
        
        NSLog(@"Phone successfully reported its Test results to the controller");
    }
    else {
        NSLog(@"Phone failed to upload results");
    }    
}
@end
