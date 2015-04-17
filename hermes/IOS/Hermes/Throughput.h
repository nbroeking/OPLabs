//
//  Throughput.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/16/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PerformanceTester;
@class TestSettings;
@class TestResults;

@interface Throughput : NSObject <NSStreamDelegate>

-(instancetype)init :(PerformanceTester*)parent withSettings:(TestSettings*)config withResults:(TestResults*)answer;

-(void) runDownloadTest;

-(void) shutdown;
@end
