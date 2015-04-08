//
//  Created by Nicolas Charles Herbert Broeking on 1/17/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "SessionData.h"

#import "Tester.h"

@interface DataTests : XCTestCase
@property SessionData *data;
@end

@implementation DataTests
@synthesize data;

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    data = [SessionData getData];
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

-(void) testBasicChange{
    
    [data setPassword:@"Hi"];
    [data setEmail:@"hello"];
    [data setHostname:@"hostname"];
    [data setSessionId:@"12345"];
    
    XCTAssert([[data  sessionId] isEqualToString: @"12345"], @"Pass");
    XCTAssert([[data  email] isEqualToString: @"hello"], @"Pass");
    XCTAssert([[data  password] isEqualToString: @"Hi"], @"Pass");
    XCTAssert([[data  hostname] isEqualToString: @"hostname"], @"Pass");
}

-(void) testSingletonChange{
    
    SessionData *yolo = [SessionData getData];
    
    [yolo setSessionId:@"test"];
    [yolo setPassword:@"yolo"];
    [yolo setHostname:@"yuk"];
    [yolo setEmail:@"junk"];
    
    [data setPassword:@"Hi"];
    [data setEmail:@"hello"];
    [data setHostname:@"hostname"];
    [data setSessionId:@"12345"];
    
    XCTAssert([[yolo  sessionId] isEqualToString: @"12345"], @"Pass" );
    XCTAssert([[yolo  email] isEqualToString:@"hello"], @"Pass");
    XCTAssert([[yolo  password] isEqualToString: @"Hi"], @"Pass");
    XCTAssert([[yolo  hostname] isEqualToString:@"hostname"], @"Pass");
}
@end
