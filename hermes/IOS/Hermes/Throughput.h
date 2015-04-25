//
//  Throughput.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/16/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

//This class opens a tcp connection to the throughput server in order to run a thoughput test
//This class assumes the owner will be running a download test first and then an upload test

//NOTE: I am not proud of this class but it gets the job done and thats what we needed. IOS isn't
//designed to be a platform to do this low level timing things

#import <Foundation/Foundation.h>

@class PerformanceTester;
@class TestSettings;
@class TestResults;

@interface Throughput : NSObject <NSStreamDelegate>

-(instancetype)init :(PerformanceTester*)parent withSettings:(TestSettings*)config withResults:(TestResults*)answer;

-(void) runDownloadTest;
-(void) runUploadTest;
-(void) shutdown;
@end
