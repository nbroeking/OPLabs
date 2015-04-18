//
//  UDPSocket.h
//  Hermes
//
//  Created by Nicolas Broeking on 4/16/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TestSettings.h"
#import "TestResults.h"

@interface UDPSocket : NSObject

-(instancetype)init :(TestSettings*)settings;

-(BOOL) sendData :(NSData*)data to:(NSString*) to;

-(NSData*) recvData :(NSInteger) timeout;

-(void) shutdown;
@end
