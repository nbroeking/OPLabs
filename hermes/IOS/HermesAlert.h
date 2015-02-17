//
//  HermesAlert.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/17/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>

enum Type { nothing, settings, login};

@interface HermesAlert : UIAlertView
{
     enum Type type;
}

-(void) setType:(enum Type)t;
-(enum Type) getType;
@end
