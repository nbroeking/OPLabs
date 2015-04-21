//
//  AnimationViewController.m
//  Hermes
//
//  Created by Nicolas Broeking on 4/11/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "AnimationViewController.h"
#import "TestState.h"
#import "MainNavigationController.h"
#import "TestSettings.h"
#import "SessionData.h"
#import "ResultsViewController.h"
#import "TestState.h"

@interface AnimationViewController()
@property (strong, nonatomic) TestState *stateMachine;

@property (strong, nonatomic) IBOutlet UILabel *StateLabel;
@property (strong, nonatomic) NSTimer* updater;

-(void) goToResults: (NSNotification*)notification;
@end

@implementation AnimationViewController
@synthesize stateMachine;
@synthesize AnimationImage;
@synthesize updater;
@synthesize StateLabel;

-(void)viewDidLoad{
    [super viewDidLoad];
    
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    stateMachine = [TestState getStateMachine];
    
    //Start Testing Process if IDLE
    if([stateMachine getState] == IDLE)
    {
        NSLog(@"Starting a Test");
        //We need to start the test
        [[((MainNavigationController*)self.navigationController) tester] startTest];
    }
    
    [self updateLabel];
    //Animation stuff
    // Load images
    NSArray *imageNames = @[@"ani2_1.png", @"ani2_2.png", @"ani2_3.png", @"ani2_4.png",@"ani2_5.png", @"ani2_6.png",@"ani2_7.png", @"ani2_8.png",@"ani2_9.png", @"ani2_10.png",];
    
    NSMutableArray *images = [[NSMutableArray alloc] init];
    for (int i = 0; i < imageNames.count; i++) {
        [images addObject:[UIImage imageNamed:[imageNames objectAtIndex:imageNames.count - 1 -i]]];
    }
    
    // Normal Animation
 
    AnimationImage.animationImages = images;
    AnimationImage.animationDuration = 2;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(startTestNotified:) name:@"NotifyStartTest" object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(goToResults:) name:@"TestComplete" object:nil];
    
}
-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [AnimationImage startAnimating];
    
    updater = [NSTimer timerWithTimeInterval:.25f target:self selector:@selector(updateLabel) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:updater forMode:NSRunLoopCommonModes];
    //Set Timer
    
}
-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [AnimationImage stopAnimating];
    
    [updater invalidate];
}

-(void) updateLabel{
    TestState *state = [TestState getStateMachine];
    
    NSString *label = [[NSString alloc] initWithFormat:@"State: %@", [state getStateAsString]];
    
    [StateLabel setText:label];
}
-(void) startTestNotified:(NSNotification*) notification{
    [self performSelectorOnMainThread:@selector(startTestNotifiedHelper:) withObject:notification waitUntilDone:NO];
}
-(void) startTestNotifiedHelper: (NSNotification *)notification{
    NSLog(@"Notified about a Start Test");
    
    TestSettings *settings = (TestSettings*)[notification object];
    
    if( settings == nil ){
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Testing Error" message:@"There was an starting a performance test. Please try again." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:NULL, nil];
        
        [alert show];
    }
}
-(void) goToResults:(NSNotification *)notification
{
    [self performSelectorOnMainThread:@selector(goToResultsHelper:) withObject:notification waitUntilDone:NO];
}
-(void) goToResultsHelper: (NSNotification*)notification{
    
    if( [notification object] == NULL || [(TestResults*)[notification object] valid] == false)
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Testing Error" message:@"There was an error running a performance test. We are sorry for the inconvience. Please try again." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:NULL, nil];
        [alert show];
    }
    else{
        
        UIStoryboard *sb = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
        ResultsViewController *vc = [sb instantiateViewControllerWithIdentifier:@"ResultsViewController"];
        
        NSMutableArray *controllers = [NSMutableArray arrayWithArray:self.navigationController.viewControllers];
        [controllers removeLastObject];
        [controllers addObject:vc];
        
        [self.navigationController setViewControllers:controllers animated:YES];
    }
}
#pragma mark - Alert View
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    //This is only called on error
    [stateMachine setState:IDLE];
    [self.navigationController popViewControllerAnimated:YES];
}
@end
