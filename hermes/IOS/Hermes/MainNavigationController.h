//
//  MainNavigationController.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/20/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Communication.h"

@interface MainNavigationController : UINavigationController

@property (strong, atomic) Communication* communicator;

@end
