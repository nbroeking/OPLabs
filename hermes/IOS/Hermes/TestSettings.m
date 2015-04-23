//
//  TestSettings.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestSettings.h"

@implementation TestSettings
@synthesize validDomains, invalidDomains, DNSServer, mobileResultID, setId, routerResultID, timeout;
@synthesize throughputServer, port;

-(instancetype)init{
    self = [super init];
    
    if( self){
        validDomains = [[NSMutableArray alloc] init];
        invalidDomains = [[NSMutableArray alloc] init];
    }
    return self;
}
//Add an invalid domain to the list of domains
-(void) addInvalidDomain: (NSString*) string
{
    [invalidDomains addObject:string];
}
//Ad a valid domain to the list
-(void) addValidDomain: (NSString*)string
{
    [validDomains addObject:string];
}

//Print the values of the settings for debug purpses
-(void) logValues{
    NSLog(@"%@ %@ %@ %ld %ld %ld %ld", validDomains, invalidDomains, DNSServer, (long)mobileResultID, (long)setId, (long)routerResultID, (long)timeout);
}
@end
