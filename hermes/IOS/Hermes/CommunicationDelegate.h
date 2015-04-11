//
//  CommProtocol.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/12/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol CommunicationDelegate <NSObject>

@required
- (void) notifyLogin: (NSString*)error;

@end