//
//  HomeViewController.m
//  Hermes
//
//  Created by Sarah Feller on 2/3/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "HomeViewController.h"
#import "SessionData.h"
#import "MainNavigationController.h"

@interface HomeViewController ()

@end

@implementation HomeViewController
@synthesize data;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    data = [SessionData getData];
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    //Ask the notification center to message us when we the app becomes active
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appBecameActive) name:UIApplicationDidBecomeActiveNotification object:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES animated:animated];
}
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.navigationController setNavigationBarHidden:NO animated:animated];
}

//Methods to interact with the application

//Call back from the NSNotification center
-(void)appBecameActive
{
    NSLog(@"Home View Controller knows the app became active");
    
    if([self checkLogin])
    {
        [[(MainNavigationController*)self.navigationController communicator] login:self];
    }
}


-(BOOL)checkLogin
{
    //All Login checking
    //Change to waiting for login
    NSLog(@"Check Login");
    if( [[data email] isEqualToString:@""]|| [[data password] isEqualToString:@""] || [[data hostname] isEqualToString:@""])
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Welcome" message:@"Welcome to hermes. Please set up your login information." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert show];
        return false;
    }
    return true;
}
#pragma mark -Communication
-(void)notifyLogin
{
    if ([data sessionId] != nil)
    {
        NSLog(@"Successful");
    }
    else
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Login Error" message:@"You have invalid credentials." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert show];
    }
}
#pragma mark - Alert View
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    if (buttonIndex == 0){
        //delete it
        [self goToSettings:self];
    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)goToSettings:(id)sender {
    NSLog(@"Go to Settings");
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
}
@end
