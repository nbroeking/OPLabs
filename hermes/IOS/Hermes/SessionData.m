//
//  SessionData.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/12/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "SessionData.h"

@implementation SessionData
@synthesize email, password, hostname, sessionId, shouldTransfer;

//Singleton factory creator
+(SessionData*) getData
{
    static SessionData *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[SessionData alloc] init];
    });
    return sharedInstance;
}
//Initilize all our values to nil
- (instancetype)init
{
    self = [super init];
    if (self) {
        NSLog(@"Creating the Session Data Object");
        sessionId = nil;
        hostname = nil;
        email = nil;
        password = nil;
        shouldTransfer = false;
    }
    return self;
}
//We need to encode the session id to be able to send it to the controller
-(NSString*)sessionIdEncoded
{
    @synchronized(self){
        NSString * encodedString = (NSString *)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(
                                                                                       NULL,
                                                                                       (CFStringRef)sessionId,
                                                                                       NULL,
                                                                                       (CFStringRef)@"!*'();:@&=+$,/?%#[]",
                                                                                       kCFStringEncodingUTF8 ));
        return encodedString;
    }
}
//This method pulls the settings from the settings bundle
-(void)sync
{
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
    
    hostname = [[standardUserDefaults objectForKey:@"hostname"] copy];
    email = [[standardUserDefaults objectForKey:@"email"] copy];
    password = [[standardUserDefaults objectForKey:@"password"] copy];
    
    
    //If they are null then we need to write the defaults and try again
    if( !hostname || !email || !password)
    {
        [self registerDefaultsFromSettingsBundle];
        [self sync];
    }
}

-(void)save
{
    //NSLog(@"Save Settings");
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
    [standardUserDefaults setValue:email forKey:@"email"];
    [standardUserDefaults setValue:password forKey:@"password"];
    //Save Data
}
#pragma NSUserDefaults
//This method will only be called once. On the very first time that the app is launched
- (void)registerDefaultsFromSettingsBundle {
    //NSLog(@"Writting Defaults");
    // this function writes default settings as settings
    NSDictionary *userDefaultsDefaults = [NSDictionary dictionaryWithObjectsAndKeys: @"https://128.138.202.143", @"hostname", @"", @"email", @"", @"password", nil];
    
    [[NSUserDefaults standardUserDefaults] registerDefaults:userDefaultsDefaults];
    
}
@end
