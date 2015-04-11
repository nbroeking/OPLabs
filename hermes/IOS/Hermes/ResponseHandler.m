//
//  ResponseHandler.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "ResponseHandler.h"
#import "SessionData.h"

@interface ResponseHandler()
-(void) handleLogin: (NSMutableDictionary*)json;
@end

@implementation ResponseHandler

- (instancetype)init
{
    self = [super init];
    if (self) {
        //Generic init
    }
    return self;
}

-(void)handle:(NSMutableDictionary *)json{
    NSLog(@"Handling");
    //Handle
    if( json)
    {
        if ( [(NSString*)[json objectForKey:@"POST_TYPE"] isEqualToString:@"Login"]) {
            
            [self handleLogin:json];
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
        
        [[SessionData getData] setSessionId:NULL];
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NotifyLogin" object:(NSString*)[json objectForKey:@"status"]];
}
@end
