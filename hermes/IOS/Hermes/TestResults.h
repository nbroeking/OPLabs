//
//  TestResults.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

//This class holds all relevant information to a test set result
#import <Foundation/Foundation.h>

@interface TestResults : NSObject

@property(assign, atomic) double dns;
@property(assign, atomic) double packetloss;
@property(assign, atomic) double latency;
@property(assign, atomic) double latencySD; //Standard Deviation
@property(assign, atomic) double throughputUpload;
@property(assign, atomic) double throughputDownload;
@property(assign, atomic) double packetlossUnderLoad;
@property(strong, atomic) NSArray *latencyUnderLoad;

@property(assign, atomic) int setID;
@property(assign, atomic) NSInteger routerIdentifier;
@property(assign, atomic) bool valid;
@property(assign, atomic) NSInteger mobileIdentifier;

-(NSString*) getPost;

-(void) print;

@end
