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

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "NdkView.h"
#include "Quaternion.h"

#include "math.h"

@interface MolecularViewerViewController : UIViewController
{
    EAGLContext *context;
    GLuint program;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
	
	// viewpoint
	float objX, objY, objZ, cameraZ, slabNear, slabFar;
	Quaternion rotationQ, currentQ;

	// for touch events
	float startX, startY;
	float currentX, currentY, currentZ, currentSlabNear, currentSlabFar, currentCameraZ;
	BOOL settingOpened;

	@public
	
	int proteinMode, hetatmMode , symmetryMode, colorMode, nucleicAcidMode;
	bool showSidechain, showUnitcell, showSolvents, doNotSmoothen, symopHetatms;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic) BOOL settingOpened;
@property (nonatomic) int proteinMode, hetatmMode, symmetryMode, colorMode, nucleicAcidMode;
@property (nonatomic) bool showSidechain, showUnitcell, showSolvents, doNotSmoothen, symopHetatms;

- (void)resetView;
- (void)drawFrame;
- (void)startAnimation;
- (void)stopAnimation;
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)loadMolecule: (NSString *) fileName;
@end
