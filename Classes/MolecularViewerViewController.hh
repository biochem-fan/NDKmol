//
//  MolecularViewerViewController.h
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/13.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

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
