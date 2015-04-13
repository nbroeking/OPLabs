//
//  ResponseHandler.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@class HermesHttpPost;

@interface ResponseHandler : NSObject
- (instancetype)init;
-(void) handle:(NSMutableDictionary*)json from:(HermesHttpPost*)parent;
@end
