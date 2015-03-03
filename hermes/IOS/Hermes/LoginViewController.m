//
//  LoginViewController.m
//  Hermes
//
//  Created by Sarah Feller on 1/27/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "LoginViewController.h"
#import "SessionData.h"

@interface LoginViewController ()
@property (strong, nonatomic) SessionData *data;
@property (strong, nonatomic) NSURL *regURL;
@end

@implementation LoginViewController
@synthesize data;
@synthesize emailField;
@synthesize passwordField;
@synthesize regURL;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    //Set the background color
    self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"background"]];
    
    //Get Data Singleton
    data = [SessionData getData];
    
    regURL = [NSURL URLWithString: [NSString stringWithFormat:@"%@%@", [[SessionData getData] hostname], @"/register"]];
    
    //Set up the tap recognizer to close keyboard
    UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]
                                   initWithTarget:self
                                   action:@selector(dismissKeyboard)];
    
    [self.view addGestureRecognizer:tap];
    
    [emailField setDelegate:self];
    [passwordField setDelegate:self];
    
    [emailField setText:[data email]];
    [passwordField setText:[data password]];
}


-(void)dismissKeyboard
{
    NSLog(@"Kill the keyboard");
    [emailField resignFirstResponder];
    [passwordField resignFirstResponder];
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)Login:(id)sender {
    NSLog(@"Login");
    [data setPassword:[passwordField text]];
    [data setEmail:[emailField text]];
    [data save];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"LOGIN" object:nil];
    [self.navigationController popToRootViewControllerAnimated:YES];
}

- (IBAction)SignUp:(id)sender {
    NSLog(@"SignUp");
    [[UIApplication sharedApplication] openURL:regURL];
}
@end
