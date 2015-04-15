//
//  SessionData.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/12/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SessionData : NSObject 

//Singleton
+(SessionData*) getData;

@property (strong, atomic) NSString *sessionId;

@property (strong, atomic) NSString *hostname;
@property (strong, atomic) NSString *email;
@property (strong, atomic) NSString *password;

//This is a work around to get the view controller to move views
@property (assign, atomic) Boolean shouldTransfer;


-(void) sync;
-(void) save;
- (void)registerDefaultsFromSettingsBundle;
-(void)setSessionId:(NSString *)theSessionId;
-(NSString*)sessionId;
-(NSString*)sessionIdEncoded;

@end
