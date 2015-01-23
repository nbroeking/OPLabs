//
//  Created by Nicolas Charles Herbert Broeking on 1/17/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>

#import "Tester.h"

@interface TesterTests : XCTestCase

@end
@implementation TesterTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

-(void) testTesterInit{
    Tester* testComm = [[Tester alloc] init];
    XCTAssert([testComm isRunning] == false, @"Pass");
}

-(void) testTesterRunning{
    Tester* testComm = [[Tester alloc] init];
    [testComm start];
    XCTAssert([testComm isRunning] == true, @"Pass");
    
}
-(void) testTesterDoubleRunning{
    Tester* testComm = [[Tester alloc] init];
    [testComm start];
    [testComm start];
    XCTAssert([testComm isRunning], @"Pass");
}
-(void) testTesterStoped {
    Tester* testComm = [[Tester alloc] init];
    [testComm start];
    [testComm stop];
    sleep(1); //Takes a second for the thread to finish its closing procedure It wont report finished untill this is complete
    
    XCTAssert([testComm isRunning] == false, @"Pass");
}

-(void) testTesterDoubleStoped {
    Tester* testComm = [[Tester alloc] init];
    [testComm start];
    [testComm stop];
    [testComm stop];
    sleep(1); //Takes a second for the thread to finish its closing procedure It wont report finished untill this is complete
    
    XCTAssert([testComm isRunning] == false, @"Pass");
}
-(void) testTesterStopedAfterInit{
    Tester* testComm = [[Tester alloc] init];
    [testComm stop];
    sleep(1); //Takes a second for the thread to finish its closing procedure It wont report finished untill this is complete
    
    XCTAssert([testComm isRunning] == false, @"Pass");
}
@end
