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

-(void) viewWillDisappear:(BOOL)animated {
    if ([self.navigationController.viewControllers indexOfObject:self]==NSNotFound) {
        // back button was pressed.  We know this is true because self is no longer
        // in the navigation stack.
        
        NSLog(@"Back was pressed we need to set our state appropriatly");
        
        [[TestState getStateMachine] setState:IDLE];
    }
    [super viewWillDisappear:animated];
}
@end