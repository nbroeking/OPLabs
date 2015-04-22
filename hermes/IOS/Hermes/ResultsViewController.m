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

//Initilize the view by loading the results page when we are created
-(void)viewDidLoad{
    [super viewDidLoad];
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    if( ([[TestState getStateMachine] mobileResults] == NULL) || ([[[TestState getStateMachine] mobileResults] valid] == false)){
        NSLog(@"Error with results from results view");
    }
    else{
        [WebView setDelegate:self];
        NSString *fullURL = [[NSString alloc] initWithFormat:@"%@%@?user_token=%@", [[SessionData getData] hostname], [[NSString alloc] initWithFormat:ViewResultsURL, [[[TestState getStateMachine] mobileResults] setID]] ,[[SessionData getData] sessionIdEncoded]];
        NSURL *url = [NSURL URLWithString:fullURL];
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        NSLog(@"%@", [url absoluteString]);
        [WebView loadRequest:requestObj];
    }
}
//We need to tell the web view to use us as a delegate when we appear
-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    [WebView setDelegate:self];
}

//Before we disapear we need to clean up our state
-(void) viewWillDisappear:(BOOL)animated {
    if ([self.navigationController.viewControllers indexOfObject:self]==NSNotFound) {
        // back button was pressed.  We know this is true because self is no longer
        // in the navigation stack.
        
        NSLog(@"Back was pressed we need to set our state appropriatly");
        
        [[TestState getStateMachine] setState:IDLE];
    }
    [WebView setDelegate:nil];
    [super viewWillDisappear:animated];
}
@end