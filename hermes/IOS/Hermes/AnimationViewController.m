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

@interface AnimationViewController()
@property (strong, nonatomic) TestState *stateMachine;

@end

@implementation AnimationViewController
@synthesize stateMachine;
@synthesize AnimationImage;

-(void)viewDidLoad{
    [super viewDidLoad];
    
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    //Start Testing Process if IDLE
    if([stateMachine getState] == IDLE)
    {
        //We need to start the test
        [[((MainNavigationController*)self.navigationController) tester] startTest];
    }
    
    //Animation stuff
    // Load images
    NSArray *imageNames = @[@"ani2_1.png", @"ani2_2.png", @"ani2_3.png", @"ani2_4.png",@"ani2_5.png", @"ani2_6.png",@"ani2_7.png", @"ani2_8.png",@"ani2_9.png", @"ani2_10.png",];
    
    NSMutableArray *images = [[NSMutableArray alloc] init];
    for (int i = 0; i < imageNames.count; i++) {
        [images addObject:[UIImage imageNamed:[imageNames objectAtIndex:i]]];
    }
    
    // Normal Animation
 
    AnimationImage.animationImages = images;
    AnimationImage.animationDuration = 2;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(startTestNotified:) name:@"NotifyStartTest" object:nil];
    
}
-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [AnimationImage startAnimating];
}
-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [AnimationImage stopAnimating];
}

-(void) startTestNotified: (NSNotification *)notification{
    NSLog(@"Notified about a Start Test");
    
    TestSettings *settings = (TestSettings*)[notification object];
    
    if( settings == nil ){
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Testing Error" message:@"There was an error running a test. Please try again." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:NULL, nil];
        
        [alert show];
    }
}

#pragma mark - Alert View
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    [self.navigationController popViewControllerAnimated:YES];
}
@end
