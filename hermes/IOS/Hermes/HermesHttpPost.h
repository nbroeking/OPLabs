//
//  HermesHttpPost.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Communication.h"

@interface HermesHttpPost : NSObject <NSURLConnectionDelegate>

-(void) post:(NSMutableURLRequest*)request :(NSString*)postType;
-(instancetype) init;
@end
