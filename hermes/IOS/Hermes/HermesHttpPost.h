//
//  HermesHttpPost.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Communication.h"

@class ResponseHandler;

@interface HermesHttpPost : NSObject <NSURLConnectionDelegate>

-(void) post:(NSMutableURLRequest*)request :(NSString*)postType :(ResponseHandler*)handlert;
-(void) posts:(NSMutableArray*)requestst : (NSString*)postType;

-(void) reportData: (NSMutableDictionary*)json;

-(instancetype) init;
-(instancetype) initWithComm: (Communication*) comm;
-(instancetype) init: (NSObject*)sender;
@end
