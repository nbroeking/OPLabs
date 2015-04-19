//
//  ResultsViewController.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "ResultsViewController.h"
#import "TestState.h"
#import "SessionData.h"
#import "TestResults.h"

NSString * const ViewResultsURL = @"/mobile/test_set/%d";

@implementation ResultsViewController
@synthesize WebView;

-(void)viewDidLoad{
    [super viewDidLoad];
    
    if( ([[TestState getStateMachine] mobileResults] == NULL) || ([[[TestState getStateMachine] mobileResults] valid] == false)){
        NSLog(@"Error with results from results view");
    }
    else{
        NSString *fullURL = [[NSString alloc] initWithFormat:@"%@%@?user_token=%@", [[SessionData getData] hostname], [[NSString alloc] initWithFormat:ViewResultsURL, [[[TestState getStateMachine] mobileResults] setID]] ,[[SessionData getData] sessionIdEncoded]];
        NSURL *url = [NSURL URLWithString:fullURL];
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        [WebView loadRequest:requestObj];
    
        [WebView setDelegate:self];
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