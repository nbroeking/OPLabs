//
//  PerformanceTester.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TestResults.h"
#import "TestSettings.h"

@interface PerformanceTester : NSObject

-(instancetype)init :(TestSettings*) settingst;
 
-(TestResults*) runTests;

-(NSMutableArray*) runDNSTest: (NSMutableArray*)domains;
@end
