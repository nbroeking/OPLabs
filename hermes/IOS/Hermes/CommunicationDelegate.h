//
//  CommProtocol.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/12/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//
// This Interface provides a way for the communication subsystem to call direct methods on the caller
// This method also gets called on Broadcast events

#import <Foundation/Foundation.h>

@protocol CommunicationDelegate <NSObject>

@required
- (void) notifyLogin: (NSString*)error;

@end