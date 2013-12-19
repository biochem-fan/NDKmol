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

