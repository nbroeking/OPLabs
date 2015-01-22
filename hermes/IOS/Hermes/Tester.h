//
//  Tester.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/22/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Tester : NSObject
{
    bool started;
    bool shouldRun;
}
@property(strong, atomic) NSThread *thread;

-(instancetype)init;
-(void) threadMain;
-(void) start;
-(void) stop;
@end
