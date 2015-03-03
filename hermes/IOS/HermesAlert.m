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
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
