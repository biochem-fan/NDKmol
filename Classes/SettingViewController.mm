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

#import "SettingViewController.h"
#import "MolecularViewerAppDelegate.h"

@implementation SettingViewController

@synthesize table;

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 6;
} 

// IMPORTANT TODO: When item order is changed, representation constants in NdkView.hpp must be modified!

- (NSInteger)tableView:(UITableView *)table numberOfRowsInSection:(NSInteger)section {
    switch(section) {
        case 0: // Protein / Nucleic acid style
            return 7;
            break;
        case 1: // Ligand / Heteroatom style
            return 4;
            break;
		case 2: // Nucleotide base style
            return 3;
            break;
		case 3: // Color
            return 5;
            break;
		case 4: // Symmetry mates
            return 3;
            break;
		case 5: // Other view options
			return 5;
			break;
    }
    return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    switch(section) {
        case 0:
            return @"Protein / Nucleic acid style";
            break;
        case 1:
            return @"Ligand / Heteroatom style";
            break;
		case 2:
            return @"Nucleotide base style";
            break;
		case 3:
            return @"Color";
            break;
		case 4:
            return @"Symmetry mates";
            break;
		case 5:
            return @"Other view options";
            break;
    }
    return nil;
} 

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
	
	switch(indexPath.section) {
        case 0:
			vc.proteinMode = indexPath.row;
			break;
		case 1:
			vc.hetatmMode = indexPath.row;
            break;
		case 2:
			vc.nucleicAcidMode = indexPath.row;
            break;
		case 3:
			vc.colorMode = indexPath.row;
            break;
		case 4:
			vc.symmetryMode = indexPath.row;
            break;
    }
	[tableView reloadData];	
}

- (void)resetView:(id)sender {
	[vc resetView];
	[table reloadData];
}
	
- (void)showSidechainSwitched:(id)sender {
    vc.showSidechain = [(UISwitch *)sender isOn];
}

- (void)doNotSmoothenSwitched:(id)sender {
    vc.doNotSmoothen = [(UISwitch *)sender isOn];
}

- (void)symopHetatmsSwitched:(id)sender {
    vc.symopHetatms = [(UISwitch *)sender isOn];
}

- (void)showUnitcellSwitched:(id)sender {
    vc.showUnitcell = [(UISwitch *)sender isOn];
}

- (void)showSolventsSwitched:(id)sender {
    vc.showSolvents = [(UISwitch *)sender isOn];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {	
	UITableViewCell *cell;
	if (indexPath.section == 5) {
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
									  reuseIdentifier:@"switchCell"];			
		UISwitch *switchObj = [[UISwitch alloc] initWithFrame:CGRectMake(1.0, 1.0, 20.0, 20.0)];

		switch (indexPath.row) {
			case 0:
				cell.textLabel.text = @"Show Side Chains";
				switchObj.on = vc.showSidechain;
				[switchObj addTarget:self
							  action:@selector(showSidechainSwitched:)
					forControlEvents:UIControlEventValueChanged];
				break;
			case 1:
				cell.textLabel.text = @"Show Unit Cell";
				switchObj.on = vc.showUnitcell;
				[switchObj addTarget:self
							  action:@selector(showUnitcellSwitched:)
					forControlEvents:UIControlEventValueChanged];
				break;
			case 2:
				cell.textLabel.text = @"Show Solvents";
				switchObj.on = vc.showSolvents;
				[switchObj addTarget:self
							  action:@selector(showSolventsSwitched:)
					forControlEvents:UIControlEventValueChanged];
				break;
			case 3:
				cell.textLabel.text = @"Don't smoothen beta-sheets";
				switchObj.on = vc.doNotSmoothen;
				[switchObj addTarget:self
							  action:@selector(doNotSmoothenSwitched:)
					forControlEvents:UIControlEventValueChanged];
				break;
			case 4:
				cell.textLabel.text = @"Show symmetry mates for HETATMs";
				switchObj.on = vc.symopHetatms;
				[switchObj addTarget:self
							  action:@selector(symopHetatmsSwitched:)
					forControlEvents:UIControlEventValueChanged];
				break;
		}
		cell.accessoryView = switchObj;
		[switchObj release];
	} else {	
		static NSString *section1 = @"checkableCell";
		cell = [tableView dequeueReusableCellWithIdentifier:section1];
		if (!cell) {
			cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
										  reuseIdentifier:section1];
		}
		cell.accessoryType = UITableViewCellAccessoryNone;
		
		switch(indexPath.section) {
			case 0:
				if (vc.proteinMode == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
				switch (indexPath.row) {
					case 0:
						cell.textLabel.text = @"Thick Ribbon";
						break;
					case 1:
						cell.textLabel.text = @"Thin Ribbon (faster)";
						break;						
					case 2:
						cell.textLabel.text = @"C alpha Trace";
						break;
					case 3:
						cell.textLabel.text = @"Strand";
						break;
					case 4:
						cell.textLabel.text = @"B factor Tube";
						break;
					case 5:
						cell.textLabel.text = @"Bonds (everything)";
						break;						
					case 6:
						cell.textLabel.text = @"None";
						break;
				}
				break;
			case 1:
				if (vc.hetatmMode == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
				switch (indexPath.row) {
					case 0:
						cell.textLabel.text = @"Sphere";
						break;
					case 1:
						cell.textLabel.text = @"Stick";
						break;
					case 2:
						cell.textLabel.text = @"Line (Fast but Ugly)";
						break;
					case 3:
						cell.textLabel.text = @"None";
						break;
				}
				break;
			case 2:
				if (vc.nucleicAcidMode == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
				switch (indexPath.row) {
					case 0:
						cell.textLabel.text = @"Line";
						break;
					case 1:
						cell.textLabel.text = @"Polygon";
						break;
					case 2:
						cell.textLabel.text = @"None";
						break;
				}
				break;
			case 3:
				if (vc.colorMode == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
				switch (indexPath.row) {
					case 0:
						cell.textLabel.text = @"Spectrum";
						break;
					case 1:
						cell.textLabel.text = @"By Chain";
						break;
					case 2:
						cell.textLabel.text = @"By Secondary Structure";
						break;
					case 3:
						cell.textLabel.text = @"Polar / Non-polar";
						break;
					case 4:
						cell.textLabel.text = @"B Factor";
						break;
				}
				break;
			case 4:
				if (vc.symmetryMode == indexPath.row) cell.accessoryType = UITableViewCellAccessoryCheckmark;
				switch (indexPath.row) {
					case 0:
						cell.textLabel.text = @"None";
						break;
					case 1:
						cell.textLabel.text = @"Biological Assembly";
						break;
					case 2:
						cell.textLabel.text = @"Crystal Packing";
						break;
				}
				break;
		}
	}
	return cell;
}

// The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
/*
 - (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
 self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
 if (self) {
 // Custom initialization.
 }
 return self;
 }
 */

/*
 // Implement loadView to create a view hierarchy programmatically, without using a nib.
 - (void)loadView {
 }
 */

- (void)viewDidLoad {
    [super viewDidLoad];
	
	[self.navigationController setNavigationBarHidden: FALSE];
	UIBarButtonItem *resetButton = [[UIBarButtonItem alloc] initWithTitle:@"Reset view" 
								style:UIBarButtonItemStylePlain target:self action:@selector(resetView:)];
	self.navigationItem.rightBarButtonItem = resetButton;
	[resetButton release];
	self.navigationItem.title = @"View Settings";
	vc = [self.navigationController.viewControllers objectAtIndex:
		  [self.navigationController.viewControllers count] - 2];
}

/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations.
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
