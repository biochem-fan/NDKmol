/*  NDKmol - Molecular Viewer on Android NDK and iOS
 
 (C) Copyright 2011 - 2013, biochem_fan
 
 This file is part of NDKmol.
 
 NDKmol is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
