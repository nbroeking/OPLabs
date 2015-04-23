//
//  LoginViewController.h
//  Hermes
//
//  Created by Sarah Feller on 1/27/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LoginViewController : UIViewController <UITextFieldDelegate>
@property (strong, nonatomic) IBOutlet UITextField *emailField;
@property (strong, nonatomic) IBOutlet UITextField *passwordField;

- (IBAction)Login:(id)sender;

-(void)dismissKeyboard;
@end
