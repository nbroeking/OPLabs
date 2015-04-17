//
//  TestResults.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/14/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface TestResults : NSObject

@property(assign, atomic) double dns;
@property(assign, atomic) double packetloss;
@property(assign, atomic) double latency;
@property(assign, atomic) double throughputUpload;
@property(assign, atomic) double throughputDownload;
@property(assign, atomic) double packetlossUnderLoad;
@property(assign, atomic) double latencyUnderLoad;

@property(assign, atomic) NSInteger routerIdentifier;
@property(assign, atomic) bool valid;

@property(assign, atomic) NSInteger mobileIdentifier;

-(NSString*) getPost;

@end
