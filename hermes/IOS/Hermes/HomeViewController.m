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
#import "HermesAlert.h"

@interface HomeViewController ()
@property (nonatomic, strong) UIAlertView *loading;
@end

@implementation HomeViewController
@synthesize data, loading;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [[SessionData getData] sync];
    data = [SessionData getData];
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    //Ask the notification center to message us when we the app becomes active
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appBecameActive) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appBecameActive) name:@"LOGIN" object:nil];
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
        [self startLogin];
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
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Welcome" message:@"Welcome to hermes. Please set up your login information." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert setType:login];
        [alert show];
        return false;
    }
    
    if (![[data sessionId] isEqualToString:@""]) {
        return false;
    }
    return true;
}
#pragma mark -Communication
-(void)notifyLogin
{
    if ([data sessionId] != nil)
    {
        if([[data sessionId] isEqualToString:@"ERROR"])
        {
            HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Login Error" message:@"There was an error connecting to the server." delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [alert setType:nothing];
            [alert show];
            [data setSessionId:nil];
        }
        else if([[data sessionId] isEqualToString:@"DOMAIN"])
        {
            HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Login Error" message:@"There was an error finding your server. Are you sure you have the right hostname? Please check the app settings for the correct domain!" delegate:self cancelButtonTitle:@"Ok Ill check!" otherButtonTitles:nil];
            [alert setType: settings];
            [alert show];
            [data setSessionId:nil];
        }
        else
        {
             NSLog(@"Successful Login");
        }
    }
    else
    {
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Login Error" message:@"You have invalid credentials." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert setType:login];
        [alert show];
    }
    [self performSelector:@selector(endLogin) withObject:nil afterDelay:1];
}
#pragma mark - Alert View
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    
    enum Type type = [(HermesAlert*)alertView getType];
    
    if( type == settings)
    {
        [self goToSettings:self];
    }
    else if(type == login)
    {
        [self goToLogin:self];
    }
    else
    {
        NSLog(@"Nothing should happen on this alert");
    }
}
-(void)startLogin
{
    NSLog(@"Start Login");
    loading = [[HermesAlert alloc] initWithTitle:@"Logging in" message:@"" delegate:self cancelButtonTitle:nil otherButtonTitles:nil];
    
    UIActivityIndicatorView *indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    
    [indicator startAnimating];
    [loading setValue:indicator forKey:@"accessoryView"];
    [loading show];
}
-(void)endLogin
{
    NSLog(@"End Login");
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:2];
    [UIView setAnimationDelay:1.0];
    [loading dismissWithClickedButtonIndex:0 animated:YES];
    [UIView commitAnimations];
    loading = nil;
}
/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

-(IBAction)goToLogin:(id)sender
{
    NSLog(@"Go to Login");
    [self performSegueWithIdentifier:@"LoginSegue" sender:self];
}
- (IBAction)goToSettings:(id)sender {
    NSLog(@"Go to Settings");
    
    if (&UIApplicationOpenSettingsURLString != NULL) {
        NSURL *appSettings = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
        [[UIApplication sharedApplication] openURL:appSettings];
    }
}
@end
