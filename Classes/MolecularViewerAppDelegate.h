//
//  MolecularViewerAppDelegate.h
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/13.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MolecularViewerViewController;

@interface MolecularViewerAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MolecularViewerViewController *viewController;
	UINavigationController *nav;
	
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet MolecularViewerViewController *viewController;
@property (nonatomic, retain) UINavigationController *nav;

@end

