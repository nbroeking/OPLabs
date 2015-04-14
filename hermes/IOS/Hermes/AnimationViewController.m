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

-(void) goToResults: (NSNotification*)notification;
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
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(goToResults:) name:@"TestComplete" object:nil];
    
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
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Testing Error" message:@"There was an starting a performance test. Please try again." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:NULL, nil];
        
        [alert show];
    }
}

-(void) goToResults: (NSNotification*)notification{
    
    if( [notification object] == NULL)
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Testing Error" message:@"There was an error running a performance test. We are sorry for the inconvience. Please try again." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:NULL, nil];

#warning DONT GIVE THIS TO CABLE LABS
        UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(220, 10, 40, 40)];
        [imageView setImage: [UIImage imageNamed:@"josh"]];
        [alert addSubview:imageView];
        
        [alert show];
    }
    else{
        [self.navigationController popViewControllerAnimated:NO];
        [(MainNavigationController*)self.navigationController performSegueWithIdentifier:@"GotoResults" sender:self];
    }
}
#pragma mark - Alert View
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    [self.navigationController popViewControllerAnimated:YES];
}
@end
