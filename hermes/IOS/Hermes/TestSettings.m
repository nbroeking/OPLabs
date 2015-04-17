//
//  TestSettings.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "TestSettings.h"

@implementation TestSettings
@synthesize validDomains, invalidDomains, DNSServer, mobileResultID, setId, routerTesultID, timeout;

-(instancetype)init{
    self = [super init];
    
    if( self){
        validDomains = [[NSMutableArray alloc] init];
        invalidDomains = [[NSMutableArray alloc] init];
    }
    return self;
}

-(void) addInvalidDomain: (NSString*) string
{
    [invalidDomains addObject:string];
}
-(void) addValidDomain: (NSString*)string
{
    [validDomains addObject:string];
}

-(void) logValues{
    NSLog(@"%@ %@ %@ %ld %ld %ld %ld", validDomains, invalidDomains, DNSServer, (long)mobileResultID, (long)setId, (long)routerTesultID, (long)timeout);
}
@end
