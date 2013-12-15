//
//  MolecularViewerAppDelegate.m
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/13.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "MolecularViewerAppDelegate.h"
#import "MolecularViewerViewController.hh"

@implementation MolecularViewerAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize nav;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
//	viewController = [[MolecularViewerViewController alloc] init];
	nav = [[UINavigationController alloc] initWithRootViewController:viewController];
	[nav setNavigationBarHidden:YES animated:YES];
//	[window makeKeyAndVisible];
	
	[self.window addSubview:nav.view];
// The following is not supported on iOS < 4.0
//    self.window.rootViewController = nav;
    return YES;
}

//- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
//{
//	if (url == nil)
//		return NO;
//	
//	NSString *path = [url path];
//	[viewController loadMolecule:path];
//}	

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    if (url != nil)
    {
		NSLog(@"opening url");
        return YES;
    }
    return NO;
}

- (void)dealloc
{
    [viewController release];
	[nav release];
    [window release];
    
    [super dealloc];
}

@end
