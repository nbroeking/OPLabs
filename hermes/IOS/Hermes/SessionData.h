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

-(void) sync;
-(void) save;
- (void)registerDefaultsFromSettingsBundle;
@end
