//
//  PerformanceTester.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "PerformanceTester.h"
#import "TestSettings.h"

@interface PerformanceTester()

@property (strong, nonatomic) TestSettings* settings;
@property (assign, nonatomic) NSInteger timeStart;
@property (assign, nonatomic) NSInteger timeEnd;

-(Boolean) sendDNSRequest :(NSString*)string withId:(NSInteger)identifier;

-(Boolean) getDNSResponse:(NSInteger)identifier;

-(NSMutableArray*) getContent:(NSString*) name :(NSInteger) identifier;

@end

@implementation PerformanceTester
@synthesize settings, timeEnd, timeStart;

-(instancetype)init:(TestSettings *)settingst
{
    self = [super init];
    
    if(self)
    {
        self.settings = settingst;
    }
    return self;
}

-(TestResults *)runTests{
    return NULL;
}
-(NSMutableArray *)runDNSTest:(NSMutableArray *)domains{
    return NULL;
}

-(Boolean)sendDNSRequest:(NSString *)string withId:(NSInteger)identifier{
    return false;
}

-(Boolean)getDNSResponse:(NSInteger)identifier{
    return false;
}
-(NSMutableArray *)getContent:(NSString *)name :(NSInteger)identifier{
    return false;
}

@end
