//
//  DetailViewController.h
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 11/9/14.
//  Copyright (c) 2014 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController

@property (strong, nonatomic) id detailItem;
@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;

@end

