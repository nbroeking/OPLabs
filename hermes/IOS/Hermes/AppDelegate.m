//
//  AppDelegate.m
//  Hermes
//
//  Created by Nicolas Charles Herbert Broeking on 1/17/15.
//  Copyright (c) 2015 NicolasBroeking. All rights reserved.
//

#import "AppDelegate.h"
#import "HomeViewController.h"
#import "MainNavigationController.h"
#import "SessionData.h"
#import "TestState.h"

@interface AppDelegate () <UISplitViewControllerDelegate>

@end

@implementation AppDelegate
@synthesize comm;
@synthesize tester;

//Right as the app first sets up we initlize our subsystems
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    NSLog(@"Application did Launch");
    [[SessionData getData] sync];
    comm = [Communication getComm];
    tester = [Tester getTester];
    
    [(MainNavigationController*)self.window.rootViewController setCommunicator:comm];
    [(MainNavigationController*)self.window.rootViewController setTester:tester];
    [tester start];
    [comm start];
    
    //Force app to always go back to home on relaunch
    [(MainNavigationController*)self.window.rootViewController popToRootViewControllerAnimated:NO];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    //Do Nothing But we need this method here to be a AppDelegate
}

//Called before the app enters the background
- (void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"Application did enter background");
    [comm stop];
    [tester stop];
}

// Called as part of the transition from the background to the inactive state;
- (void)applicationWillEnterForeground:(UIApplication *)application {
    
    NSLog(@"Application Will Enter Foreground");
    [[SessionData getData] sync];
    [comm start];
    [tester start];
    
    //Force app to always go back to home on relaunch
    //(MainNavigationController*)self.window.rootViewController popToRootViewControllerAnimated:NO];
}
// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, refresh the user interface.
- (void)applicationDidBecomeActive:(UIApplication *)application {
    NSLog(@"Application Did Become Active");
}

//Called when the user is trying to terminate the app. We clean up all our memory references and subsystems here
- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    // Saves changes in the application's managed object context before the application terminates.
    [self saveContext];
    [comm forceStop];
    [tester forceStop];
    comm = nil;
    tester = nil;
}

//Currently not used but is there for saving data local to the phone in future versions
#pragma mark - Core Data stack

@synthesize managedObjectContext = _managedObjectContext;
@synthesize managedObjectModel = _managedObjectModel;
@synthesize persistentStoreCoordinator = _persistentStoreCoordinator;

- (NSURL *)applicationDocumentsDirectory {
    // The directory the application uses to store the Core Data store file. This code uses a directory named "Nicolas.Broeking.Hermes" in the application's documents directory.
    return [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
}

- (NSManagedObjectModel *)managedObjectModel {
    // The managed object model for the application. It is a fatal error for the application not to be able to find and load its model.
    if (_managedObjectModel != nil) {
        return _managedObjectModel;
    }
    NSURL *modelURL = [[NSBundle mainBundle] URLForResource:@"Hermes" withExtension:@"momd"];
    _managedObjectModel = [[NSManagedObjectModel alloc] initWithContentsOfURL:modelURL];
    return _managedObjectModel;
}

- (NSPersistentStoreCoordinator *)persistentStoreCoordinator {
    // The persistent store coordinator for the application. This implementation creates and return a coordinator, having added the store for the application to it.
    if (_persistentStoreCoordinator != nil) {
        return _persistentStoreCoordinator;
    }
    
    // Create the coordinator and store
    
    _persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:[self managedObjectModel]];
    NSURL *storeURL = [[self applicationDocumentsDirectory] URLByAppendingPathComponent:@"Hermes.sqlite"];
    NSError *error = nil;
    NSString *failureReason = @"There was an error creating or loading the application's saved data.";
    if (![_persistentStoreCoordinator addPersistentStoreWithType:NSSQLiteStoreType configuration:nil URL:storeURL options:nil error:&error]) {
        // Report any error we got.
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        dict[NSLocalizedDescriptionKey] = @"Failed to initialize the application's saved data";
        dict[NSLocalizedFailureReasonErrorKey] = failureReason;
        dict[NSUnderlyingErrorKey] = error;
        error = [NSError errorWithDomain:@"YOUR_ERROR_DOMAIN" code:9999 userInfo:dict];
        // Replace this with code to handle the error appropriately.
        // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
        NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
        abort();
    }
    
    return _persistentStoreCoordinator;
}


- (NSManagedObjectContext *)managedObjectContext {
    // Returns the managed object context for the application (which is already bound to the persistent store coordinator for the application.)
    if (_managedObjectContext != nil) {
        return _managedObjectContext;
    }
    
    NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
    if (!coordinator) {
        return nil;
    }
    _managedObjectContext = [[NSManagedObjectContext alloc] init];
    [_managedObjectContext setPersistentStoreCoordinator:coordinator];
    return _managedObjectContext;
}

#pragma mark - Core Data Saving support

- (void)saveContext {
    NSManagedObjectContext *managedObjectContext = self.managedObjectContext;
    if (managedObjectContext != nil) {
        NSError *error = nil;
        if ([managedObjectContext hasChanges] && ![managedObjectContext save:&error]) {
            // Replace this implementation with code to handle the error appropriately.
            // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
            NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
            abort();
        }
    }
}

@end
