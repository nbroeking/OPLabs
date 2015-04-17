//
//  ResultsViewController.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "ResultsViewController.h"
#import "TestState.h"

@implementation ResultsViewController

-(void)viewDidLoad{
    NSLog(@"Check the state in Results");
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(routerResults:) name:@"ReceivedRouterResults" object:nil];
    
    if( [[TestState getStateMachine] routerResults] != NULL )
    {
         NSLog(@"Received Router Results on the ResultsViewController");
    }
}

-(void) routerResults:(NSNotification*)notification{
    NSLog(@"Received Router Results on the ResultsViewController");
}
@end