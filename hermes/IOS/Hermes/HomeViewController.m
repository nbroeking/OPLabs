//
//  HomeViewController.m
//  Hermes
//
//  Created by Nic Broeking on 2/3/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "HomeViewController.h"
#import "SessionData.h"
#import "MainNavigationController.h"
#import "HermesAlert.h"
#import "TestState.h"

@interface HomeViewController ()
@property (nonatomic, strong) UIAlertView *loading;
@end

@implementation HomeViewController
@synthesize data, loading;
@synthesize RunTestsButton;

//When the view is loaded we add ourselves to the notification center watch list
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Do any additional setup after loading the view.
    data = [SessionData getData];
    
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];

}

//When we are done testing we need to move to the new testing view. Depending on the state we can either move to the animation or the results page
- (IBAction)MoveToTesting:(id)sender {
    //NSLog(@"Run Test was pressed");
    
    if (![self checkLogin]) {
        
        TestState *stateMahine = [TestState getStateMachine];
        if([stateMahine getState] != COMPLETED)
        {
            //Go to animation
            [self performSegueWithIdentifier:@"Testing" sender:self];
        }
        else{
            [self performSegueWithIdentifier:@"Results" sender:self];
        }
    }
}

//When the view is about to be shown we have to adjust for our states
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self.navigationController setNavigationBarHidden:YES animated:animated];
    
    if( [data shouldTransfer]){
        [data setShouldTransfer:false];
        [self performSegueWithIdentifier:@"Results" sender:self];
    }
    
    TestState *stateMachine = [TestState getStateMachine];
    
    
    if( [stateMachine getState] == IDLE ){
        [RunTestsButton setTitle:@"Run Tests" forState:UIControlStateNormal];
    }
    else{
        [RunTestsButton setTitle:@"Get Results" forState:UIControlStateNormal];
    }
    
    //Ask the notification center to message us when we the app becomes active
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appBecameActive) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appBecameActive) name:@"LOGIN" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(notifyLogin:) name:@"NotifyLogin" object:nil];
}

//Clean up the view before we disappear
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.navigationController setNavigationBarHidden:NO animated:animated];
    
    //Clean up the notification center
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


//Call back from the NSNotification center
-(void)appBecameActive
{
    //NSLog(@"Home View Controller knows the app became active");
    
    if([self checkLogin])
    {
        [self startLogin];
        [[(MainNavigationController*)self.navigationController communicator] login:self];
    }
}

//Checks if we are logged in. If we aren't we start the login process
-(BOOL)checkLogin
{
    //All Login checking
    //Change to waiting for login
     if (!([data sessionId] == NULL) &&![[data sessionId] isEqualToString:@""]) {
        NSLog(@"Already Logged in: %@", [data sessionId]);
        return false;
    }
    
    //NSLog(@"Check Login");
    if( ([[data email] isEqualToString:@""]&& [[data password] isEqualToString:@""]))
    {
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Welcome" message:@"Welcome to hermes. Please set up your login information." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert setType:login];
        [alert show];
        return false;
    }
    else if ([[data hostname] isEqualToString:@""])
    {
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Error" message:@"Invalid Hostname configured." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert setType:settings];
        [alert show];
        return false;
    }
   
    return true;
}

#pragma mark -Communication

//We need to redirect all method calls to the main thread to do gui updating
-(void)notifyLogin:(NSString *)error{
    [self performSelectorOnMainThread:@selector(notifyLoginHelper:) withObject:error waitUntilDone:NO];
}

//Notified when a login broadcast has been received. Depending on the state of the session id
//We can determine if there was an error and adjust accordingly
-(void)notifyLoginHelper:(NSString*)result
{
    if([[data sessionId] isEqualToString:@"ERROR"])
    {
        NSLog(@"Login Error: ERROR");
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Login Error" message:@"You have invalid credentials." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [alert setType:login];
        [alert show];
        [data setSessionId:@""];
    }
    else if([[data sessionId] isEqualToString:@"DOMAIN"])
    {
        NSLog(@"Login Error: Domain");
        HermesAlert *alert = [[HermesAlert alloc] initWithTitle:@"Login Error" message:@"There was an error finding your server. Are you sure you have the right hostname? Please check the app settings for the correct domain!" delegate:self cancelButtonTitle:@"Ok Ill check!" otherButtonTitles:nil];
        [alert setType: settings];
        [alert show];
        [data setSessionId:@""];
    }
    else{
        NSLog(@"Successful Login");
    }
    [self performSelector:@selector(endLogin) withObject:nil afterDelay:1];
}

#pragma mark - Alert View
//This is called when an alert button has been pressed. It allows us to go to different menus depending on
//What kind of alert was pressed
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

//Called when we start the login process to update the gui
-(void)startLogin
{
    NSLog(@"Start Login");
    loading = [[HermesAlert alloc] initWithTitle:@"Logging in" message:@"" delegate:self cancelButtonTitle:nil otherButtonTitles:nil];
   
    UIActivityIndicatorView *indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    
    [indicator startAnimating];

    [loading setValue:indicator forKey:@"accessoryView"];
    [loading show];
}

//Called when the login process has completed so we can update the gui
-(void)endLogin
{
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:2];
    [UIView setAnimationDelay:.25];
    [loading dismissWithClickedButtonIndex:0 animated:YES];
    [UIView commitAnimations];
    loading = nil;
}

// Tell the navigation controller to perform a segue bringing us to the login page
-(IBAction)goToLogin:(id)sender
{
    [self performSegueWithIdentifier:@"LoginSegue" sender:self];
}
//Tells the navigation controller to bring us to the settings page
- (IBAction)goToSettings:(id)sender {
    NSURL *appSettings = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
    [[UIApplication sharedApplication] openURL:appSettings];
}
@end
