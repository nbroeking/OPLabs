//
//  AboutViewController.m
//  Hermes
//
//  Created by Sarah Feller on 4/24/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "AboutViewController.h"

@implementation AboutViewController

//When the view is loaded we add ourselves to the notification center watch list
- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.layer.contents = (id)[UIImage imageNamed:@"background.png"].CGImage;

}

@end
