//
//  TestSettings.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TestSettings : UIViewController

@property (strong, atomic) NSMutableArray *validDomains;
@property (strong, atomic) NSMutableArray *invalidDomains;
@property (strong, atomic) NSString * DNSServer;
@property (assign, atomic) NSInteger timeout;
@property (assign, atomic) NSInteger setId;
@property (assign, atomic) NSInteger mobileResultID;
@property (assign, atomic) NSInteger routerResultID;

//Throughput settings
@property (strong, atomic) NSString* throughputServer;
@property (assign, atomic) NSInteger port;

-(void) addInvalidDomain: (NSString*) string;
-(void) addValidDomain: (NSString*)string;

-(void) logValues;
@end
