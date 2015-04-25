//
//  HermesAlert.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 2/17/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "HermesAlert.h"

@implementation HermesAlert

-(instancetype)init
{
    if( self = [super init])
    {
        self.frame = CGRectMake(0, 0, 0, 0);
        type = -1;
    }
    return self;
}

-(void) setType:(enum Type)t
{
    type = t;
}

-(enum Type)getType
{
    return type;
}

@end
