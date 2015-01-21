//
//  Communication.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/18/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Communication : NSObject <NSStreamDelegate>
{
    bool started;
    bool inputOpen;
    bool outputOpen;
}
@property(strong, atomic) NSRunLoop *loop;
@property(strong, atomic) NSThread *thread;

-(instancetype)init;
-(void) threadMain;
-(void) start;
-(void) stop;


@end
