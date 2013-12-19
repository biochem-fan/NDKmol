//
//  SettingViewController.h
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/16.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MolecularViewerViewController.hh"

@interface SettingViewController : UIViewController <UIApplicationDelegate, UITableViewDataSource>{
	MolecularViewerViewController *vc;
	UITableView *table;
}

@property (nonatomic, retain) IBOutlet UITableView *table;

@end
