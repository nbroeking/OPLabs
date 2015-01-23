//
//  MainViewController.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/20/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "MainViewController.h"

@interface MainViewController ()

@end

@implementation MainViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)Switch:(id)sender {
    
    NSLog(@"Here");
    [self performSegueWithIdentifier:@"Next" sender:self];
    NSLog(@"Here1");
    //[ self performSegueWithIdentifier:@"Next1" sender:self];
    //[ self performSegueWithIdentifier:@"Next2" sender:self];
}
@end
